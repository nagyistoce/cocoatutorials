/* @(#)CM_VerSion xatmbitmap.c 3010 1.4 19246.eco sum= 25418 3010.005 */
/* @(#)CM_VerSion xatmbitmap.c 2017 1.8 17841.eco sum= 12415 2017.013 */
/* @(#)CM_VerSion xatmbitmap.c 2016 1.2 13826.eco sum= 02000 */
/* @(#)CM_VerSion xatmbitmap.c 2015 1.5 12669.eco sum= 04647 */
/*
  xatmbitmap.c

(c) Copyright 1991-1994 Adobe Systems Incorporated.
All rights reserved.  All information contained herein is the property
of Adobe Systems Incorporated or its Licensors, and are protected trade
secrets and copyrights, and may be covered by U.S. and foreign patents
or patents pending and/or mask works.  Any reproduction or dissemination
of any portion of this document or of software or other works derived
from it is strictly forbidden unless prior written permission is
obtained from Adobe Systems Incorporated.

Patents Pending

PostScript and Display PostScript are trademarks of Adobe Systems
Incorporated which may be registered in certain jurisdictions.

*/

/*
This file contains the interface functions given back to X as the entry
points for character requests.  There are three main functions - one to get
the character glyphs, another to just get the character metrics, and another
to destroy a font once it is no longer needed.
*/

/*-----------------------------------------*/
/* Public interfaces from PostScript world */
/*-----------------------------------------*/

#include PACKAGE_SPECS
#include PUBLICTYPES
#include EXCEPT
#include DEVICETYPES
#include FRAMEPIXEL
#include PSLIB

/*----------------------------*/
/* Include files from X world */
/*----------------------------*/

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "xatmfont.h"
#include "xatmbitmap.h"
#include "xatmproperty.h"
#include "xatminterpreter.h"
#include "X_internals.h"

/*
  =============================
 
  Private Function Declarations

*/

#define FONTCACHESIZE	5

extern integer  additionalMaskCount;
extern PMask	additionalMasks;

PUBLIC int	XatmGetGlyphs ARGDECL6(FontPtr, font, Card32, count, 
				       register PCard8, chars, FontEncoding,
				       charEncoding, PCard32, glyphCount,
				       CharInfoPtr *, glyphsBase);

PRIVATE int	XatmGetMetrics ARGDECL6(FontPtr, font, Card32, count, 
					register PCard8, chars, 
					FontEncoding, charEncoding, 
					PCard32, glyphCount,
					xCharInfo **, glyphsBase);

PUBLIC char		*lastSelectedFont = NULL;
PUBLIC Mtx		lastSelectedMtx = {-10.0, -10.0, -1.0, -10.0, -10.0, 
					   -10.0};
PRIVATE void	XatmUnloadFont ARGDECL1(FontPtr, font);

  /* Scratch area for doing bitmap conversions.				   */
PRIVATE charptr		scratchBase, scratchPtr;
PRIVATE Card32		scratchSize;

  /* Scratch area for storing glyph information temporarily.		   */
PRIVATE CharInfoPtr	infoBase, infoPtr;
PRIVATE Card32		infoSize;

  /* Area for keeping a small open-font cache.				   */
#if	FONTCACHESIZE > 0
PRIVATE charptr	fontNames[FONTCACHESIZE];
PRIVATE Int16	whichFont;
#endif	/* FONTCACHESIZE */

/*
  ===========================
  
  Public Function Definitions

*/

/* 

XatmLoadBitmapFuncs

This is the only function which knows about the other functions in this
module.  It also knows about the FontRec structure and where to stick the
procedure pointers to the bitmap manipulation functions.  Which is what it
does.

*/

PUBLIC procedure XatmLoadBitmapFuncs ARGDEF1(FontPtr, font)
{
  if (font != NULL) 
  {
    font->get_glyphs = XatmGetGlyphs;
    font->get_metrics = XatmGetMetrics;
    font->unload_font = XatmUnloadFont;
  }
}
     

/*
  ============================

  Private Function Definitions

*/

/*

XatmPrepareScratch

This function controls the size of the scratch area.  It will shrink the
scratch are if it has grown too large.  It returns false if something went
wrong with the resizing.  It will allocate the scratch space if it has not
yet been allocated.

This function also calls into XatmCleanUpMasks, asking it to de-reference any
masks that were referenced the last build.  The masks must be referenced
until we know they are not needed again, which is the next time through
either XatmGetGlyphs or XatmGetMetrics.

*/

