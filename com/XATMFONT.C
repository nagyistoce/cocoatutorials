/* @(#)CM_VerSion xatmfont.c 3010 1.4 19246.eco sum= 19794 3010.005 */
/* @(#)CM_VerSion xatmfont.c 2017 1.9 17841.eco sum= 63172 2017.013 */
/* @(#)CM_VerSion xatmfont.c 2016 1.3 14560.eco sum= 52614 */
/* @(#)CM_VerSion xatmfont.c 2015 1.3 12669.eco sum= 09127 */
/*
  xatmfont.c

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


Entry points for the rasterization callbacks as registered with the fontfile
rasterizer.  These two functions are the ones called when X wants to use one
of our fonts.  ATMOpenScalable will be the main entry point.  Part of the
return value from this function is a set of function pointers used to
acutally obtain the glyphs from the font that is being opened.
*/

/*-----------------------------------------*/
/* Public interfaces from PostScript world */
/*-----------------------------------------*/

#include PACKAGE_SPECS
#include PUBLICTYPES
#include EXCEPT
#include <ctype.h>   /* USING isdigit() */
#include UNIXSTREAM
#include PSLIB

#if (OS != os_hpux)
#include <stdlib.h>  /* USING atoi() */
#endif

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

/*----------------
  Public variables

*/

PUBLIC integer	XatmDevPrivatesIndex;

#ifndef	BUFSIZE
#define BUFSIZE		1024
#endif	/* BUFSIZE */

/*-------------------- 
  Forward Declarations 

*/

PRIVATE procedure XatmCalculateMatrix ARGDECL2(FontPtr, font,
					       PXatmPrivateInfo, privateInfo);
PRIVATE boolean XatmSetFontInfo ARGDECL2(FontPtr, font,
					 PXatmPrivateInfo, privateInfo);
PRIVATE boolean XatmPostScriptFontInfo ARGDECL2(FontPtr, font,
						PXatmPrivateInfo, privateInfo);

extern int	XatmGetGlyphs ARGDECL6(FontPtr, font, Card32, count, 
				       register PCard8, chars, FontEncoding,
				       charEncoding, PCard32, glyphCount,
				       CharInfoPtr *, glyphsBase);
extern char		*lastSelectedFont;
extern Mtx		lastSelectedMtx;
/*

ATMOpenScalable

This is the main entry point for ATM rasterization.  We are handed an XLFD,
and from this mess of dashes and numbers, must deduce what font we really
want.  Once we know what font we want, we can try and load that font into our
PostScript context's VM.  Then we can gather all the information we need
about it in order to fill in the X font structure.

*/

