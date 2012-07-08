/* @(#)CM_VerSion actual.c 3010 1.8 20436.eco sum= 15712 3010.103 */
/* @(#)CM_VerSion actual.c 2017 1.6 17841.eco sum= 06915 2017.013 */
/* @(#)CM_VerSion actual.c 2016 1.2 13614.eco sum= 42516 */
/* @(#)CM_VerSion actual.c 2015 1.9 12669.eco sum= 63446 */
/* @(#)CM_VerSion actual.c 2014 1.4 08790.eco sum= 20540 */
/* @(#)CM_VerSion actual.c 2013 1.4 08389.eco sum= 22725 */
/*
  actual.c

(c) Copyright 1993 Adobe Systems Incorporated.
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
*******************************************************************************
*                                                                             *
*  File: actual.c                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
*  The PUBLIC procedures in this file are declared extern in actual.h         *
*                                                                             *
*  This file contains the implementation for SetXRGBActual.                   *
*                                                                             *
*  - We only implement the actual mechanism for color devices. We could       *
*    in theory also do it for gray devices, but it doesn't seem that          *
*    useful in that case. The code could easily be added following the        *
*    model presented for the color case. Actual doesn't make any sense        *
*    for mono devices.                                                        *
*                                                                             *
*  - This implementation asserts bitdepth == 8 && rgb model. It would be      *
*    relatively straightforward to build a general version or several         *
*    special case versions for different bit depths.                          *
*                                                                             *
*  - This implementation asserts SCANUNIT == 32. So does the rest of the      *
*    X/DPS system.                                                            *
*                                                                             *
*******************************************************************************
*                                                                             *
*  PRIVATE PROCEDURES:                                                        *
*                                                                             *
*    SolidColor                                                               *
*    AddToHash                                                                *
*    FreeActualPattern                                                        *
*    ActualColorSetupPattern                                                  *
*    ActualPatternInfo                                                        *
*    ActualDestroyPattern                                                     *
*    ActualPattern                                                            *
*                                                                             *
*  PUBLIC PROCEDURES:                                                         *
*                                                                             *
*    XDPS_UpdateActualPattern                                                 *
*    XDPS_SetXRGBActual                                                       *
*                                                                             *
*  PUBLIC VARIABLES:                                                          *
*                                                                             *
*    <list>                                                                   *
*                                                                             *
*******************************************************************************
*/

/*-----------------------------------------*/
/* Public interfaces from PostScript world */
/*-----------------------------------------*/

#include PACKAGE_SPECS
#include ENVIRONMENT
#include PUBLICTYPES
#include XDPS_SUPPORT
#include DEVPATTERN
#include DPSCUSTOMOPS
#include CANTHAPPEN

#include "devcommon.h"  /* USING [defaultHalftone] */
#include "winclip.h"    /* USING [WinInfo]         */
/* XYZ: Backdoor into device package, not clean */

#define XWININFO_FROM_DEVICE(device)    \
        ( (XWinInfo*) ( (WinInfo*)(device)->priv.ptr )->window )

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "actual.h"
#include "genxsupport.h"
#include "xdevice.h"
#include "xwininfo.h"

/*-----------*/
/* constants */
/*-----------*/

#define	TableSize			256
#define	Hash(r, g, b)			((integer)((r) + (g) + (b)) % TableSize)
#define	PixelValue(entry)		(((entry) & 0xff000000) >> 24)
#define	ColorValue(entry)		((entry) & 0x00ffffff)
#define	RGBMatches(target, entry)	((target) == ColorValue(entry))
#define	Matches(target, entry)		((target) == (entry))

/*-------------------------------------*/
#define	ComposeEntry(r, g, b, p)        \
        (                               \
           (((Card32)(p)) << 24)        \
         + (((Card32)(r)) << 16)        \
         + (((Card32)(g)) << 8 )        \
         + (((Card32)(b))      )        \
        )

#if (SCANUNIT ==64)

#define	FillInData(val, data)			\
	{					\
	register SCANTYPE p = PixelValue(val);	\
	data->start = &(data->value);		\
	data->end = data->start+1;		\
	data->constant = true;			\
	data->width = 1;			\
	data->id = 0;				\
	p |= p << 8;				\
	p |= p << 16;				\
	data->value = p | (p << 32);		\
	}