#define MINSCRATCH	(12 * sizeof(long int) * 256)
#define MAXSCRATCH	(24 * sizeof(long int) * 256)

PRIVATE boolean XatmPrepareScratch ARGDEF2(int, count, boolean, compatible)
{
  /* Make sure the info array is large enough to hold all the possible
     character information.						   */
  if (infoBase == NIL)
  {
    infoSize = 256;
    if (count > infoSize) infoSize = count;

    infoBase = (CharInfoPtr)sys_malloc(infoSize * sizeof(CharInfoRec));	
    if (infoBase == NIL) return AllocError;
  }
  else if (count > infoSize)
  {
    infoSize = count;
    infoPtr = (CharInfoPtr)sys_realloc(infoBase, 
				       infoSize * sizeof(CharInfoRec));
    if (infoPtr == NIL) return AllocError;
    infoBase = infoPtr;
  }
  infoPtr = infoBase;

  /* Clear out the array.						   */
  os_bzero(infoBase, (infoSize * sizeof(CharInfoRec)));

  /* Allocate enouch scratch space to completely hold a 12 point font.	   */
  if (!scratchBase && !compatible) 
  {
    scratchBase = (charptr)sys_malloc(MINSCRATCH);
    scratchPtr = scratchBase;
    if (scratchBase == NIL)
    {
      scratchSize = 0;
      return false;
    }
    scratchSize = MINSCRATCH;
    return true;
  }

  if (scratchSize > MAXSCRATCH)
  {
    /* Free the current scratch space.					   */
   sys_free(scratchBase);
    scratchSize = 0;

    /* Allocate one big enough to hold all of a 12 point font.		   */
    scratchBase = (charptr)sys_malloc(MINSCRATCH);
    scratchPtr = scratchBase;
    if (scratchBase == NIL)
      return false;
    
    scratchSize = MINSCRATCH;
    return true;
  }

  /* Do nothing if the scratch space is allocated and is of a decent size. */
  return true;
}

/*

XatmCompatibleFormat

This function determines if Xatm can simply return the glyphs to X as they
come out of the PostScript mask cache.  Hopefully, it can.  This avoids
copying the data and mucking with it for each GetGlyphs request.

*/
PRIVATE boolean XatmCompatibleFormat ARGDEF2(fsBitmapFormat, format,
					     fsBitmapFormatMask, fmask)
{
  int	bitOrder, byteOrder, scanUnit, glyphPad, junk;

  /* Set up the defaults.						   */
#if	SWAPBITS
  bitOrder = LSBFirst;
  byteOrder = LSBFirst;
#else	/* SWAPBITS */
  bitOrder = MSBFirst;
  byteOrder = MSBFirst;
#endif	/* SWAPBITS */  
  scanUnit = SCANUNIT / 8;
  glyphPad = SCANUNIT / 8;

  /* Get the proper format bits for checking.				   */
  CheckFSFormat(format, fmask, &bitOrder, &byteOrder, 
		&scanUnit, &glyphPad, &junk);

  /* Bit ordering compatiblity.						   */
  if ((bitOrder == MSBFirst) == (SWAPBITS == DEVICE_CONSISTENT))
    return false;

  /* Byte ordering compatibility.					   */
  if (SWAPBITS == (byteOrder == MSBFirst))
    return false;
    
  /* Check scanunit size.						   */
  if (glyphPad != (SCANUNIT / 8))
    return false;

  return true;
}


/*

XatmComaptibleGlyph

This function takes a pointer to some mask data and converts it into the
format required by the X server for returning.  The bitmaps are all converted
into a scratch space, which must be reallocated if it fills up.  This
function returns true if the bitmap could be converted, false if there was
some kind of memory problem, or whatever.  If the returned offset value is
non-zero, it means the scratch area has to be reallocated and the pointers
returned previously by this function need to be bumped by the returned offset
value.

*/

#define NEWSCRATCHSIZE(oldSize) 	((oldSize) * 2)