PUBLIC integer ATMOpenScalable ARGDEF8(FontPathElementPtr, fpe, FontPtr *, 
				       pFont, int, flags, FontEntryPtr, entry,
				       char *, fileName, FontScalablePtr,
				       vals, fsBitmapFormat, format, 
				       fsBitmapFormatMask, fmask)
  /*- 
    FontPathElementPtr fpe	INPUT
    int flags			INPUT (font.h)
    FontEntryPtr entry		INPUT
    char *fileName		INPUT
    fsBitmapFormat format	INPUT
    fsBitmapFormatMask fmask	INPUT

    FontPtr *pFont		OUTPUT
    FontScalablePtr vals	OUTPUT
  */  
{
  char			fontName[MAXFONTNAMELEN];
  charptr		encodingName;
  Card32		newAtom, value;
  FontPtr		font;
  IntX			retValue;
  PXatmPrivateInfo	privateInfo;
  boolean		isCID, isType0;

  /* Check to make sure we are initialized.				   */
  if (!XatmMaybeInitialize()) return AllocError;

  /* Grab a copy of the X font name belonging to this request.		   */
  os_strncpy(fontName, entry->name.name, MAXFONTNAMELEN);
  fontName[MAXFONTNAMELEN-1] = '\0';
  
  /* Allocate the font record that we will need to fill in to return to the
     X server.  If at any point in this process an error occurs, we must
     remember to free this structure before returning the error.  Nearly all
     the information we need to satisfy X's requests for this font are
     contained in this structure.  The only additional piece needed is the
     Type 1 font name.  We'll have to pull it out of the font file itself,
     as the filename we are given isn't necessarily sufficient.	   */
  font = (FontPtr)sys_malloc(sizeof(FontRec));
  if (font == NIL) 
  {
    PDEBUGINFO("Xatm: Alloc Error - FontRec.\n");
    return AllocError;
  }   

  /* Initialize key portions of the structure - first, the routines that will
     let us release the font easily, and second, the structures which need to
     be NILed out in order for the free procedure to work properly.	   */
  XatmLoadBitmapFuncs(font);
  font->refcnt = 0;
  font->maxPrivate = -1;
  font->devPrivates = NIL;
  font->fontPrivate = NIL;
  font->info.props = NIL;
  font->info.isStringProp = NIL;

  /* Allocate our private information record.  This record contains the
     additional information needed to rasterize characters from this font -
     the Type 1 font name that PostScript needs in order to find and use the
     font, and the matrix that should be used for scaling the font by.	   */
  privateInfo = (PXatmPrivateInfo)sys_malloc(sizeof(XatmPrivateInfo));
  if (privateInfo == NIL)
  {
    PDEBUGINFO("Xatm: Alloc Error - Private Info.\n");
    (*font->unload_font)(font);
    return AllocError;
  }
  /* Zero out the pointer pieces in the privateInfo structure that will egt
     allocated later.							   */
  privateInfo->type1FontName = NIL;
  privateInfo->fileName = NIL;

  /* Set up the devPrivates private pointer array for the font by asigning
     our private field to the index we have alloated.			   */
  if (!_FontSetNewPrivate(font, XatmDevPrivatesIndex, privateInfo))
  {
    PDEBUGINFO("Xatm: Alloc Error - DevPrivate Assignment.\n");
    (*font->unload_font)(font);
    return AllocError;
  }

  /* Allocate and copy the filename for this font.			   */
  privateInfo->fileName = (charptr)sys_malloc(os_strlen(fileName) + 1);
  if (!privateInfo->fileName)
  {
    PDEBUGINFO("Xatm: Alloc Error - FileName.\n");
    (*font->unload_font)(font);
    return AllocError;
  }
  os_strcpy((char *)privateInfo->fileName, fileName);

  /* Set the values, or the font name, whichever is needed.  Note this has to
     be done before the properties are retrieved from the font name, in case
     the font size is contained within vals already, and not within the font
     name.								   */
  FontParseXLFDName(fontName, vals, FONT_XLFD_REPLACE_VALUE);

  privateInfo->cacheStatus = cachingNotYetAttempted;

  /* Call the property computing function.  This function tears apart the X
     font name from the request and inserts the values into the right places
     in the property tables that it allocates.  We can then call a companion
     function to get the values out of that table one at a time.	   */
  retValue = XatmComputeProps(font, fontName);
  if (retValue != Successful) 
  {
    PDEBUGINFO("ATM: Alloc error - Properties.\n");
    (*font->unload_font)(font);
    return retValue;
  }

  /* If the encoding string for the font is NIL, this means we have a
     composite font, so we need to send in the encoding name into the
     selectfont routine.  If it isn't NIL, then we have a roman font, and for
     that we do the encoding ourselves, manually.  The exception is the
     default encoding, for which the encoding strings return value is NIL,
     but so it the encoding name - so it works out correctly, just following
     an different path than other roman fonts.				   */
  (void)XatmGetProperty(font, "CHARSET_ENCODING", &value);
  if (value != (Card32)0 && XatmGetEncodingStrings(value) == NIL)
    encodingName = XatmGetEncodingName(value);
  else
    encodingName = NIL;

  /* Now, find out what the Type 1 font name is for this font.  This requires
     parsing a font file, searching for a keyword.  This utility function
     allocates memory on our behalf, so we must remember to free it when
     necessary.  The returned name includes the font's encoding name if it is
     not NULL.								   */	
  privateInfo->type1FontName = (charptr)XatmGetFontNameAndType(
						(charptr)fileName, 
						encodingName, 
						&(privateInfo->cidFontName),
						&(privateInfo->encodingName),
						&isCID, &isType0);
  if (privateInfo->type1FontName == NIL)
  {	
    PDEBUGINFO("Xatm: Alloc Error - Type 1 FontName.\n");
    (*font->unload_font)(font);
    return AllocError;
  }
  privateInfo->isCID = isCID;
  privateInfo->isType0 = isType0;
	
  /* Put the format and the mask in the right place.  We actually don't use
     these until building the characters.				   */
  font->format = format;

  /* Figure out the matrix for the request.				   */
  XatmCalculateMatrix(font, privateInfo);

  /* Put the Type 1 font name into the property list for this font, so that
     applications that know where to look can get it - this is necessary if
     they want to use the fonts for printing.  In order for the client to
     have access to the Type 1 font name as a string, it has to be made into
     an atom.								   */
  newAtom = MakeAtom((char *)privateInfo->type1FontName,
		     os_strlen((char *)privateInfo->type1FontName), 
		     true);
  XatmPutProperty(font, ADOBE_POSTSCRIPT_FONTNAME, newAtom);

  /* Set the bit/byte/glyph/scan information.				   */
  {
    int	bit, byte, scan, glyph;

    CheckFSFormat(format, fmask, &bit, &byte, &scan, &glyph, (int*) &value);
    font->bit = bit;
    font->byte = byte;
    font->scan = scan;
    font->glyph = glyph;
  }

  /* Set the FontPathElementPtr.					   */
  font->fpe = fpe;

  /* Get the encoding bounds set in the font, along with some other obscure
     constant information.						   */
  if (!XatmSetFontInfo(font, privateInfo))
  {
    /* Failure!  Somehow, we don't know about the right encoding anymorw, or
       somebody smashed our property tables in memory.  Not much to do but
       fail.								   */
    PDEBUGINFO("Xatm: Internal Error - Property Table Value.\n");
    (*font->unload_font)(font);
    return AllocError;
  }

  if (!XatmPostScriptFontInfo(font, privateInfo))
  {
    /* Failure!  Somehow, we don't know about the right encoding anymore, or
       somebody smashed our property tables in memory.  Not much to do but
       fail.								   */
    (*font->unload_font)(font);
    return AllocError;
  }

  /* Return the font.							   */
  *pFont = font;
  return Successful;
}

/*

ATMGetInfo

Very similar to ATMOpenScalable, in terms of how it gets it's job done.  It
just hands back different information to X.

*/