#else /* (SCANUNIT ==64) */

#define	FillInData(val, data)			\
	{					\
	register SCANTYPE p = PixelValue(val);	\
	data->start = &(data->value);		\
	data->end = data->start+1;		\
	data->constant = true;			\
	data->width = 1;			\
	data->id = 0;				\
	p |= p << 8;				\
	data->value = p | (p << 16);		\
	}

#endif /* (SCANUNIT ==64) */

/*----------*/
/* typedefs */
/*----------*/

typedef struct
{
  PatternProcsRec  pat;
  PatternHandle    origPat;
  XWinInfo        *xWinInfo;
} ActualPatternObj;

/*--------------------*/
/* PRIVATE procedures */
/*--------------------*/

/*****************************************************************************/
/*                                                                           */
/*  SolidColor                                                               */
/*                                                                           */
/*****************************************************************************/

PRIVATE boolean SolidColor ARGDEF4(PDevice, device, Card8, r, Card8, g, Card8, b)
{
  DevMarkInfo    info;
  PatternData    patData;
  char          *arrayView;
  DevColorSpace  dcs;

  os_bzero (&dcs, sizeof(dcs));

  dcs.priv              = NIL;
  dcs.colorSpace        = DEVRGB_COLOR_SPACE;
/*dcs.cie               = not applicable here */
/*dcs.devNInfo          = not applicable here */
/*dcs.createDevNColor   = not applicable here */
/*dcs.addDevNColorRef   = not applicable here */
/*dcs.remDevNColorRef   = not applicable here */

  info.halftone         = defaultHalftone;
  info.screenphase.x    = 0;
  info.screenphase.y    = 0;
  info.color.rgbw.red   = r;
  info.color.rgbw.green = g;
  info.color.rgbw.blue  = b;
  info.color.rgbw.white = MAXCOLOR8;
  info.colorSpace       = &dcs;
  info.priv.ptr         = (void*) PSGetGStateExt(NULL);
  info.overprint        = false;
  info.cachedPattern    = NIL;
  info.transfer         = NIL;
  info.shading          = false;
  info.xfer16Info       = NIL;
  info.shColorHdr       = NIL;

  SetupPattern ( ((PFmStuff)device)->marker->pattern, &info, &patData,
	((PFmStuff)device)->marker->planeToMark);

  if (!patData.constant)
     return(false);

  arrayView = (char*) &patData.value;

#if (SCANUNIT == 64)

  if ((arrayView[0] == arrayView[1])
  &&  (arrayView[0] == arrayView[2])
  &&  (arrayView[0] == arrayView[3])
  &&  (arrayView[0] == arrayView[4])
  &&  (arrayView[0] == arrayView[5])
  &&  (arrayView[0] == arrayView[6])
  &&  (arrayView[0] == arrayView[7]))
     return(true);
  else
    return(false);

#else /* (SCANUNIT == 64) */

  if ((arrayView[0] == arrayView[1])
  &&  (arrayView[0] == arrayView[2])
  &&  (arrayView[0] == arrayView[3]))
     return(true);
  else
    return(false);

#endif /* (SCANUNIT == 64) */

}

/*****************************************************************************/
/*                                                                           */
/*  AddToHash                                                                */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure AddToHash ARGDEF5(Card32 *,table, unsigned char, r, unsigned char, g, unsigned char, b, unsigned char, pixel)
{
  Card32 mapEntry, newEntry;
  Card32 *mapPtr, *endOfMap;

  newEntry = ComposeEntry(r, g, b, pixel);
  mapPtr = &table[Hash(r, g, b)];
  endOfMap = table + TableSize;

  while (1)
    {
    if ( (mapEntry = *mapPtr) == 0) break;
    if (Matches(newEntry, mapEntry)) return;
    if (mapPtr == endOfMap) mapPtr = table;
    else mapPtr++;
    }	/* End of while */

  *mapPtr = newEntry;
  }

/*****************************************************************************/
/*                                                                           */
/*  FreeActualPattern                                                        */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure FreeActualPattern ARGDEF1(ActualPatternObj *,h)
{
  sys_free(h);
  }