PRIVATE boolean XatmCompatibleGlyph ARGDEF5(CharInfoPtr, data,
					     fsBitmapFormat, format,
					     fsBitmapFormatMask, fmask,
					     charptr *, converted,
					     PInt32, offset)
  /*
    CharInfoPtr data		INPUT
    fsBitmapFormat format	INPUT
    fsBitmapFormatMask fmask	INPUT
    charptr *converted		OUTPUT
    PInt32 offset		OUTPUT
  */
{
  int		bitOrder, byteOrder, scanUnit, glyphPad, junk;
  Card32	convertSize;
  boolean	realloced;

  *offset = 0L;
  if (scratchBase == NIL) return false;

  /* Set up the defaults.						   */
#if	SWAPBITS
  bitOrder = LSBFirst;
  byteOrder = LSBFirst;
#else	/* SWAPBITS */
  bitOrder = MSBFirst;
  byteOrder = MSBFirst;
#endif	/* SWAPBITS */  
  scanUnit = SCANUNIT / 8;
  glyphPad = SCANUNIT / 8;

  /* Get the proper format bits for conversion.				   */
  CheckFSFormat(format, fmask, &bitOrder, &byteOrder, 
		&scanUnit, &glyphPad, &junk);

  /* First, figure out the size of the new bitmap, in bytes.		   */
  convertSize = (GLYPHWIDTHBYTESPADDED((data->metrics.rightSideBearing -
				       data->metrics.leftSideBearing), 
				      glyphPad) * 
		 (data->metrics.ascent + data->metrics.descent));

  /* Check to see if we still have room in the scratch area for the converted
     bitmap.								   */
  if (((scratchPtr - scratchBase) + convertSize) > scratchSize)
  {
    charptr	tempPtr;

    realloced = true;

    /* Try and bump up the size of the scratch bitmap.			   */
    tempPtr = (charptr)sys_realloc(scratchBase, NEWSCRATCHSIZE(scratchSize));
    if (tempPtr == NIL)
      return false;

    /* Figure out the offset to go from pointers into the old scratch area to
       pointers into the new scratch area.				   */
    *offset = tempPtr - scratchBase;
    scratchBase += *offset;
    scratchPtr += *offset;
    scratchSize = NEWSCRATCHSIZE(scratchSize);
  }

  /* We have enough space, so move the character from where it is to it's new
     spot, and complete the conversion.  If we need to repad, repad as we
     move, both to avoid overlapping src/dest problems, and to reduce the
     number of times the data is copied.				   */
  if (glyphPad != (SCANUNIT / 8))
    RepadBitmap(data->bits, (char*)scratchPtr, (SCANUNIT / 8), glyphPad,
		(data->metrics.rightSideBearing - 
		 data->metrics.leftSideBearing),
		(data->metrics.ascent + data->metrics.descent));
  else 
    os_bcopy(data->bits, scratchPtr, convertSize);

  /* Check for the need to reverse the bits.				   */
  if ((bitOrder == MSBFirst) == (SWAPBITS == DEVICE_CONSISTENT))
    BitOrderInvert(scratchPtr, convertSize);

  /* Check for the need to do any byte swapping.			   */
  if (SWAPBITS == (byteOrder == MSBFirst)) 
  {
    switch (bitOrder == byteOrder ? (SCANUNIT / 8) : scanUnit)
    {
      case 2:
      TwoByteSwap(scratchPtr, convertSize);
      break;
      
      case 4:
      FourByteSwap(scratchPtr, convertSize);
      break;
      
      default:
      break;
    }
  }

  scratchPtr += convertSize;
  return true;
}

boolean XatmCopyGlyphMaskBits ARGDEF5(CharInfoPtr, source,
				      CharInfoPtr, dest,
				      int, glyphPad,
				      char **, maskSpace,
				      char *,  maskLimit)

{
  *dest = *source;

  if (source->bits)
  {
    Card32 bitsSize  = (GLYPHWIDTHBYTESPADDED((source->metrics.rightSideBearing -
					       source->metrics.leftSideBearing),
					      glyphPad) *
			(source->metrics.ascent + source->metrics.descent));
    /* 2ps_xdps.499 */
    if ( *maskSpace + bitsSize >= maskLimit){
      /* printf( "XatmCopyGlyphMaskBits: Cannot copy mask\n" ); */
      return false;
    }
    os_bcopy(source->bits, *maskSpace, bitsSize);
    dest->bits = *maskSpace;
    *maskSpace += bitsSize;
  }
  return true; /* 2ps_xdps.499 */
}

