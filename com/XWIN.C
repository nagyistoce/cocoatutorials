/* @(#)CM_VerSion xwin.c 3011 1.4 22350.eco sum= 60206 3011.004 */
/* @(#)CM_VerSion xwin.c 3010 1.11 20436.eco sum= 15246 3010.103 */
/* @(#)CM_VerSion xwin.c 2017 1.6 17841.eco sum= 26211 2017.013 */
/* @(#)CM_VerSion xwin.c 2016 1.4 13872.eco sum= 37342 */
/* @(#)CM_VerSion xwin.c 2015 1.9 12669.eco sum= 01215 */
/* @(#)CM_VerSion xwin.c 2014 1.4 08790.eco sum= 63426 */
/* @(#)CM_VerSion xwin.c 2013 1.6 08577.eco sum= 46860 */
/*
  xwin.c

(c) Copyright 1989-1993 Adobe Systems Incorporated.
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
*  File: xwin.c                                                               *
*                                                                             *
*******************************************************************************
*                                                                             *
*  The PUBLIC procedures in this file are declared extern in xwin.h           *
*                                                                             *
*  This file implements the X Device based on X imaging primitives            *
*                                                                             *
*  NOTES:                                                                     *
*  - We implement a one element cache of pixmaps that have been               *
*    "discarded". When we try to add a pixmap to the tile cache               *
*    and find that we have no room, instead of deleting it                    *
*    immediately, we hold on to it on the assumption that GetNewTile          *
*    will want to use it some time soon. If a pixmap is discarded             *
*    when another is already in the one element cache, the latter             *
*    is deleted and the former is placed in the cache.                        *
*                                                                             *
*******************************************************************************
*                                                                             *
*  PRIVATE PROCEDURES:                                                        *
*                                                                             *
*    Cleanup                                                                  *
*    DrawingFunctionChanged                                                   *
*    NoteGCChange                                                             *
*    SetupType2Image                                                          *
*                                                                             *
*    CreateShadow                                                             *
*    SetColor                                                                 *
*    InitColor                                                                *
*                                                                             *
*  PUBLIC PROCEDURES:                                                         *
*                                                                             *
*    XWin_Create                                                              *
*    XWin_Init                                                                *
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
#include DEVIMAGE
#include PSXDPSMARKER
#include DPSCUSTOMOPS
#include CANTHAPPEN
#include PARAMETERS
#include FRAMEMARKER
#include DEVICE

#include "framedev.h"
#include "devcommon.h"  /* USING [defaultHalftone] */
#include "winclip.h"    /* USING [WinInfo]         */
/* XYZ: Backdoor into device package, not clean */

#define XWININFO_FROM_DEVICE(device)    \
	( (XWinInfo*) ( (WinInfo*)(device)->priv.ptr )->window )

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "xwin.h"
#include "genxsupport.h"

#ifdef DPS_X11R5
#include "X_server_internals.h"
#endif

/*----------------------------*/
/* Include files from X world */
/*----------------------------*/

#define  NEED_EVENTS
#include "scrnintstr.h" /* USING [_Screen]              */
#include "resource.h"   /* USING [LookupIDByType]       */
#include "pixmapstr.h"  /* USING [_Drawable]            */

/*-----------*/
/* constants */
/*-----------*/

#define XWinInterests           \
        (GCPlaneMask | GCSubwindowMode | GCClipXOrigin |        \
        GCClipYOrigin | GCClipMask)

/* XYZ: consolidate with PixFromValueYY in xdpsmarker.c */
#define XX_bitsPerPixel(device)  ((XWININFO_FROM_DEVICE(device))->drawable->bitsPerPixel)

#define	PixFromValueXX(device, v)	\
	((v) & ((unsignedlonginteger) ~0 >> ((PREFERREDALIGN * 8) - XX_bitsPerPixel(device) )))
  /* And the value 'v' with a mask containing 1's in	*/
  /* the low-order 'bitsPerPixel' bits.			*/

/* 
Undefine max and min macros to avoid confusion with decode structure.
Should eventually move decode assignement to image2.c in the graphics
package instead of undefing max and min 
*/
#undef max
#undef min

