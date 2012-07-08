/* @(#)CM_VerSion xdirect.c 3011 1.5 22350.eco sum= 11539 3011.004 */
/* @(#)CM_VerSion xdirect.c 3010 1.9 20436.eco sum= 07583 3010.103 */
/* @(#)CM_VerSion xdirect.c 2017 1.10 17334.eco sum= 09283 2017.011 */
/* @(#)CM_VerSion xdirect.c 2016 1.6 14560.eco sum= 38040 */
/* @(#)CM_VerSion xdirect.c 2015 1.9 13214.eco sum= 30429 */
/* @(#)CM_VerSion xdirect.c 2014 1.6 08790.eco sum= 56974 */
/* @(#)CM_VerSion xdirect.c 2013 1.9 08577.eco sum= 31253 */
/*
  xdirect.c

(c) Copyright 1990-1995 Adobe Systems Incorporated.
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
*  File: xdirect.c                                                            *
*                                                                             *
*******************************************************************************
*                                                                             *
*  The PUBLIC procedures in this file are declared extern in xdirect.h        *
*                                                                             *
*  This file implements the X Direct Access Device.                           *
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
*    DirectAccess                                                             *
*                                                                             *
*  PUBLIC PROCEDURES:                                                         *
*                                                                             *
*    XDirect_Init                                                             *
*    XDirect_Create                                                           *
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
#include "winclip.h"
/* XYZ: Backdoor into device package, not clean */

#define XWININFO_FROM_DEVICE(device)    \
	( (XWinInfo*) ( (WinInfo*)(device)->priv.ptr )->window )

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "xdirect.h"
#include "genxsupport.h"

#ifdef DPS_X11R5
#include "X_server_internals.h"
#endif

/*----------------------------*/
/* Include files from X world */
/*----------------------------*/

#define  NEED_EVENTS
#include "scrnintstr.h"	/* USING [_Screen]		*/
#include "resource.h"	/* USING [LookupIDByType]	*/
#include "pixmapstr.h"	/* USING [_Drawable]		*/

#if XDPSCONF_PREFER_BACKING_STORE_TO_DIRECT
#include "windowstr.h"  /* USING [_Window]              */
#endif

/*-----------*/
/* constants */
/*-----------*/

/*
 * The direct device can only handle windows on platforms which
 * have memory mapped framebuffers, so that windows look just like pixmaps.
 * 
 * XXX We need a better test for this!
 */
#ifndef XDPS_DIRECT_WINDOW_ACCESS_AVAILABLE
#define	XDPS_DIRECT_WINDOW_ACCESS_AVAILABLE \
	(OS == os_sun || OS == os_solaris || OS == os_ultrix)
#endif

/*
 * The direct device doesn't support backing store (yet).
 * If you want to force the use of the window device when
 * the user requests backing store, this is the macro for you.
 */
#ifndef XDPSCONF_PREFER_BACKING_STORE_TO_DIRECT
#define	XDPSCONF_PREFER_BACKING_STORE_TO_DIRECT	0
#endif

/*-----------------*/
/* local variables */
/*-----------------*/

PRIVATE PDevProcs  directXProcs;

/*--------------------*/
/* PRIVATE procedures */
/*--------------------*/

/*****************************************************************************/
/*                                                                           */
/*  Cleanup                                                                  */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure Cleanup (XWinInfo *xWinInfo)
{
  PSDestroyMarker ( ( (PFmStuff) xWinInfo->device )->marker );
  sys_free  ( ( (PFmStuff) xWinInfo->device )->imagePipelineConfig );
}

/*****************************************************************************/
/*                                                                           */
/*  DrawingFunctionChanged                                                   */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure DrawingFunctionChanged (PDevice       device,
                                          GSDrawingFunc old,
                                          GSDrawingFunc new)
{
  if (new != GXcopy)
    XDPS_RemakeDevice(XWININFO_FROM_DEVICE(device));
    /* XDPS_RemakeDevice might change the device out from under	*/
    /* us, so do it last!					*/
}