PUBLIC integer ATMGetInfo ARGDEF6(FontPathElementPtr, fpe, FontInfoPtr, 
				  pFontInfo, FontEntryPtr, entry, FontNamePtr,
				  fontName, char *, fileName, 
				  FontScalablePtr, vals)
  /*-
    FontPathElementPtr fpe		INPUT
    FontEntryPtr entry			INPUT
    FontNamePtr fontName		INPUT
    char *fileName			INPUT
    
    FontInfoPtr pFontInfo		OUTPUT
    FontScalablePtr vals		OUTPUT
  */
{
  FontPtr		font; /* 2ps_xdps:501 */
  XatmPrivateInfo	*privateInfo;
  IntX			retValue;
  charptr		encodingName;
  Card32		value;
  boolean		isCID, isType0;

  /* Check to make sure we are initialized.				   */
  if (!XatmMaybeInitialize()) return AllocError;
 /* 2ps_xdps:501 */
  font = (FontPtr)sys_malloc(sizeof(FontRec)); 
  if (font == NIL) 
  {
    PDEBUGINFO("Xatm: Alloc Error - FontRec.\n");
    return AllocError;
  }   

  /* Initialize key portions of the structure - first, the routines that will
     let us release the font easily, and second, the structures which need to
     be NILed out in order for the free procedure to work properly.	   */
  XatmLoadBitmapFuncs(font);
  font->refcnt = 0;
  font->maxPrivate = -1;
  font->devPrivates = NIL;
  font->fontPrivate = NIL;
  font->info.props = NIL;
  font->info.isStringProp = NIL;

  privateInfo = (PXatmPrivateInfo)sys_malloc(sizeof(XatmPrivateInfo));
  if (privateInfo == NIL)
  {
    PDEBUGINFO("Xatm: Alloc Error - Private Info.\n");
    (*font->unload_font)(font);
    return AllocError;
  }

  privateInfo->type1FontName = NIL;
  privateInfo->fileName = NIL;
   /* 2ps_xdps:501 */
  /* Set up the devPrivates private pointer array for the font by asigning
     our private field to the index we have alloated.			   */
  if (!_FontSetNewPrivate(font, XatmDevPrivatesIndex, privateInfo))
  {
    PDEBUGINFO("Xatm: Alloc Error - DevPrivate Assignment.\n");
    (*font->unload_font)(font);
    return AllocError;
  }
 /* 2ps_xdps:501 */

  /* Set the values, or the font name, whichever is needed.  Note this has to
     be done before the properties are retrieved from the font name, in case
     the font size is contained within vals already, and not within the font
     name.								   */
  FontParseXLFDName(fontName->name, vals, FONT_XLFD_REPLACE_VALUE);
    
  /* Call the property computing function.  This function tears apart the X
     font name from the request and inserts the values into the right places
     in the property tables that it allocates.  We can then call a companion
     function to get the values out of that table one at a time.	   */
  retValue = XatmComputeProps(font, (char *)fontName->name);
  if (retValue != Successful) 
  {
    PDEBUGINFO("ATM: Alloc error - Properties.\n");
    return retValue;
  }

  /* Any failure after this point must remember to deallocate the property
     arrays.								   */

  /* If the encoding string for the font is NIL, this means we have a
     composite font, so we need to send in the encoding name into the
     selectfont routine.  If it isn't NIL, then we have a roman font, and for
     that we do the encoding ourselves, manually.  The exception is the
     default encoding, for which the encoding strings return value is NIL,
     but so it the encoding name - so it works out correctly, just following
     an different path than other roman fonts.				   */
  (void)XatmGetProperty(font, "CHARSET_ENCODING", &value);
  if (value != (Card32)0 && XatmGetEncodingStrings(value) == NIL)
    encodingName = XatmGetEncodingName(value);
  else
    encodingName = NIL;

  /* Now, find out what the Type 1 font name is for this font.  This requires
     parsing a font file, searching for a keyword.  This utility function
     allocates memory on our behalf, so we must remember to free it when
     necessary.								   */
  privateInfo->type1FontName = (charptr)XatmGetFontNameAndType(
						(charptr)fileName,
						encodingName, 
						&(privateInfo->cidFontName),
						&(privateInfo->encodingName),
						&isCID, &isType0 );
  if (privateInfo->type1FontName == NIL)
  {	
    PDEBUGINFO("Xatm: Alloc Error - Type 1 FontName.\n");
    (void)sys_free((char *)font->info.props);
    (void)sys_free((char *)font->info.isStringProp);
    return AllocError;
  }
  privateInfo->isCID = isCID;
  privateInfo->isType0 = isType0;

  /* Allocate and copy the filename for this font.			   */
  privateInfo->fileName = (charptr)sys_malloc(os_strlen(fileName) + 1);
  if (!privateInfo->fileName)
  {
    PDEBUGINFO("Xatm: Alloc Error - FileName.\n");
    (void)sys_free((char *)privateInfo->type1FontName);
    (void)sys_free((char *)font->info.props);
    (void)sys_free((char *)font->info.isStringProp);
    return AllocError;
  }
  os_strcpy((char *)privateInfo->fileName, fileName);

  /* Calculate the matrix for the request.				   */
  XatmCalculateMatrix(font, privateInfo);

  /* Get the encoding bounds set in the font, along with some other obscure
     constant information.						   */
  if (!XatmSetFontInfo(font, privateInfo))
  {
    PDEBUGINFO("Xatm: Internal Error - Property Table Value.\n");
    (void)sys_free((char *)privateInfo->fileName);
    (void)sys_free((char *)privateInfo->type1FontName);
    (void)sys_free((char *)font->info.props);
    (void)sys_free((char *)font->info.isStringProp);
    return AllocError;
  }

  /* Get the information that can only be obtained by loading the font and
     asking it.								   */
  if (!XatmPostScriptFontInfo(font, privateInfo))
  {
    (void)sys_free((char *)privateInfo->fileName);
    (void)sys_free((char *)privateInfo->type1FontName);
    (void)sys_free((char *)font->info.props);
    (void)sys_free((char *)font->info.isStringProp);
    return AllocError;
  }

  /* Assign the  Info Rec, and the pointers  to the  property lists from the
     transient font record.						   */
  *pFontInfo = font->info;
  pFontInfo->nprops = font->info.nprops;
  pFontInfo->props = font->info.props;
  pFontInfo->isStringProp = font->info.isStringProp;

  /* Free the allocated pieces of the font recors that are not now owned by
     the pFontInfo structure.  Basically, that's the memory we allocated for
     our private font record.						   */
  (void)sys_free((char *)privateInfo->fileName);
  (void)sys_free((char *)privateInfo->type1FontName);
  (void)sys_free((char *)privateInfo);
  (void)sys_free((char *)font);

  /* The transient records - the private font information record and the font
     rec - now do not own any memory, so they may float off into the
     ether...								   */
  return Successful;
}