PRIVATE void XatmCacheGlyphs ARGDEF2(FontPtr, font, FontEncoding, charEncoding)
{
  PXatmPrivateInfo	privateInfo;
  boolean		cacheable;
  charptr		*encodingStrings;
  Card32		value, numMasks = 0;
  CharInfoPtr		glyphs, glyphsBase =  NULL; /* 2ps_xdps 502 */
  int                   glyphPad = SCANUNIT/8;
  Card8			chars[256];
  char			*masks, *masksBase = (char *)NULL;
  char			*maskLimit;
  Card32		maskBytesNeeded;
  int			i;
  static boolean        disableGlyphCaching = false;

  privateInfo = (PXatmPrivateInfo)font->devPrivates[XatmDevPrivatesIndex];

  DebugAssert( privateInfo->isCID == 0 ); /* 2ps_xdps.473 */
  DebugAssert( privateInfo->isType0 == 0 );
  if (disableGlyphCaching)
  {
    privateInfo->cacheStatus = uncacheable;
    return;
  }

  if( font->info.firstRow != font->info.lastRow || 
      ( (font->info.lastCol-font->info.firstCol) > 256 ) )
    goto cannotCacheGlyphs;
  if ( privateInfo->isCID || privateInfo->isType0 )
    goto cannotCacheGlyphs;

  cacheable = XatmCompatibleFormat(font->format,
				   (fsBitmapFormatMask)-1);
  if (!cacheable)
    goto cannotCacheGlyphs;

  if (!XatmPrepareScratch(256, cacheable))
    goto cannotCacheGlyphs;

  if (XatmGetProperty(font, "CHARSET_ENCODING", &value) != encodingType)
    goto cannotCacheGlyphs;
  encodingStrings = XatmGetEncodingStrings(value);

  if ( (glyphsBase = (CharInfoPtr)sys_malloc(256*sizeof(struct _CharInfo)))
      == NULL)
      goto cannotCacheGlyphs;
  glyphs = glyphsBase;

  maskBytesNeeded = (256 *
		     GLYPHWIDTHBYTESPADDED((font->info.maxbounds.rightSideBearing -
					    font->info.minbounds.leftSideBearing),
					   glyphPad) *
		     (font->info.maxbounds.ascent + font->info.maxbounds.descent));
  if ((masksBase = (char*)sys_malloc(maskBytesNeeded)) == (char *)NULL)
    goto cannotCacheGlyphs;
  masks = masksBase;
  maskLimit = masksBase+maskBytesNeeded;
  
  /* 2ps_xdps.459 */
  if( !lastSelectedFont ||
    (os_strcmp( lastSelectedFont,(const char *)privateInfo->type1FontName )!=0)
    || ( !MtxEqAlmost( &privateInfo->buildMatrix, &lastSelectedMtx ) ) 
    ){
    int len;

    if( lastSelectedFont != NULL ){
	  sys_free( lastSelectedFont );
	  lastSelectedFont = NULL;
    }
    len = os_strlen( (char *)privateInfo->type1FontName );
    lastSelectedFont = ( char * ) sys_malloc( len + 1 ); /* 2ps_xdps.503 */
    if( lastSelectedFont == NULL )
      goto cannotCacheGlyphs;
    os_strcpy( lastSelectedFont, (char *)privateInfo->type1FontName );
    lastSelectedMtx.a = (privateInfo->buildMatrix).a;
    lastSelectedMtx.b = (privateInfo->buildMatrix).b;
    lastSelectedMtx.c = (privateInfo->buildMatrix).c;
    lastSelectedMtx.d = (privateInfo->buildMatrix).d;
    lastSelectedMtx.tx = (privateInfo->buildMatrix).tx;
    lastSelectedMtx.ty = (privateInfo->buildMatrix).ty;
    if( privateInfo->isCID ){
  	XatmComposeFont(
			(char *)privateInfo->type1FontName, 
		      	(char *)privateInfo->fileName,
			(char *)privateInfo->cidFontName,
			(char *)privateInfo->encodingName
			);
    }
    if (!XatmSelectFont((char *)privateInfo->type1FontName, 
		      (char *)privateInfo->fileName,
		      &privateInfo->buildMatrix, privateInfo->isCID))
      goto cannotCacheGlyphs;
  }
  for ( i = 0; i < 256; i++)
    chars[i] = i;

  /* Get an information array filled with the characters.		   */
  if (!XatmGetCharacterMasks( chars, encodingStrings, 256,
			      false, false, (PXatmCharInfo)infoBase, 
			      &numMasks, privateInfo->isType0 ))
  {
    /* Failed building any characters.				   */
    goto cannotCacheGlyphs;
  }

  if (numMasks != 256)
      goto cannotCacheGlyphs;

  while (numMasks--)
  {
    if (!XatmCopyGlyphMaskBits(infoPtr, glyphs, glyphPad,
                               &masks, maskLimit))
        goto cannotCacheGlyphs;
    glyphs++;
    infoPtr++;
  }
  /* 2ps_xdps.499 */
  /* Since we have made deep copies of all masks, ok to free additional masks */
  FLUSH_ADDITIONAL_MASKS

  privateInfo->cacheStatus = cached;
  privateInfo->cachedGlyphs = glyphsBase;
  privateInfo->cachedMaskBits = masksBase; /* 2ps_xdps.499 */
  return;

 cannotCacheGlyphs:
  if (glyphsBase != NULL)
    (void)sys_free((char *)glyphsBase);
  privateInfo->cacheStatus = uncacheable;
  return;
}