/*****************************************************************************/
/*                                                                           */
/*  ActualColorSetupPattern                                                  */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure ActualColorSetupPattern ARGDEF4(
  PatternHandle, h,
  DevMarkInfo *, markinfo,
  PatternData *, data,
  integer, plane)
  /* Fill in the 'data' parameter with the actual color	*/
  /* that corresponds to the color spec'd in markinfo.	*/
  /* If no actual color exists, We call the devpattern	*/
  /* package to fill in 'data'.				*/ 
  {
  Card32 index;
  register Card32 *mapPtr, mapEntry, target;
  DevColor color;
  ActualPatternObj *a = (ActualPatternObj *)h;

  color = markinfo->color;

  DebugAssert ( ( ((PFmStuff)(a->xWinInfo->device))->marker->dcc == dccK   ) ||
                ( ((PFmStuff)(a->xWinInfo->device))->marker->dcc == dccRGB )  );

  target = ComposeEntry(color.rgbw.red, color.rgbw.green, color.rgbw.blue, 0);
  index = Hash(color.rgbw.red, color.rgbw.green, color.rgbw.blue);
  mapPtr = &a->xWinInfo->vInfo.actualHash[index];
  if ((mapEntry = *mapPtr) == 0)
    {
    SetupPattern(a->origPat, markinfo, data, plane);
    return;
    }
  else
    {
    Card32 *endOfMap;

    if (RGBMatches(target, mapEntry))
      {
      FillInData(mapEntry, data);
      return;
      }

    endOfMap = a->xWinInfo->vInfo.actualHash + TableSize;
    while (1)
      {
      if (mapPtr == endOfMap)
        mapPtr = a->xWinInfo->vInfo.actualHash;
      else mapPtr++;
      if ( (mapEntry = *mapPtr) == 0) break;
      if (RGBMatches(target, mapEntry))
        {
        FillInData(mapEntry, data);
        return;
        }
      }	/* End of while */
    }	/* End of else	*/
  SetupPattern(a->origPat, markinfo, data, plane);
  }

/*****************************************************************************/
/*                                                                           */
/*  ActualPatternInfo                                                        */
/*                                                                           */
/*****************************************************************************/

PRIVATE integer ActualPatternInfo ARGDEF6(PatternHandle, h, DevColorData *,red, DevColorData *,green, DevColorData *,blue, DevColorData *,gray, SCANTYPE *,firstColor)
{
  ActualPatternObj *a = (ActualPatternObj *)h;

  return(
    (*(a->origPat->patternInfo))(
      a->origPat, red, green, blue, gray, firstColor));
  }

/*****************************************************************************/
/*                                                                           */
/*  ActualDestroyPattern                                                     */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure ActualDestroyPattern ARGDEF1(PatternHandle, h)
                  
  /* Destroys the pattern object and all resources it 	*/
  /* maintains including the underlying pattern from	*/
  /* devpattern package. The handle is no longer valid	*/
  /* after it is destroyed.				*/
  {
  ActualPatternObj *a = (ActualPatternObj *)h;

  DestroyPattern(a->origPat);
  sys_free(a);
  }

/*****************************************************************************/
/*                                                                           */
/*  ActualPattern                                                            */
/*                                                                           */
/*****************************************************************************/

PRIVATE ActualPatternObj *ActualPattern ARGDEF2(PDevice, device, PatternHandle, h)
  /* Create a pattern object that deals with 'actual'	*/
  /* colors. The return value is a new pattern handle	*/
  /* whose procedures know how deal with actual colors	*/
  /* and how to use the original pattern handle if no	*/
  /* adequate actual color exists.			*/ 
  {
  ActualPatternObj *new;

  new = (ActualPatternObj *)sys_malloc(sizeof(ActualPatternObj));
  if (new == NIL) return(NIL);
  new->pat.setupPattern = ActualColorSetupPattern;
  new->pat.destroyPattern = ActualDestroyPattern;
  new->pat.patternInfo = ActualPatternInfo;
  new->origPat = h;
  new->xWinInfo = XWININFO_FROM_DEVICE(device);
  return(new);
  }