/*

XatmCalculateMatrix

Calculates the font requests' matrix form the information contained in the
font.  This may place some properties back into the font if necessary.

*/
PRIVATE procedure XatmCalculateMatrix ARGDEF2(FontPtr, font,
					      PXatmPrivateInfo, privateInfo)
{
  Card32		value;
  int			numRes, xRes, yRes, success, len, count;
  double		pntSize;
  fsResolution		*resolutions;
  char			buffer[MAXFONTNAMELEN];

  /* Figuring out the matrix of the request.  This doesn't take much
     information.  We need to know the x and y resolution of the destination
     device, and the matrix of the request.  If there is no matrix in the
     request, look for a point size.  Construct the matrix from that point
     size if necessary.  If only on of the matrices is present, construct a
     string for the other.  Pixel size requests can only be approximated.  We
     start out with default values in all of the necessary variables, and
     then place in replacement values as we find them.			   */
  xRes = DEFAULT_RESOLUTION;
  yRes = DEFAULT_RESOLUTION;
  pntSize = (double)DEFAULT_POINTSIZE;

  /* Next, try and get the client resolutions.  This is done second, so that
     when the properties are snagged out of the font, third, they take
     precidence (if present).						   */
  resolutions = (fsResolution *)GetClientResolutions(&numRes);
  if (numRes != 0) 
  {
    xRes = resolutions->x_resolution;
    yRes = resolutions->y_resolution;
    pntSize = (double)(resolutions->point_size) / 10; /* 2ps_xdps.466 */
  }

  /* Try to snag the values out of the font property table.		   */
  (void)XatmGetProperty(font, "RESOLUTION_X", &value);
  if (value != 0) xRes = value;
  (void)XatmGetProperty(font, "RESOLUTION_Y", &value);
  if (value != 0) yRes = value;

  /* First look for a pixel size request.  This is done first so that a point
     size request will take precedence.					   */
  (void)XatmGetProperty(font, "PIXEL_SIZE", &value);
  if (value != 0) 
  {
    pntSize = (double)value;
    pntSize *= POINTS_PER_INCH;
    pntSize /= yRes;
  }    

  /* Next look for a point size.  This is done before the matrix checking, so
     that those can take precedence.					   */
  (void)XatmGetProperty(font, "POINT_SIZE", &value);
  if (value != 0) pntSize = value / 10.0;

  /* Look for the matrices.  If anything goes wring in here, wipe out the
     propeerty and revert back to the point size requests, even if that ends
     up being the default.						   */
  success = false;
  (void)XatmGetProperty(font, "POINT_SIZE_MATRIX", &value);
  if (value != 0)
  {
    /* The POINT_SIZE_MATRIX takes precedence.  It consists of four floating
       point numbers, with '~' substituted as the minus sign.  So, parse it
       piece by piece.							   */
    char	*matrix = NameForAtom(value);
    int		len;

    /* Check for some basic validity of the string.			   */
    if ((matrix == NULL) || (matrix[0] != '[')) goto BadPointSizeMatrix;

    /* Copy the matrix to our private buffer, chack the last character to
       make sure it closes the matrix, and then wipe it out.		   */
    len = os_strlen(matrix) - 1;
    os_strcpy(buffer, &matrix[1]);
    if (buffer[len-1] != ']') goto BadPointSizeMatrix;/* 2ps_xdps.466 */
    buffer[len-1] = 0;

    /* Change squigles into minus signs so that we can parse the matrix.   */
    for (count = 0 ; count < len ; count++)
      if (buffer[count] == '~') buffer[count] = '-';

    /* Parse the point size matrix.					   */
    if (sscanf(buffer, "%f %f %f %f", &privateInfo->pointSizeMtx.a,
	       &privateInfo->pointSizeMtx.b, &privateInfo->pointSizeMtx.c,
	       &privateInfo->pointSizeMtx.d) != 4)
      goto BadPointSizeMatrix;

    /* Convert the matrix into pixel size.				   */
    privateInfo->buildMatrix.a = privateInfo->pointSizeMtx.a * xRes / 
				 POINTS_PER_INCH;
    privateInfo->buildMatrix.b = privateInfo->pointSizeMtx.b * xRes / 
				 POINTS_PER_INCH;
    privateInfo->buildMatrix.c = privateInfo->pointSizeMtx.c * yRes / 
				 POINTS_PER_INCH;
    privateInfo->buildMatrix.d = privateInfo->pointSizeMtx.d * yRes / 
				 POINTS_PER_INCH;
    
    privateInfo->buildMatrix.tx = privateInfo->buildMatrix.ty =
    privateInfo->pointSizeMtx.tx = privateInfo->pointSizeMtx.ty = 0.0;
    /* We succeded in getting the matrix.				   */
    success = true;
  }

 BadPointSizeMatrix:
  /* Check for a pixel size matrix.					   */
  (void)XatmGetProperty(font, "PIXEL_SIZE_MATRIX", &value);
  if (!success && value != 0)
  {
    /* Only a pixel size matrix was present.  We'll have to construct a
       point size matrix.						   */
    /* The POINT_SIZE_MATRIX takes precedence.  It consists of four floating
       point numbers, with '~' substituted as the minus sign.  So, parse it
       piece by piece.							   */
    char	*matrix = NameForAtom(value);
    int		len;

    /* Check for some basic validity of the string.			   */
    if ((matrix == NULL) || (matrix[0] != '[')) goto BadPixelSizeMatrix; 
    /* 2ps_xdps.459 */

    /* Copy the matrix to our private buffer, chack the last character to
       make sure it closes the matrix, and then wipe it out.		   */
    len = os_strlen(matrix) - 1;
    os_strcpy(buffer, &matrix[1]);
    if (buffer[len-1] != ']') goto BadPixelSizeMatrix; /* 2ps_xdps.459 & 466 */
    buffer[len-1] = 0;

    /* Change squigles into minus signs so that we can parse the matrix.   */
    for (count = 0 ; count < len ; count++)
      if (buffer[count] == '~') buffer[count] = '-';

    /* Parse the pixel size matrix.					   */
    if (sscanf(buffer, "%f %f %f %f", &privateInfo->buildMatrix.a,
	       &privateInfo->buildMatrix.b, &privateInfo->buildMatrix.c,
	       &privateInfo->buildMatrix.d) != 4)
      goto BadPixelSizeMatrix; /* 2ps_xdps.459 */

    /* Convert the matrix into point size.				   */
    privateInfo->pointSizeMtx.a = privateInfo->buildMatrix.a / xRes * 
				 POINTS_PER_INCH;
    privateInfo->pointSizeMtx.b = privateInfo->buildMatrix.b / xRes * 
				 POINTS_PER_INCH;
    privateInfo->pointSizeMtx.c = privateInfo->buildMatrix.c / yRes * 
				 POINTS_PER_INCH;
    privateInfo->pointSizeMtx.d = privateInfo->buildMatrix.d / yRes * 
				 POINTS_PER_INCH;
    privateInfo->buildMatrix.tx = privateInfo->buildMatrix.ty =
    privateInfo->pointSizeMtx.tx = privateInfo->pointSizeMtx.ty = 0.0;
    /* 2ps_xdps.466 */
    
    /* We succeded in getting the matrix.				   */
    success = true;
  }

 BadPixelSizeMatrix:
  if (!success)
  {
    /* Neither kind of matrix was present.  Go ahead and construct one from
       the point size information.					   */
    double	pixelSize;

    /* Store away the information without xRes and yRes factored in, to make
       it easy to construct the point size matrix information.		   */
    privateInfo->pointSizeMtx.a = privateInfo->pointSizeMtx.d = pntSize;
    privateInfo->pointSizeMtx.b = privateInfo->pointSizeMtx.c = 
      privateInfo->pointSizeMtx.tx = privateInfo->pointSizeMtx.ty = 0.0;

    pixelSize = pntSize;
    pixelSize *= xRes;
    pixelSize /= POINTS_PER_INCH;
    privateInfo->buildMatrix.a = pixelSize;

    pixelSize = pntSize;
    pixelSize *= yRes;
    pixelSize /= POINTS_PER_INCH;
    privateInfo->buildMatrix.d = pixelSize;
      
    privateInfo->buildMatrix.b = privateInfo->buildMatrix.c = 
      privateInfo->buildMatrix.tx = privateInfo->buildMatrix.ty = 0.0;
  }

  /* Now, construct a canonical pixel and point size matrix specification,
     and put them back into the property list.  At the same time, construct a
     canonical font name with the matrix fields filled in.		   */
  os_sprintf(buffer, "[%.2f %.2f %.2f %.2f]", privateInfo->buildMatrix.a,
	  privateInfo->buildMatrix.b, privateInfo->buildMatrix.c,
	  privateInfo->buildMatrix.d); /* 2ps_xdps.457 */
  len = os_strlen(buffer);
  for (count = 0 ; count < len ; count++)
    if (buffer[count] == '-') buffer[count] = '~';
  value = MakeAtom(buffer, os_strlen(buffer), true);
  XatmPutProperty(font, "PIXEL_SIZE_MATRIX", value);

  os_sprintf(buffer, "[%.2f %.2f %.2f %.2f]", privateInfo->pointSizeMtx.a,
	  privateInfo->pointSizeMtx.b, privateInfo->pointSizeMtx.c,
	  privateInfo->pointSizeMtx.d); /* 2ps_xdps.457 */
  len = os_strlen(buffer);
  for (count = 0 ; count < len ; count++)
    if (buffer[count] == '-') buffer[count] = '~';
  value = MakeAtom(buffer, os_strlen(buffer), true);
  XatmPutProperty(font, "POINT_SIZE_MATRIX", value);

  /* Now, construct the font name.					   */
  os_strcpy(buffer, "-");
  XatmGetProperty(font, "FOUNDRY", &value);
  if (value != 0) os_strcat(buffer, NameForAtom(value));
  os_strcat(buffer, "-");
  XatmGetProperty(font, "FAMILY_NAME", &value);
  if (value != 0) os_strcat(buffer, NameForAtom(value));
  os_strcat(buffer, "-");
  XatmGetProperty(font, "WEIGHT_NAME", &value);
  if (value != 0) os_strcat(buffer, NameForAtom(value));
  os_strcat(buffer, "-");
  XatmGetProperty(font, "SLANT", &value);
  if (value != 0) os_strcat(buffer, NameForAtom(value));
  os_strcat(buffer, "-");
  XatmGetProperty(font, "SETWIDTH_NAME", &value);
  if (value != 0) os_strcat(buffer, NameForAtom(value));
  os_strcat(buffer, "-");
  XatmGetProperty(font, "ADD_STYLE_NAME", &value);
  if (value != 0) os_strcat(buffer, NameForAtom(value));
  os_strcat(buffer, "-");
  XatmGetProperty(font, "PIXEL_SIZE_MATRIX", &value);
  if (value != 0) os_strcat(buffer, NameForAtom(value));
  os_strcat(buffer, "-");
  XatmGetProperty(font, "POINT_SIZE_MATRIX", &value);
  if (value != 0) os_strcat(buffer, NameForAtom(value));
  os_strcat(buffer, "-");
  /* The X and Y resolutions, and the average width properties are integers,
     and have to be handled special.					   */
  {
    char	resBuf[10];

    os_sprintf(resBuf, "%-d", xRes); /* 2ps_xdps.457 */
    os_strcat(buffer, resBuf);
    os_strcat(buffer, "-");
    os_sprintf(resBuf, "%-d", yRes); /* 2ps_xdps.457 */
    os_strcat(buffer, resBuf);
    os_strcat(buffer, "-");
    XatmGetProperty(font, "SPACING", &value);
    if (value != 0) os_strcat(buffer, NameForAtom(value));
    os_strcat(buffer, "-");
    XatmGetProperty(font, "AVERAGE_WIDTH", &value);
    if (value != 0) os_sprintf(resBuf, "%-d", value); /* 2ps_xdps.457 */
    os_strcat(buffer, resBuf);
  }
  /* 2ps_xdps.466
  os_strcat(buffer, "-");
  XatmGetProperty(font, "CHARSET_REGISTRY", &value);
  if (value != 0) os_strcat(buffer, NameForAtom(value));
  */
  os_strcat(buffer, "-");
  XatmGetProperty(font, "CHARSET_ENCODING", &value);
  if (value != 0) os_strcat(buffer, NameForAtom(value));
  /* os_strcat(buffer, "-"); 2ps_xdps.466 */

  value = MakeAtom(buffer, os_strlen(buffer), true);
  XatmPutProperty(font, "FONT", value);

  /* Figure out if the font is anamorphically scaled.			   */
  if ((privateInfo->buildMatrix.a == privateInfo->buildMatrix.d) &&
      (privateInfo->buildMatrix.b == 0) &&
      (privateInfo->buildMatrix.c == 0))
    font->info.anamorphic = false;
  else
    font->info.anamorphic = true;
}