PRIVATE int XatmGetCachedGlyphs ARGDEF6(FontPtr, font, Card32, count, 
				  register PCard8, chars, FontEncoding,
				  charEncoding, PCard32, glyphCount,
				  CharInfoPtr *, glyphsBase)
  /*-
    FontPtr font;		INPUT - pointer to X font structure.
    Card32 count;		INPUT - length of request string in bytes.
    register PCard8 chars;	INPUT - request string.
    FontEncoding charEncoding;	INPUT - interpretation of string.
    PCard32 glyphCount;		OUTPUT - number of pointers to glyph structures
    CharInfoPtr *glyphsBase;	OUTPUT - array of pointers to glyph structs
  */
{
  PXatmPrivateInfo	privateInfo;
  register CharInfoPtr	*glyphs, cachedGlyphs;
  int i, charSize;

  privateInfo = (PXatmPrivateInfo)font->devPrivates[XatmDevPrivatesIndex];

  cachedGlyphs = privateInfo->cachedGlyphs;
  glyphs = glyphsBase;

  DebugAssert( (charEncoding == Linear8Bit) || (charEncoding == TwoD8Bit) ||
	       (charEncoding == Linear16Bit) || (charEncoding == TwoD16Bit) );

  if( (charEncoding == Linear8Bit) || (charEncoding == TwoD8Bit) )
	charSize = 1;
  else
	charSize = 2;
  for (i=0; i<count; i++)
  {
    *glyphs++ = cachedGlyphs + chars[i*charSize+charSize-1];
  }
  *glyphCount = glyphs - glyphsBase;
  return Successful;
}