/*****************************************************************************/
/*                                                                           */
/*  NoteGCChange                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure NoteGCChange (XWinInfo *xWinInfo, GCPtr gc, int changes)
  /* This procedure is registered as the proc to call	*/
  /* when the origGC of a context is modified in some	*/
  /* interesting way (see xDirectGCFuncs). This proc	*/
  /* notices changes to the planemask and takes req'd	*/
  /* action. If the client clip changes, we force a	*/
  /* validation to get the new clip.			*/
{
  if (changes & GCClipMask){ /* xdps.525 */
	if( xWinInfo->drawableID != None )/* xdps.525 */
    		xWinInfo->lastDrawSerial = ~xWinInfo->drawable->serialNumber;
    /* Force a validation before drawing again so that we	*/
    /* pick up the new client clip.				*/
    } /* xdps.525 */
  if (changes & GCPlaneMask)
    XDPS_RemakeDevice(xWinInfo);
    /* XDPS_RemakeDevice might change the device out from under	*/
    /* us, so do it last!					*/
}

/*****************************************************************************/
/*                                                                           */
/*  SetupType2Image                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure SetupType2Image (PDevice    genArg,
                                   DevImage  *image,
                                   boolean    copy)
{
PFmStuff srcDev = (PFmStuff)genArg;
PDevFrameMarker marker;
  marker = (PDevFrameMarker) srcDev->marker;
  image->source->samples[0] = (unsigned char *) marker;
  (*marker->SetupImageSamples)(image, copy);
  XDPS_SetupType2Image((PDevice)srcDev, image, copy);
}

/*****************************************************************************/
/*                                                                           */
/*  DirectAccess                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE boolean DirectAccess (XID drawID, XID gcID)
{
	PSParamValue pval;
	GSDrawingFunc *pFunc;
	GCPtr gc;
	DrawablePtr drawable = 0;

	if (prm_GetParam("internal", "ForceMarker", &pval)) {
		switch (pval.strval[0]) {
		case 'd':	/* "direct" */
			return true;
		case 'w':	/* "window" */
			return false;
		case 'n':	/* "none" */
		default:
			break;
		}
	}
	else
		CantHappen();

#if (OS == os_osf1) || (OS == os_irixV) || (OS == os_irix6) || (OS == os_solaris)
	return (false);
#endif

	/*-------------------------------------------------------------------*/
	/* The direct device can't handle the X drawing modes, so if we're   */
	/* using a mode other than GXCopy, we cannot create a direct device. */
	/*-------------------------------------------------------------------*/

	pFunc = (GSDrawingFunc *) PSGetGStateExt(NULL);
	if (pFunc != NULL && GetDrawingFunction(pFunc) != GXcopy)
		return (false);

	/*---------------------------------------------------*/
	/* The direct device can't handle planemasks either. */
	/*---------------------------------------------------*/

	gc = (GCPtr) LookupIDByType(gcID, RT_GC);
	if (gc->planemask != ~0)
		return (false);

#if XDPS_DIRECT_WINDOW_ACCESS_AVAILABLE || XDPSCONF_PREFER_BACKING_STORE_TO_DIRECT
	drawable = (DrawablePtr) LookupIDByClass(drawID, RC_DRAWABLE);

	/*----------------------------------------------------------*/
	/* The direct device can't handle windows on some platforms */
	/*----------------------------------------------------------*/

	if (drawable->type == DRAWABLE_WINDOW)
	{
#if XDPS_DIRECT_WINDOW_ACCESS_AVAILABLE == 0
		return false;
#endif /* XDPS_DIRECT_WINDOW_ACCESS_AVAILABLE */

		/*--------------------------------------------------------*/
		/* We may prefer backing store to using the direct device */
		/*--------------------------------------------------------*/

#if XDPSCONF_PREFER_BACKING_STORE_TO_DIRECT
		if (((WindowPtr) drawable)->backingStore != NotUseful)
			return false;
#endif /* XDPSCONF_PREFER_BACKING_STORE_TO_DIRECT */
	}