/*-------------------*/
/* Public procedures */
/*-------------------*/

/*****************************************************************************/
/*                                                                           */
/*  XDPS_UpdateActualPatternn                                                */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean XDPS_UpdateActualPattern ARGDEF2(
  PDevice, old, PDevice, new)
{
  void (*proc)ARGDECL4(PatternHandle, h,
		      DevMarkInfo *, markinfo,
		      PatternData *, data,
			  integer, plane) = ActualColorSetupPattern;

  ActualPatternObj *a = (ActualPatternObj *) ( ((PFmStuff)old)->marker->pattern);

  if (a->pat.setupPattern == proc)
  {
     XWinInfo *new_xWinInfo = XWININFO_FROM_DEVICE(new);
     XWinInfo *old_xWinInfo = XWININFO_FROM_DEVICE(old);

     new_xWinInfo->vInfo.actual       = old_xWinInfo->vInfo.actual;
     new_xWinInfo->vInfo.actualsInUse = old_xWinInfo->vInfo.actualsInUse;
     new_xWinInfo->vInfo.actualHash   = old_xWinInfo->vInfo.actualHash;

     a->xWinInfo = new_xWinInfo;

     DestroyPattern ( ((PFmStuff)new)->marker->pattern );

     ((PFmStuff)new)->marker->pattern = ((PFmStuff)old)->marker->pattern;

     return (true);
  }
  else
     return (false);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_SetXRGBActual                                                       */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean XDPS_SetXRGBActual ARGDEF3(PDevice, device, DevColor *,devColor, DevInputColor *,devInputColor)
{
  XWinInfo *xWinInfo = XWININFO_FROM_DEVICE(device);
  unsigned char  red8,  green8,  blue8;
  unsigned short red16, green16, blue16;
  unsigned long pixel;
  ActualPatternObj *h;

  if (!DPSSys_IsAnXDevice(device))
  {
    PSUndefined();
    return(false);
  }

  if ( ( ((PFmStuff)device)->marker->dcc != dccK   ) &&
       ( ((PFmStuff)device)->marker->dcc != dccRGB )  )
     return(false);

  red8   = devColor->rgbw.red;
  green8 = devColor->rgbw.green;
  blue8  = devColor->rgbw.blue;

  red16   = (unsigned short) (devInputColor->value.rgb.red   * 65535.0 + 0.5);
  green16 = (unsigned short) (devInputColor->value.rgb.green * 65535.0 + 0.5);
  blue16  = (unsigned short) (devInputColor->value.rgb.blue  * 65535.0 + 0.5);

  if (xWinInfo->vInfo.colorMap == (XID)None)   return(false);
  if (SolidColor(device, red8, green8, blue8)) return(true);

  {
     DevColorData red, green, blue, gray;
     SCANTYPE firstColor;

     integer bits_per_pixel = (*(((PFmStuff)device)->marker->pattern->patternInfo))
                                     (((PFmStuff)device)->marker->pattern,
                                     &red, &green, &blue, &gray, &firstColor);

     if (bits_per_pixel != 8) 
        return(false);
  }

  if (xWinInfo->vInfo.actualsInUse == 0)
  {
     xWinInfo->vInfo.actualHash = (Card32 *)sys_calloc(TableSize, sizeof(Card32));

     if (xWinInfo->vInfo.actualHash == NIL)
        return(true);

     h = ActualPattern (device, ((PFmStuff)device)->marker->pattern);

     if (h == NIL)
     {
        sys_free(xWinInfo->vInfo.actualHash);
        return(false);
     }

     if (XDPS_AllocColor(xWinInfo, &red16, &green16, &blue16, &pixel) == false)
     {
        FreeActualPattern(h);
        sys_free(xWinInfo->vInfo.actualHash);
        return(false);
     }

     ((PFmStuff)device)->marker->pattern = (PatternHandle)h;
  }
  else
  {
     if (XDPS_AllocColor(xWinInfo, &red16, &green16, &blue16, &pixel) == false)
        return(false);
  }

  AddToHash(xWinInfo->vInfo.actualHash, red8, green8, blue8, pixel);
  xWinInfo->vInfo.actualsInUse++;
  return(true);
}