/*-----------------*/
/* local variables */
/*-----------------*/

PRIVATE PDevProcs winXProcs;
PRIVATE DevImSampleDecode decode[4];

/*--------------------*/
/* Private procedures */
/*--------------------*/

/*****************************************************************************/
/*                                                                           */
/*  Cleanup                                                                  */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure Cleanup ARGDEF1(XWinInfo *,xWinInfo)
{
  PSDestroyMarker ( ( (PFmStuff) xWinInfo->device )->marker );
  os_free  ( ( (PFmStuff) xWinInfo->device )->imagePipelineConfig );

  (*xWinInfo->maskPixmap->drawable.pScreen->DestroyPixmap) (xWinInfo->maskPixmap);

  FreeGC (xWinInfo->pixmGC,(XID)0);
  FreeGC (xWinInfo->maskGC,(XID)0);

  if (xWinInfo->imageGC != NULL)
     FreeGC (xWinInfo->imageGC,(XID)0);

  FreeScratchGC (xWinInfo->gc);
}

/*****************************************************************************/
/*                                                                           */
/*  DrawingFunctionChanged                                                   */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure DrawingFunctionChanged ARGDEF3(PDevice,       device,
                                                 GSDrawingFunc, old,
                                                 GSDrawingFunc, new)
{
  if (new == GXcopy)
    XDPS_RemakeDevice(XWININFO_FROM_DEVICE(device));
  else  /* Drawing function changed - update shadow gc  */
    {
    XID vals[1];
    long int mask;

    mask = GCFunction;
    vals[0] = (XID)new;
    ChangeGC(XWININFO_FROM_DEVICE(device)->gc, mask, vals);
    }
  }

/*****************************************************************************/
/*                                                                           */
/*  NoteGCChange                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE void NoteGCChange ARGDEF3(XWinInfo *,xWinInfo, GCPtr, gc, int, changes)
  /* This procedure is registered as the proc to call   */
  /* when the origGC of a context is modified in some   */
  /* interesting way (see xdpsGCFuncs). This proc       */
  /* simply copies the changed fields into the shadow.  */
  /* If the planemask or clip has changed, then we must */
  /* abandon any optimizations that we may have been    */
  /* making w.r.t. Pixmap devices.                      */
  {
  if (changes & XWinInterests)
    CopyGC(gc, xWinInfo->gc, changes & XWinInterests);
  if (changes & GCClipMask){ /* xdps.525 */
        if( xWinInfo->drawableID != None )/* xdps.525 */
                xWinInfo->lastDrawSerial = ~xWinInfo->drawable->serialNumber;
    /* Force a validation before drawing again so that we       */
    /* pick up the new client clip.                             */
    } /* xdps.525 */
  }

/*****************************************************************************/
/*                                                                           */
/*  SetupType2Image                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure SetupType2Image ARGDEF3(
Device*, genArg,
DevImage*, image,
boolean, copy)
{
PFmStuff srcDev = (PFmStuff)genArg;
DevImageSource *source = image->source;
integer i;
/* DrawablePtr drawable_ptr = (XWININFO_FROM_DEVICE((Device *)srcDev))->drawable;  */

  image->invert = false;
  image->sampleProc = NULL;
  image->procData = NULL;
  for (i = 0; i < source->nComponents; i++) {
    decode[i].min = 0;
    decode[i].max = 1;
  }
  source->decode = decode;
  XDPS_SetupType2Image((PDevice)srcDev, image, true);
}

/*****************************************************************************/
/*                                                                           */
/*  CreateShadow                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE GCPtr CreateShadow ARGDEF1(XWinInfo *,xWinInfo)
  /* Create a gc which shadows the origGC specified by  */
  /* the client. Wrap the funcs vector of origGC so we  */
  /* track any changes made to interesting fields.      */

  {
  GCPtr gc, origGC;
  XID vals[3];
  long int mask;

  gc = (GCPtr) CreateScratchGC(xWinInfo->drawable->pScreen, xWinInfo->drawable->depth);
  origGC = xWinInfo->origGC;
  CopyGC(origGC, gc, ~0);       /* Copy Everything!     */

  mask = GCFunction | GCTileStipXOrigin | GCTileStipYOrigin;
  {
     GSDrawingFunc *pFunc;

     pFunc = (GSDrawingFunc *)PSGetGStateExt(NULL);
     vals[0] = (XID) GetDrawingFunction(pFunc);
  }

  vals[1] = (XID)0; vals[2] = (XID)0;
  ChangeGC(gc, mask, vals);

  return(gc);
  }