/*

XatmGetGlyphs

This function satifies an X requirest for glyphs out of a font.  Looping
through the characters in the request, this function asks PostScript for the
glyph.  PostScript will hopefully build it and return it (through the utility
functions in xatminterpreter.c).  Once we have the glyph, we may have to
massage it into the form of the request.  This function is ready to handle 16
bit requests.  The information is returned in two structures which are
allocated by the caller.

*/
PUBLIC int XatmGetGlyphs ARGDEF6(FontPtr, font, Card32, count, 
				  register PCard8, chars, FontEncoding,
				  charEncoding, PCard32, glyphCount,
				  CharInfoPtr *, glyphsBase)
  /*-
    FontPtr font;		INPUT - pointer to X font structure.
    Card32 count;		INPUT - length of request string in bytes.
    register PCard8 chars;	INPUT - request string.
    FontEncoding charEncoding;	INPUT - interpretation of string.
    PCard32 glyphCount;		OUTPUT - number of pointers to glyph structures
    CharInfoPtr *glyphsBase;	OUTPUT - array of pointers to glyph structs
  */
{
  PXatmPrivateInfo	privateInfo;
  register CardX	numCols, numRows, firstCol, firstRow;
  CardX			oneChar;
  register CharInfoPtr	*glyphs, infoOffset;
  charptr		*encodingStrings;
  boolean		compatible;
  Card32		value, numMasks = 0;

  /* Protection against failure in the middle.				   */
  *glyphCount = 0;

  /* Sanity check.							   */
  if (font == NULL) return BadFontFormat;
  if (sizeof(struct _CharInfo) != sizeof(XatmCharInfo)) return BadFontFormat;

  /* Get our private information structure out of the devPrivates array.   */
  privateInfo = (PXatmPrivateInfo)font->devPrivates[XatmDevPrivatesIndex];

  /* 2ps_xdps.499 */
  /* In both cases, viz., cached (i.e., Type1) and non cached fonts, we
     do NOT need to save the additional masks across XatmGetGlyphs calls.

     In case of Type1, only the first call to XatmGetGlyphs will call
     show and result in calls to XatmRunMark (or any other mark) which creates
     and uses additionalMasks. Before that XatmGetGlyphs returns, the masks
     will be deep copied into the private mask cache of the font. In fact,
     the additional masks will be freed right after that (see XatmCacheGlyphs).

     For non-cached fonts, nobody will ever refer to the glyphs/masks returned
     by previous calls to XatmGetGlyphs, so no point in saving them. There
     is the theoretical possibility that X server saves up (shallow copies of)
     the glyphs from
     old calls to GetGlyphs and uses them later, but that does not happen in
     practise. Since Xatm does DevRemRefMask on (noncached) masks, that would
     have failed in the first place */

  FLUSH_ADDITIONAL_MASKS

  if( !( privateInfo->isCID || privateInfo->isType0 ) ) {
      /* If we haven't tried to cache the glyphs yet, try to do so now */
      if (privateInfo->cacheStatus == cachingNotYetAttempted)
	  XatmCacheGlyphs(font, charEncoding);

      /* if the glyphs are cached, get the cached glyphs */
      if (privateInfo->cacheStatus == cached)
	  return XatmGetCachedGlyphs(font, count, chars, charEncoding,
				 glyphCount, glyphsBase);
  }

  /* Get the format that the request needs back.  If they are lucky, they are
     rquesting the same format that we like to build in.  Otherwise, I hope
     they feel patient.							   */
  compatible = XatmCompatibleFormat(font->format, (fsBitmapFormatMask)-1);

  /* Check to make sure the scratch space is allocated if neccesary.	   */
  if (!XatmPrepareScratch(count, compatible))
    return AllocError;

  /* Find out what encoding we are supposed to use.			   */
  if (XatmGetProperty(font, "CHARSET_ENCODING", &value) != encodingType)
    return BadFontFormat;
  encodingStrings = XatmGetEncodingStrings(value);

  /* Get some of the bounds of the matrix to impose on the encoding array we
     are going to use.							   */
  firstCol = font->info.firstCol;
  firstRow = font->info.firstRow;
  numCols = font->info.lastCol - firstCol + 1;
  numRows = font->info.lastRow - firstRow + 1;

  /* Save away the base address of the glyphs array the we are going to mess
     with, so that later the calculation of how many glyphs we stuck together
     is trivial.							   */
  glyphs = glyphsBase;
  
  /*2ps_xdps.502 */
  /* Select the font that we are going to be using if not already selected.*/
  if( !lastSelectedFont || 
    (os_strcmp(lastSelectedFont,(const char *)privateInfo->type1FontName )!=0) 
    || ( !MtxEqAlmost( &privateInfo->buildMatrix, &lastSelectedMtx ) ) 
    ){
    int len;

    if( lastSelectedFont != NULL ){
	  sys_free( lastSelectedFont );
	  lastSelectedFont = NULL;
    }
    len = os_strlen( (char *)privateInfo->type1FontName );
    lastSelectedFont = ( char * ) sys_malloc( len + 1 ); /* 2ps_xdps.503 */
    if( lastSelectedFont == NULL ){
       PDEBUGINFO("Xatm: Selecting font for build.");
       return AllocError;
    }
    os_strcpy( lastSelectedFont, (char *)privateInfo->type1FontName );
    lastSelectedMtx.a = (privateInfo->buildMatrix).a;
    lastSelectedMtx.b = (privateInfo->buildMatrix).b;
    lastSelectedMtx.c = (privateInfo->buildMatrix).c;
    lastSelectedMtx.d = (privateInfo->buildMatrix).d;
    lastSelectedMtx.tx = (privateInfo->buildMatrix).tx;
    lastSelectedMtx.ty = (privateInfo->buildMatrix).ty;
    if( privateInfo->isCID ){
  	XatmComposeFont(
			(char *)privateInfo->type1FontName, 
		      	(char *)privateInfo->fileName,
			(char *)privateInfo->cidFontName,
			(char *)privateInfo->encodingName
			);
    }
    if (!XatmSelectFont((char *)privateInfo->type1FontName, 
		      (char *)privateInfo->fileName,
		      &privateInfo->buildMatrix, privateInfo->isCID))
    {
      PDEBUGINFO("Xatm: Selecting font for build.");
      return AllocError;
    }
  } 
  /* Based on the desired interpretation of the string, switch to different
     parts of the code.							   */
  /* 2ps_xdps.473: There is hardly any difference in the way 
     XatmGetCharacterMasks is called in 8bit and 16bit case. Why devote
     so much code ( switch statement etc.) */
  if( !XatmGetCharacterMasks( chars, encodingStrings, count,
			      (numRows != 1), 
			      ( charEncoding != Linear8Bit && 
			        charEncoding != TwoD8Bit ), 
			      (PXatmCharInfo)infoBase, &numMasks,
			      privateInfo->isType0 ) )
    {
      /* Failed building any characters.				   */
      return AllocError;
    }

  /* Step through the character information records, looking for valid,
     filled in records.  If one is found, put a reference to it in the output
     pointer array.  It may need converting!				   */
  while (numMasks--)
  {
    /* If the bitmap is compatible, no worry.  Just stick it into the
       array.								   */
    if (compatible)
    {
      *glyphs++ = infoPtr;
    }
    else 
    {
      /* The bitmap is not compatible.  However, we only need to do a
	 conversion if the character has bits that need to be converted.
	 After that is done, stick the character into the output array.  */
      if (infoPtr->bits)
      {
	if (XatmCompatibleGlyph(infoPtr,
				font->format, (fsBitmapFormatMask)-1,
				(charptr *)&infoPtr->bits, (PInt32)&oneChar))
	{
	  if (oneChar != 0L)
	    for (infoOffset = infoBase ; infoOffset < infoPtr ; infoOffset++)
	      infoOffset->bits += oneChar;
	  *glyphs++ = infoPtr;
	}
      }
      else
      {
	*glyphs++ = infoPtr;
      }
    }

    infoPtr++;
  }

  /* ALWAYS return at least one character, even if it has no width and no
     bits, if at least one char was requested.  This is to work around a bug
     in QueryGlyphExtents when tryin to handle defaultChar - it doesn't
     check the return count, it simply assumes that the first pointer we
     assigned it is valid.                                                 */
  if (count > 0 && glyphsBase == glyphs)
  {
    /* Zero out the first character record.				   */
    infoPtr->bits = NULL;
    infoPtr->metrics.leftSideBearing = 0;
    infoPtr->metrics.rightSideBearing = 0;
    infoPtr->metrics.ascent = 0;
    infoPtr->metrics.descent = 0;
    infoPtr->metrics.attributes = 0;

    *glyphs++ = infoPtr;
  }

  /* Calculate how many glyphs that we put into the glyph return array.
     These are both of the same type, so the subtraction is valid.	   */
  *glyphCount = glyphs - glyphsBase;
  return Successful;
}