/*

XatmSetFontInfo

Gets the encoding bounds for the font, and then sets some other contant
information.

*/
PRIVATE boolean XatmSetFontInfo ARGDEF2(FontPtr, font,
					PXatmPrivateInfo, privateInfo)
{
  Int16		firstCol, lastCol, firstRow, lastRow;
  Card32	value;

  /* Obtain the encoding boundaries for the encoding for this font.  For a
     roman font, this should consist of one entire row.  For a compsite font,
     it should consists of more rows, and possibly (though not likely) fewer
     columns.								   */
  (void)XatmGetProperty(font, "CHARSET_ENCODING", &value);
  if (!XatmGetEncodingInfo(value, &firstCol, &lastCol, &firstRow, &lastRow))
    return false;

  font->info.firstCol = firstCol;
  font->info.lastCol = lastCol;
  font->info.firstRow = firstRow;
  font->info.lastRow = lastRow;
  font->info.defaultCh = 32;		/* Space character.		   */

  /* We must assume this is false - there is no way to tell without building
     every last character in the font.					   */
  font->info.noOverlap = false;

  /* Even for a fixed pitch font, this must be false because of changing
     ascenders and descenders.						   */
  font->info.constantMetrics = false;

  /* Not all of the characters in the font exist.  Although we can certainly
     produce bitmaps for them (through the .notdef mechanism).		   */
  font->info.allExist = false;

  /* Yes, X can try to cache this font.  Is there ever a case where this
     would be false?  Maybe if you were randomly tweaking the characters ever
     time they were shown or something.					   */
  font->info.cachable = true;

  /* All ink inside character cell.  What good would a character cell be
     otherwise?								   */
  font->info.inkInside = true;	

  /* We don't provide inkMetrics, since they are the same as the character
     metrics - they would just waste space.				   */
  font->info.inkMetrics = false;

  /* Which way did he go?						   */
  font->info.drawDirection = LeftToRight;

  return true;
}