/*****************************************************************************/
/*                                                                           */
/*  XWI_SetColor                                                             */
/*                                                                           */
/*****************************************************************************/

/* XYZ: temporarily PUBLIC because xdpsmarker.c needs it */

PUBLIC procedure XWI_SetColor ARGDEF3(XWinInfo *       , xWinInfo,
                                      long unsigned int, value,
                                      int              , setColorMode)
  /* Set the foreground pixel to the spec'd value. If   */
  /* setColorMode = 1, also change fill mode to solid.  */

  {
  longinteger mask;
  XID vals[3];

  if (setColorMode)
    {
    vals[0] = (XID)value;
    vals[1] = (XID)FillSolid;
    mask = GCForeground | GCFillStyle;
    }
  else
    {
    vals[0] = (XID)value;
    mask = GCForeground;
    }

  ChangeGC(xWinInfo->gc, mask, vals);
  }

/*****************************************************************************/
/*                                                                           */
/*  InitColor                                                                */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure InitColor ARGDEF1(PDevice, device)
  /* This procedure is called to get the color initialized in	*/
  /* the GC as well as in our own data structures. We determine	*/
  /* the pixel value for white and set it as the current color	*/
  /* using SetColor(). It also sets the fill mode to solid.	*/
  /* We note the fact we're in constColor paintMode and the	*/
  /* value for white in the xWinInfo structure.			*/
{
  DevMarkInfo   info;
  PatternData   patData;
  DevColorSpace dcs;

  DebugAssert ( sizeof(info.color.rgbw.red)   == sizeof(Card8) );
  DebugAssert ( sizeof(info.color.rgbw.green) == sizeof(Card8) );
  DebugAssert ( sizeof(info.color.rgbw.blue)  == sizeof(Card8) );
  DebugAssert ( sizeof(info.color.rgbw.white) == sizeof(Card8) );

  dcs.priv              = NIL;
  dcs.colorSpace        = DEVGRAY_COLOR_SPACE;
/*dcs.cie               = not applicable here */
/*dcs.devNInfo          = not applicable here */
/*dcs.createDevNColor   = not applicable here */
/*dcs.addDevNColorRef   = not applicable here */
/*dcs.remDevNColorRef   = not applicable here */

  info.halftone         = defaultHalftone;
  info.screenphase.x    = 0;
  info.screenphase.y    = 0;
  info.color.rgbw.red   = MAXCOLOR8;
  info.color.rgbw.green = MAXCOLOR8;
  info.color.rgbw.blue  = MAXCOLOR8;
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

  XWININFO_FROM_DEVICE(device)->paintMode = constColor;
  XWININFO_FROM_DEVICE(device)->curColor = patData.value;
  XWI_SetColor(XWININFO_FROM_DEVICE(device), PixFromValueXX(device, patData.value), 1);
}

/*-------------------*/
/* Public procedures */
/*-------------------*/

/*****************************************************************************/
/*                                                                           */
/*  XWin_Init                                                                */
/*                                                                           */
/*****************************************************************************/

PUBLIC PDevProcs XWin_Init ARGDEF0()
{
  winXProcs = (PDevProcs) sys_sureMalloc ( (long int) sizeof(DevProcs) );

  *winXProcs = *fmProcs;

  winXProcs->ViewBounds          = XDPS_ViewBounds;
  winXProcs->WinToDevTranslation = XDPS_WinToDevTranslation;
  winXProcs->GoAway              = XDPS_GoAway;
  winXProcs->InitPage            = XDPS_InitPage;
  winXProcs->VisibleRegions      = XDPS_VisibleRegions;
  winXProcs->Type2ImageOK        = XDPS_Type2ImageOK;
  winXProcs->SetupType2Image     = SetupType2Image;
  winXProcs->CheckForOverlap     = XDPS_CheckForOverlap;
  winXProcs->WindowOffset        = XDPS_WindowOffset;
  winXProcs->BPPMatch            = XDPS_BPPMatch;

  return (winXProcs);
}