/*

XatmGetMetrics

This function returns the metrics of the characters requested in the same
format as XatmGetGlyphs.  There really is no way of getting per-character
bounding boxes without building the character.  So this function ends up
building all of the characters anyway.  However, it does have one major
advantage over actually getting the glyphs when talking to a machine with a
difference bit/byte ordering - instead of having to convert the bitmap, we
simply drop it on the floor.

2ps_xdps.459: By setting font.info.maxbounds = font.info.minbounds during
	      CID font opening, we prevent X from calling get_metrics for
	      each and every charcter in the font at the time of opening
	      the font. 
*/
PRIVATE int XatmGetMetrics ARGDEF6(FontPtr, pfont, Card32, count, 
				   register PCard8, chars, 
				   FontEncoding, charEncoding, 
				   PCard32, metricCount,
				   xCharInfo **, metricBase)
  /*-
    FontPtr pfont;		INPUT - pointer to X font structure.
    Card32 count;		INPUT - length of request string in bytes.
    register PCard8 chars;	INPUT - request string.
    FontEncoding charEncoding;	INPUT - interpretation of string.
    PCard32 metricCount;	OUTPUT - number of pointers to glyph structures
    xCharInfo **metricBase;	OUTPUT - array of pointers to glyph structs
  */
{
  int 			retval;

  *metricCount = 0;
  retval = XatmGetGlyphs( pfont, count, chars, charEncoding, metricCount, 
			  (CharInfoPtr *)metricBase );
  if( retval == Successful){
     *metricCount = count;
  }
  return retval;
}
      