/*

XatmPostScriptFontInfo

This function asks the interpreter to select this font, and then gets the
required information from the interpreter.  It then shoves the remaining
information into the font.
*/
PRIVATE boolean XatmPostScriptFontInfo ARGDEF2(FontPtr, font,
					       PXatmPrivateInfo, privateInfo)
{
  XatmBBox	bounds;
  boolean	fixedPitch;
  charptr	notice = NIL, copyright = NIL;
  Int32		underlinePosition, underlineThickness, newAtom, 
		italicAngle;
  xCharInfo     maxBounds;
  xCharInfo     minBounds;

  /* First off, select the 1000 pixel font and get the information for
     *it*.								   */
  /* 2ps_xdps.459 */
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
  /* 2ps_xdps.459: XatmSelectFont was previously done in XatmGetGlyphs which
     is called many times - was inefficient. entering and leaving 
     interpreter is expensive */
  {
    PDEBUGINFO("Xatm: Selecting font for info call.\n");
    return false;
  }
  if( lastSelectedFont != NULL ){
	sys_free( lastSelectedFont );
	lastSelectedFont = NULL;
  }
  lastSelectedFont = ( char * ) sys_malloc( 
			     os_strlen( (char *)privateInfo->type1FontName )
			     +1
  					 ); /* 2ps_xdps.503 */
  if( lastSelectedFont == NULL ){
       PDEBUGINFO("Xatm: Selecting font for build.");
       return AllocError;
  }
  /*2ps_xdps.502 */
  os_strcpy( lastSelectedFont, (char *)privateInfo->type1FontName ); 
  lastSelectedMtx.a = (privateInfo->buildMatrix).a;
  lastSelectedMtx.b = (privateInfo->buildMatrix).b;
  lastSelectedMtx.c = (privateInfo->buildMatrix).c;
  lastSelectedMtx.d = (privateInfo->buildMatrix).d;
  lastSelectedMtx.tx = (privateInfo->buildMatrix).tx;
  lastSelectedMtx.ty = (privateInfo->buildMatrix).ty;

  if( !XatmGetFontInfo( privateInfo, &bounds,
			&fixedPitch, &italicAngle,
		        &underlinePosition, &underlineThickness, 
		        &notice, 
		        privateInfo->isCID,
			privateInfo->isType0) )
  {
    PDEBUGINFO("Xatm: Error Obtaining font info.\n");
    fixedPitch = 0;
    italicAngle = 0;
    underlinePosition = 0;
    underlineThickness = 0;
    notice = (charptr) "";
    bounds.llx = bounds.lly = bounds.urx = bounds.ury = 0;
   }

  /* We know have all the information needed to complete filling ot the font
     structure.								   */
  font->info.constantWidth = fixedPitch;

  /* 2ps_xdps.459: Since the font dictionary can provide only the
   fontbbox, we have to somehow estimate the the maxbounds and
   minbounds and minbounds from that. This is where we do it.
   Essentially some arbitrary padding ups and downs are done. */
	   
  /* Never claim to be a terminal font - we *never* know if all our cells
     will be exactly the same height and width.				   */
  font->info.terminalFont = false;

  font->info.fontAscent = bounds.ury + 1;
  font->info.fontDescent = -bounds.lly + 1;
  font->info.maxbounds.leftSideBearing = bounds.urx + 2;
  font->info.maxbounds.rightSideBearing = bounds.urx + 2;
  font->info.maxbounds.ascent	= bounds.ury + 2;
  font->info.maxbounds.descent = -bounds.lly + 2;
  font->info.maxbounds.characterWidth = bounds.urx - bounds.llx;
  font->info.maxbounds.attributes = 0;

  font->info.minbounds.leftSideBearing = bounds.llx - 2;
  font->info.minbounds.rightSideBearing = bounds.llx - 2;
  font->info.minbounds.ascent	= -font->info.maxbounds.descent;
  font->info.minbounds.descent = -font->info.maxbounds.ascent;
  font->info.minbounds.characterWidth = 0;
  font->info.minbounds.attributes = 0;

  /* setting font->info.ink_minbounds = font->info.ink_maxbounds prevents X 
     from calling get_metrics for each and every character in the font. */

  font->info.ink_maxbounds = font->info.maxbounds;
  if( privateInfo->isCID || privateInfo->isType0 )
	font->info.ink_minbounds = font->info.ink_maxbounds;
  else{/* font->info.minbounds/maxbounds etc have already been filled
	  with *estimates* based on the font bounding box. That is the best
	  we can do for non Type1 fonts (since the large number of characters
	  there make it unrealistic to go over all characters to obtain
	  true max/min bounds). However, since we have only 256 characters
	  in Type1 and we will cache *all* of them during font opening 
	  anyway, we are calling get_glyphs() on the entire range here
	  to obtain global mtric info. Incidentally, this would cache
	  all characters, so future renderings will be fast. */
	Card8     chars[512];
	Card32    metricCount;
	xCharInfo *metricBase[256];
	int       r, c, i = 0, retval;

	/* we are basically simulating what the xserver function
	   QueryFont() in dixfonts.c will do */
	r = font->info.firstRow; /* for Type1 font->info.firstRow == 
				    font->info.lastRow */
	for (c = font->info.firstCol; c <= (int)font->info.lastCol; c++){
		chars[i++] = r;
		chars[i++] = c;
	}
	DebugAssert( (font->info.lastCol-font->info.firstCol+1) == 256 );
	retval = XatmGetGlyphs( font, font->info.lastCol-font->info.firstCol+1, 
				chars, TwoD16Bit, &metricCount,
				(CharInfoPtr *)metricBase );
	if( retval == Successful ){
  	  maxBounds.leftSideBearing = maxBounds.rightSideBearing =
	  maxBounds.characterWidth = maxBounds.ascent = maxBounds.descent
	  = -10000; maxBounds.attributes = 0;
  	  minBounds.leftSideBearing = minBounds.rightSideBearing =
	  minBounds.characterWidth = minBounds.ascent = minBounds.descent
	  = 10000; minBounds.attributes = 0;
	  for( i = 0; i < metricCount; i++ ){
	    if( metricBase[i]->leftSideBearing > maxBounds.leftSideBearing ) 
	      maxBounds.leftSideBearing = metricBase[i]->leftSideBearing;
	    if( metricBase[i]->rightSideBearing > maxBounds.rightSideBearing ) 
	      maxBounds.rightSideBearing = metricBase[i]->rightSideBearing;
	    if( metricBase[i]->characterWidth > maxBounds.characterWidth ) 
	      maxBounds.characterWidth = metricBase[i]->characterWidth;
	    if( metricBase[i]->ascent > maxBounds.ascent ) 
	      maxBounds.ascent = metricBase[i]->ascent;
	    if( metricBase[i]->descent > maxBounds.descent ) 
	      maxBounds.descent = metricBase[i]->descent;

	    if( metricBase[i]->leftSideBearing < minBounds.leftSideBearing ) 
	      minBounds.leftSideBearing = metricBase[i]->leftSideBearing;
	    if( metricBase[i]->rightSideBearing < minBounds.rightSideBearing ) 
	      minBounds.rightSideBearing = metricBase[i]->rightSideBearing;
	    if( metricBase[i]->characterWidth < minBounds.characterWidth ) 
	      minBounds.characterWidth = metricBase[i]->characterWidth;
	    if( metricBase[i]->ascent < minBounds.ascent ) 
	      minBounds.ascent = metricBase[i]->ascent;
	    if( metricBase[i]->descent < minBounds.descent ) 
	      minBounds.descent = metricBase[i]->descent;
	  }

  	  font->info.ink_maxbounds = font->info.maxbounds = maxBounds;
	  font->info.ink_minbounds = font->info.minbounds = minBounds;
	}
  }

  font->info.maxOverlap = (font->info.maxbounds.rightSideBearing - 
			   font->info.minbounds.leftSideBearing);

  /* Shove in some values into the property section of the font.	   */
  XatmPutProperty(font, "ITALIC_ANGLE", italicAngle);
  XatmPutProperty(font, "UNDERLINE_POSITION", underlinePosition);
  XatmPutProperty(font, "UNDERLINE_THICKNESS", underlineThickness);

  /* The Notice and Copyright strings must be turned into atoms before
     installing them as properties.					   */
  if (notice)
  {
    newAtom = MakeAtom((char *)notice, os_strlen( (char*)notice), true);
    XatmPutProperty(font, "NOTICE", newAtom);
  }

  if (copyright)
  {
    newAtom = MakeAtom((char *)copyright, os_strlen( (char*)notice), true);
    XatmPutProperty(font, "COPYRIGHT", newAtom);
  }

  /* Random other properties.						   */
  XatmPutProperty(font, "DESTINATION", 0);	/* WYSIWYG		   */

  return true;
}