/*****************************************************************************/
/*                                                                           */
/*  XWin_Create                                                              */
/*                                                                           */
/*****************************************************************************/

PUBLIC int XWin_Create (PDevice      *device_ptr_ptr,
                        DPSSysClient  client,
                        XID           drawID,
                        XID           gcID,
                        Card32        redmax,
                        Card32        greenmax,
                        Card32        bluemax,
                        Card32        graymax,
                        Card32        redmult,
                        Card32        greenmult,
                        Card32        bluemult,
                        Card32        graymult,
                        Card32        colorbase,
                        Card32        graybase,
                        Card32        actual,
                        Colormap      cmap,
                        Int16         xOff,
                        Int16         yOff)
{
  int               status;
  integer           wbytes;
  DrawablePtr       drawable_ptr;
  XWinInfo         *xWinInfo;
  PDevMarker        pmarker;
  DCPixelArgs       pixelArgs;
  PFmStuff          fmStuff;
  Mtx               matrix;
  GCPtr             gc_ptr;
  PImagePipelineConfig pipConfig;
  PSParamValue      pval;
  integer           HalftoneThreshold;
  XID               gcvals[5];
  integer           MaxDeviceWidth  = 0;
  integer           MaxDeviceHeight = 0;
  FrameDeviceParamsRec params;
  integer xShift;

  static PSCANTYPE  frameBuffer = NULL;         /* dummy */
  static PSCANTYPE *frameBase   = &frameBuffer; /* dummy */

  status = XDPS_Setup_xWinInfo (&xWinInfo, client, drawID, gcID,
                                redmax,  greenmax,  bluemax,  graymax,
                                redmult, greenmult, bluemult, graymult,
                                colorbase,  graybase, actual, cmap,
                                xOff, yOff);

  /*------------------------------------------------------*/
  /* here xOff and yOff have been adjusted for bitgravity */
  /*------------------------------------------------------*/

  if (status != Success)
     return (status);

  drawable_ptr =
  xWinInfo->drawable = (DrawablePtr) LookupIDByClass(drawID, RC_DRAWABLE);

  /*----------------------*/
  /* Get max device space */
  /*----------------------*/

  if ( prm_GetParam ("internal", "MaxDeviceWidth", &pval) )
     MaxDeviceWidth = pval.ival;
  else
     CantHappen();

  if ( prm_GetParam ("internal", "MaxDeviceHeight", &pval) )
     MaxDeviceHeight = pval.ival;
  else
     CantHappen();

  /*--------------------*/
  /* Set up DCPixelArgs */
  /*--------------------*/

  os_bzero (&pixelArgs, sizeof(pixelArgs) );

  pixelArgs.depth = (integer) xWinInfo->vInfo.depth;

  if ( prm_GetParam ("system", "HalftoneThreshold", &pval) )
     HalftoneThreshold = pval.ival;
  else
     HalftoneThreshold = 16; /* fall back to default */

  if ( (redmax == 0) || (greenmax == 0) || (bluemax == 0) )
  {
     if ( (graymax == 0) || (graymult == 0) )
        return (CANT_CREATE);

     pixelArgs.firstColor = (integer) 0;
     pixelArgs.dcc        = (DevColorantClass) dccK;
     pixelArgs.isPlanar   = (boolean) true;

     if (graymax >= HalftoneThreshold-1)
        pixelArgs.doConstPattern = true;
     else
        pixelArgs.doConstPattern = false;
  }
  else
  {
     if ( (redmax  == 0) || (greenmax  == 0) || (bluemax  == 0)
       || (redmult == 0) || (greenmult == 0) || (bluemult == 0) )
        return (CANT_CREATE);

     pixelArgs.firstColor = (integer) colorbase;
     pixelArgs.isPlanar   = (boolean) false;

     if (graymax == 0)
        pixelArgs.dcc = (DevColorantClass) dccRGB;
     else
     {
        if (graymult == 0)
           return (CANT_CREATE);

        pixelArgs.dcc = (DevColorantClass) dccRGB;
     }

     if ( (redmax   >= HalftoneThreshold-1)
     &&   (greenmax >= HalftoneThreshold-1)
     &&   (bluemax  >= HalftoneThreshold-1)
     &&   (graymax  >= HalftoneThreshold-1 || graymax == 0) )
        pixelArgs.doConstPattern = true;
     else
        pixelArgs.doConstPattern = false;

     XDPS_ConvertToDPSRamp (redmax,   redmult,   &pixelArgs.red);
     XDPS_ConvertToDPSRamp (greenmax, greenmult, &pixelArgs.green);
     XDPS_ConvertToDPSRamp (bluemax,  bluemult,  &pixelArgs.blue);
  }

  XDPS_ConvertToDPSRamp (graymax,  graymult,  &pixelArgs.gray);

  pixelArgs.gray.first += graybase;

  /*----------------------------------------------------------------------------*/
  /* For the monochrome case we cannot assume that 0 and 1 are white and black. */
  /*----------------------------------------------------------------------------*/

  if ( (pixelArgs.gray.n == 2) && (pixelArgs.gray.first == 1) && (pixelArgs.red.n == 0) )
     XDPS_GetMono (xWinInfo, &pixelArgs.gray);

  if (drawable_ptr->type == DRAWABLE_WINDOW) {
    wbytes = (integer)(drawable_ptr->bitsPerPixel * drawable_ptr->width) >> 3;
    /* round up to nearest SCANUNIT */
    wbytes = (wbytes + sizeof(SCANUNIT) - 1) & (~(sizeof(SCANUNIT) - 1));
  } else
    wbytes = ((PixmapPtr) drawable_ptr)->devKind;

  if (!AllocImageBuffers(&pixelArgs, 1, XDPS_await_pages, NULL))
     return (CANT_CREATE);

  pmarker = PSXdpsMarker ( (PSCANTYPE*)    frameBase,           /* frameBase      */
                           (integer)       wbytes,              /* frameByteWidth */
                           (integer)       0,                   /* firstScanLine  */
                           (integer)       MaxDeviceHeight,     /* height         */
                           (PDCPixelArgs) &pixelArgs,           /* pixelArgs      */
                           (integer)       XDPS_WINDOW_MARKER); /* markerType     */

  if (pmarker == NIL)
     return (CANT_CREATE);

  /*-----------------------------------------------------------*/
  /* set the priv field in the xwinmarker to point to xwininfo */
  /*-----------------------------------------------------------*/

  PSSetXdpsMarkerPriv ( pmarker, (DevPrivate *) xWinInfo );

  /*-------------------------------------------------*/
  /* set up the initial matrix and create the device */
  /*-------------------------------------------------*/

  matrix.a   =  ((float)xWinInfo->vInfo.xRes)/(72.0);
  matrix.b   = 0.0;
  matrix.c   = 0.0;
  matrix.d   = -((float)xWinInfo->vInfo.yRes)/(72.0);
  matrix.tx  = 0.0;
  matrix.ty  = 0.0;

  pipConfig = (PImagePipelineConfig)sys_calloc(sizeof(ImagePipelineConfig), 1);

  if (pipConfig == NIL) {
    PSDestroyMarker( pmarker);
    return CANT_CREATE;
  }

  XDPS_SetupImagePipelineConfig (XDEVTYPE_WINDOW, &pixelArgs, pipConfig);

  os_bzero (&params, sizeof(params));

  params.pmtx                       = &matrix;
  params.pagePixelWidth             = MaxDeviceWidth;
  params.pageHeight                 = MaxDeviceHeight;
  params.writeWhite                 = false;
  params.productMaskID              = 0;
  params.marker                     = pmarker;
  params.imagePipelineConfig        = pipConfig;
  params.devRenderingInfo           = NIL;
  params.pageProc                   = NIL;
  params.trackingProcs              = NIL;
  params.procHook                   = NIL;
  params.makingSeparations          = false;
  params.htPixelData.depth          = pixelArgs.depth;
  params.htPixelData.dcc            = pixelArgs.dcc;
  params.htPixelData.isPlanar       = pixelArgs.isPlanar;
  params.htPixelData.doConstPattern = pixelArgs.doConstPattern;
  params.outputOrder                = NIL;

  /* Force device origin abscissa to be on a 32 pixel boundary */
  xShift = (params.pagePixelWidth & 0x3F) >> 1;
  params.initialClippingRegion.bl.x = -xShift;
  params.initialClippingRegion.bl.y = -(params.pageHeight);
  params.initialClippingRegion.tr.x = params.pagePixelWidth - xShift;
  params.initialClippingRegion.tr.y = (params.pageHeight+1)/2;

  fmStuff = (PFmStuff) FrameDevice (&params);

  if (fmStuff == NIL)
  {
     sys_free(pipConfig);
     PSDestroyMarker (pmarker);

     return (CANT_CREATE);
  }

  /*------------------------------------------------------------------------------*/
  /* The FrameDevice() by default calls AdjustDevMatrix (PRIVATE in framedev.c)   */
  /* to compensate for the fact that the level-2 default device origin is in      */
  /* the middle of the marking space (in level-1 it was in the upper left corner. */
  /* In DPS, however, this will not work. The DPS applications expect the default */
  /* device origin to be in the upper left corner.  We therefore now reverse the  */
  /* effect of the call to AdjustDevMatrix().                                     */
  /*------------------------------------------------------------------------------*/

  fmStuff->gen.matrix.tx =
  fmStuff->gen.matrix.ty = 0.0;

  xWinInfo->device = (PDevice) &fmStuff->gen;

  *device_ptr_ptr = xWinInfo->device;

  /*------------------------------------------------------------------*/
  /* NOTE: We always make the maskID zero regardless of the depth of  */
  /* the underlying device. This is because we want to always build 1 */
  /* bit deep masks and expand them on the fly.                       */
  /*------------------------------------------------------------------*/

  XDPS_SetDevParams (xWinInfo,
                     Cleanup,
                     NoteGCChange,
                     DrawingFunctionChanged);

  /*--------------------------------*/
  /* set up the gc and pixmap cache */
  /*--------------------------------*/

  gc_ptr = xWinInfo->gc = CreateShadow(xWinInfo);

  xWinInfo->paintMode  = constColor;    /* Solid Color       */
  xWinInfo->curPatID   = -1;            /* No tile installed */
  xWinInfo->curOffsetY = 0;

  gcvals[0] = GXcopy;
  gcvals[1] = LASTSCANVAL;
  gcvals[2] = 1;
  gcvals[3] = 0;
  gcvals[4] = FillSolid;
  xWinInfo->pixmGC = CreateGC(drawable_ptr, GCFunction | GCPlaneMask |
                              GCForeground | GCBackground | GCFillStyle,
                              gcvals, &status);

  xWinInfo->maskPixmap = (PixmapPtr) (*gc_ptr->pScreen->CreatePixmap)
                                     (drawable_ptr->pScreen, 1, 1, 1);

  xWinInfo->maskGC  = CreateGC((DrawablePtr)xWinInfo->maskPixmap, GCFunction | GCPlaneMask |
                               GCForeground | GCBackground | GCFillStyle,
                               gcvals, &status);

  if (xWinInfo->gc->depth == 1)
     xWinInfo->imageGC = CreateGC(drawable_ptr, GCFunction | GCPlaneMask |
                                 GCForeground | GCBackground | GCFillStyle,
                                 gcvals, &status);

  /* Make serial numbers different to start out... */
  xWinInfo->lastDrawSerial = ~drawable_ptr->serialNumber;

  /* enable use of device clipping */
  xWinInfo->deviceCanClip = true;

  /* enable backing store if allowed */
  if (prm_GetParam("internal", "AllowBackingStore", &pval) &&
        pval.bval) {
        xWinInfo->backingStoreOK = true;
  }

  xWinInfo->scanline         = NIL;
  xWinInfo->scanlineInUse    = false;
  xWinInfo->y = xWinInfo->xl = xWinInfo->xg = 0;

  InitColor (xWinInfo->device);

  return (Success);
}