/*

XatmUnloadFont

This function is very simple - it simply releases the few parts of memory
that were allocated in loading the font.  It also maintains a small cache of
fonts that we keep around before actually undefining them, in case we end up
flipping back and forth between sizes of a couple of fonts.

*/


PRIVATE void XatmUnloadFont ARGDEF1(FontPtr, font)
{
  PXatmPrivateInfo	privateInfo;

  /* If the font value is bogus, skip it.				   */
  if (font == NIL) return;

  /* If the Xatm privates structure was allocated, free the pieces of it, and
     then free the structure itself.					   */
  privateInfo = (PXatmPrivateInfo)font->devPrivates[XatmDevPrivatesIndex];
 
  if (privateInfo != NIL) {
    Int16	fontIndex;

#if FONTCACHESIZE > 0
    if (privateInfo->type1FontName != NIL)
    {
      /* Search through the names currently in the fontNames cache to see if
         the current name to be freed is already in there.		   */
      for (fontIndex = 0 ; fontIndex < whichFont ; fontIndex++)
      {
	/* If we found the string, move it to the front (to indicate recent
	   use) and free the privateInfo copy.			   */
	if (!os_strcmp((char *)fontNames[fontIndex], 
		    (char *)privateInfo->type1FontName))
	{
	  charptr	hold = fontNames[fontIndex];

	  for ( ; fontIndex > 0 ; fontIndex--)
	  {
	    fontNames[fontIndex] = fontNames[fontIndex-1];
	  }
	  fontNames[0] = hold;

	  (void)sys_free(privateInfo->type1FontName);
	  privateInfo->type1FontName = NIL;
	  break;
	}
      }

      /* If the privateInfo string is not empty, we need to insert the name
	 into the cache.  If the cache is full, undefine and free the
	 oldest name.							   */
      if (privateInfo->type1FontName != NIL)
      {
	/* Cache is full.						   */
	if (whichFont == FONTCACHESIZE)
	{
	  whichFont--;
	  XatmUndefineFont((char *)fontNames[whichFont]);
	  (void)sys_free(fontNames[whichFont]);
	  fontNames[whichFont] = NIL;
	}

	/* Move every entry up, and assign the new name to the "last used"
	   position - position 0.  Indicate that we have inserted a new
	   name into the cache by incrementing the whichFont count.	   */
	for (fontIndex = whichFont ; fontIndex > 0 ; fontIndex--)
	{
	  fontNames[fontIndex] = fontNames[fontIndex-1];
	}
	fontNames[0] = privateInfo->type1FontName;
	whichFont++;

	/* The privateInfo entry no longer owns the memory; zero it.	   */
	privateInfo->type1FontName = NIL;
      }
    }				/* privateInfo->type1FontName != NIL */
#else	/* FONTCACHESIZE > 0 */

    /* The cache is turned off; simply release the font and fontName.	   */
    if (privateInfo->type1FontName != NIL)
    {
      XatmUndefineFont(privateInfo->type1FontName);
      (void)sys_free(privateInfo->type1FontName);
      privateInfo->type1FontName = NIL;
    }
#endif	/* FONTCACHESIZE > 0 */

    /* Let go of the file name.						   */
    if (privateInfo->fileName != NIL)
      (void)sys_free((char *)privateInfo->fileName);

    /* if glyphs were cached, free their storage */
    if (privateInfo->cacheStatus == cached)
    {
      (void)sys_free((char*)privateInfo->cachedGlyphs);
      (void)sys_free((char*)privateInfo->cachedMaskBits); /* 2ps_xdps.499 */
    }

    /* Free the structure itself.					   */
    (void)sys_free((char *)privateInfo);
  }

  /* If any properties were allocated, release that memory.		   */
  if (font->info.props != NIL)
    (void)sys_free((char *)font->info.props);
  if (font->info.isStringProp != NIL)
    (void)sys_free((char *)font->info.isStringProp);

  /* If the devPrivates field was allocated, it also needs to be freed.	   */
  if (font->devPrivates != NIL)
    (void)sys_free((char *)font->devPrivates);

  /* All done.  Release the font record itself.				   */
  (void)sys_free((char *)font);
  /* 2ps_xdps.499 */
  FLUSH_ADDITIONAL_MASKS

  PDEBUGINFO("Xatm: Unloaded font...\n");
}