#endif /* XDPS_DIRECT_WINDOW_ACCESS_AVAILABLE || XDPSCONF_PREFER_BACKING_STORE_TO_DIRECT */

	/*-------------------------------------------------*/
	/* Finally,  we can actually use the direct device */
	/*-------------------------------------------------*/

	return true;
}

/*-------------------*/
/* Public procedures */
/*-------------------*/

/*****************************************************************************/
/*                                                                           */
/*  XDirect_Init                                                             */
/*                                                                           */
/*****************************************************************************/

PUBLIC PDevProcs XDirect_Init (void)
{
  directXProcs = (PDevProcs) sys_sureMalloc ( (long int) sizeof(DevProcs) );

  *directXProcs = *fmProcs;

  directXProcs->ViewBounds          = XDPS_ViewBounds;
  directXProcs->WinToDevTranslation = XDPS_WinToDevTranslation;
  directXProcs->GoAway              = XDPS_GoAway;
  directXProcs->InitPage            = XDPS_InitPage;
  directXProcs->VisibleRegions      = XDPS_VisibleRegions;
  directXProcs->Type2ImageOK        = XDPS_Type2ImageOK;
  directXProcs->SetupType2Image     = SetupType2Image;
  directXProcs->CheckForOverlap     = XDPS_CheckForOverlap;
  directXProcs->WindowOffset        = XDPS_WindowOffset;
  directXProcs->BPPMatch            = XDPS_BPPMatch;

  return (directXProcs);
}

/*****************************************************************************/
/*                                                                           */
/*  XDirect_Create                                                           */
/*                                                                           */
/*****************************************************************************/

PUBLIC int XDirect_Create (PDevice      *device_ptr_ptr,
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
  PixmapPtr         pixmap_ptr;
  DrawablePtr       drawable_ptr;
  XWinInfo         *xWinInfo;
  PDevMarker        pmarker;
  DCPixelArgs       pixelArgs;
  PFmStuff          fmStuff;
  Mtx               matrix;
  PImagePipelineConfig pipConfig;
  PSParamValue      pval;
  integer           HalftoneThreshold;
  integer           MaxDeviceWidth  = 0;
  integer           MaxDeviceHeight = 0;
  FrameDeviceParamsRec params;
  integer xShift;

  if (!DirectAccess(drawID, gcID))
    return (CANT_CREATE);

  status = XDPS_Setup_xWinInfo (&xWinInfo, (DPSSysClient) client, drawID, gcID,
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

  /*-------------------*/
  /* Set up pixmap_ptr */
  /*-------------------*/

  if (drawable_ptr->type == DRAWABLE_WINDOW)
    pixmap_ptr = (PixmapPtr) drawable_ptr->pScreen->devPrivate;
  else
    pixmap_ptr = (PixmapPtr) drawable_ptr;

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

  if (!AllocImageBuffers(&pixelArgs, 1, XDPS_await_pages, NULL))
     return (CANT_CREATE);

  pmarker = PSXdpsMarker ( (PSCANTYPE*)   &pixmap_ptr->devPrivate.ptr,  /* frameBase      */
                           (integer)       pixmap_ptr->devKind,         /* frameByteWidth */
                           (integer)       0,                           /* firstScanLine  */
                           (integer)       MaxDeviceHeight,             /* height         */
                           (PDCPixelArgs) &pixelArgs,                   /* pixelArgs      */
                           (integer)       XDPS_DIRECT_MARKER);         /* markerType     */

  if (pmarker == NIL)
     return (CANT_CREATE);

  /*-------------------------------------------------------*/
  /* set the priv field in the marker to point to xwininfo */
  /*-------------------------------------------------------*/

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

  XDPS_SetupImagePipelineConfig (XDEVTYPE_DIRECT, &pixelArgs, pipConfig);

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

  return (Success);
}