/*

XatmGetFontNameAndType

This function opens the Type 1 font program file handed in to us (if
possible) and searches for the keyword /FontName.  Once this is found, the
subsequent token (if there) is a PostScript string (e.g., "(Times-Roman}")
containing the Type 1 font name which we must allocate space for and hand
back.

There is a minumum size for the buffer - 32 characters.  Enough to hold ne
font name.

*/
#define LOOKINGFORKEY 0
#define KEYFOUND 1
#define VALUEFOUND 2
#define VALUESTART 3

PUBLIC charptr XatmGetFontNameAndType ARGDEF6(charptr, filename,
					    charptr, encodingName, 
					    charptr *, cidFontName,
					    charptr *, privencodingName,
					    boolean *, isCID,
					    boolean *, isType0)
{
  Stm		fontProgram;
  register char *searchptr, *chkptr, *chkptr1, 
		*key = "/FontName", *key1 = "/FontType";
  char		buffer[BUFSIZE], *nameStart, 
		nameBuffer[256], *nameBuf = nameBuffer;
  register int	readlen, searchlen = os_strlen(key), 
		chklen, chklen1, 
		foundname = LOOKINGFORKEY, foundtype = LOOKINGFORKEY,
		fontType = -1;

  /* First, check if the filename isn't actually the name of the file.  This
     is a special escape for those files which are still fonts, but do not
     contain the /FontName () def syntax in them.  The ".t1n" or ".T1N"
     extension indicates this, and we simply return the tail of the filename,
     sans extension, as the font name. Also check for ".cid" or ".CID for
	 the cid case.				   
*/
  chklen = os_strlen((char *)filename);
  *isCID = !os_strcmp((char *)&filename[chklen-4], ".cid") ||
	 	!os_strcmp((char *)&filename[chklen-4], ".CID");
  if( *isCID ){
    /* Find the tail of the file name.	*/
    nameStart = (char *)&filename[0];
    while ((chkptr = os_index(nameStart, '/')) != NIL) /* 2ps_xdps.459. Bugfix
							  os_index( searchptr,.
							  was called erroneously
							  */
      nameStart = chkptr+1;
    
    searchlen = os_strlen(nameStart) - 4;
    chklen = os_strlen( (char*)encodingName) + 1;
    searchptr=(char *)sys_malloc(searchlen + chklen + 2);
    if (searchptr == NIL) return NIL;
    *cidFontName = (charptr) sys_malloc(searchlen + 1);
    if (*cidFontName == NIL) return NIL;
    *privencodingName = (charptr)sys_malloc(searchlen + 1);
    if (*privencodingName == NIL) return NIL;
    os_bcopy(nameStart, searchptr, searchlen);
    searchptr[searchlen] = '\0';
    os_strcpy( (char *)*cidFontName, searchptr );
    os_strcat(searchptr, "-");
    os_strcat(searchptr, (char *)encodingName);
    os_strcpy( (char *)*privencodingName, (char *)encodingName );
    return (charptr)searchptr;
  }

  /* 2ps_xdps.473 */
  /* Initialize a couple of variables, in such a way as to avoid a special
     first-time-through case.						   */
  chkptr = key; /* "/FontName" */
  chklen = 0;
  chkptr1 = key1; /* "/FontType" */
  chklen1 = 0;

  /* Try and open the font file.					   */
  fontProgram = os_fopen((char *)filename, "r");
  if (fontProgram == NULL) return (charptr)NULL;

  /* Now that we have the font file open, begin the search for font name and 
     type*/
  while ((readlen = fread(searchptr = buffer, 1, BUFSIZE, fontProgram)) > 0)
  {
    while (readlen > 0)
    {
      DebugAssert(!( ( foundname == KEYFOUND ) && ( foundtype == KEYFOUND ) ));

      if( foundname == KEYFOUND ){
	   if( *searchptr == '/' ){
		foundname = VALUESTART;
		nameBuf = nameBuffer;
	   }
      }
      else if(foundname == VALUESTART){
	  if (*searchptr != '\0' && *searchptr != '\t' && 
	      *searchptr != '\n' && *searchptr != ' ' &&
	      *searchptr != '\f' && *searchptr != '\r' && 
	      *searchptr != '(' && *searchptr != ')' && 
	      *searchptr != '<' && *searchptr != '>' && 
	      *searchptr != '[' && *searchptr != ']' &&
	      *searchptr != '{' && *searchptr != '}' && 
	      *searchptr != '/' && *searchptr != '%')
	  {
		*nameBuf = *searchptr;
		nameBuf++;
		DebugAssert( (nameBuf - nameBuffer) < 256 );
	  }
	  else{
		foundname = VALUEFOUND;
		*nameBuf = '\0';
	  }
      }
      else if(foundname == LOOKINGFORKEY){
           if( *searchptr == *chkptr){
		chkptr++;
		if( (*chkptr) == '\0' ){
			foundname = KEYFOUND;
		}
      	   }
           else{
		chkptr = key;
           }
      }
      else{
	DebugAssert( foundname == VALUEFOUND );
      }

      if( foundtype == KEYFOUND ){
	   if( isdigit( (int) *searchptr ) ){
		searchptr[1] = '\0';
		fontType = atoi( searchptr );
		*isType0 = ( fontType == 0 );
		foundtype = VALUEFOUND;
	   }
      }
      else if(foundtype == LOOKINGFORKEY){
           if(*searchptr == *chkptr1)
           {
 		chkptr1++;
 	    if( (*chkptr1) == '\0' ){
 		foundtype = KEYFOUND;
 	    }
           }
           else{
 		chkptr1 = key1;
           }
      }
      searchptr++; readlen--; 
      if( ( foundname == VALUEFOUND ) && ( foundtype == VALUEFOUND ) )
	   break;
    }
  }

  fclose(fontProgram);

  /* Allocate the memory for the font name, and copy the font name into the
     allcated memory.							   */
  nameBuf = (char *)sys_malloc( os_strlen( nameBuffer ) + 1 );
  if (nameBuf == NULL) return NULL;
  os_strcpy( nameBuf, nameBuffer );
  return (charptr)nameBuf;

}
