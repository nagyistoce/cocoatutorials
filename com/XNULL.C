/* @(#)CM_VerSion xnull.c 3011 1.5 22350.eco sum= 48158 3011.004 */
/* @(#)CM_VerSion xnull.c 3010 1.8 19928.eco sum= 39841 3010.102 */
/* @(#)CM_VerSion xnull.c 2017 1.6 17334.eco sum= 25330 2017.011 */
/* @(#)CM_VerSion xnull.c 2016 1.5 14560.eco sum= 42672 */
/* @(#)CM_VerSion xnull.c 2015 1.8 12985.eco sum= 13973 */
/* @(#)CM_VerSion xnull.c 2014 1.6 08790.eco sum= 01451 */
/* @(#)CM_VerSion xnull.c 2013 1.7 08588.eco sum= 32940 */
/*
  xnull.c

(c) Copyright 1990-1993 Adobe Systems Incorporated.
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
*  File: xnull.c                                                              *
*                                                                             *
*******************************************************************************
*                                                                             *
*  The PUBLIC procedures in this file are declared extern in xnull.h          *
*  This file implements the X Null Device.                                    *
*                                                                             *
*******************************************************************************
*                                                                             *
*  PRIVATE PROCEDURES:                                                        *
*                                                                             *
*    Cleanup                                                                  *
*    DrawingFunctionChanged                                                   *
*    NoteGCChange                                                             *
*                                                                             *
*    nullWinToDevTranslation                                                  *
*                                                                             *
*  PUBLIC PROCEDURES:                                                         *
*                                                                             *
*    XNull_Init                                                               *
*    XNull_Create                                                             *
*    XDPS_IsDummyXNullDevice                                                  *
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
#include CANTHAPPEN
#include PARAMETERS
#include FRAMEMARKER
#include DEVICE
#include EXCEPT

#include "devcommon.h"
#include "framedev.h"
/* XYZ: Backdoor into device package, not clean */

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "xnull.h"
#include "genxsupport.h"

/*-----------------*/
/* local variables */
/*-----------------*/

PRIVATE PDevProcs nullXProcs;
PRIVATE PDevice   DummyXNullDevice;

/*--------------------*/
/* PRIVATE procedures */
/*--------------------*/

/*****************************************************************************/
/*                                                                           */
/*  Cleanup                                                                  */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure Cleanup ARGDEF1(XWinInfo *,xWinInfo)
{
  PSDestroyMarker ( ( (PFmStuff) xWinInfo->device )->marker );
  sys_free  ( ( (PFmStuff) xWinInfo->device )->imagePipelineConfig );
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
  return; /* nothing to do here */
}

/*****************************************************************************/
/*                                                                           */
/*  NoteGCChange                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure NoteGCChange ARGDEF3(XWinInfo *,xWinInfo, GCPtr, gc, int, changes)
{
  return; /* nothing to do here */
}

/*****************************************************************************/
/*                                                                           */
/*  nullWinToDevTranslation                                                  */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure nullWinToDevTranslation ARGDEF2(PDevice, device, DevPoint *,translation)
{
  translation->x = translation->y = 0;
  }

/*-------------------*/
/* Public procedures */
/*-------------------*/

/*****************************************************************************/
/*                                                                           */
/*  XNull_Init                                                               */
/*                                                                           */
/*****************************************************************************/

PUBLIC PDevProcs XNull_Init ARGDEF0()
{
  int result;
  nullXProcs = (PDevProcs) sys_sureMalloc ( (long int) sizeof(DevProcs) );

  *nullXProcs = *fmProcs;

  nullXProcs->Mark                = (DP_MarkProc) DevNoOp;
  nullXProcs->MarkForm            = (DP_MarkFormProc) DevAlwaysFalse;
  nullXProcs->WinToDevTranslation = nullWinToDevTranslation;
  nullXProcs->GoAway              = XDPS_GoAway;
  /*
     To make sure we have at least some kind of device to fall back on
     if the system runs out of memory, we need to create a dummy null
         device for that purpose
  */
  DummyXNullDevice = NULL;
  result = XNull_Create(&DummyXNullDevice, NULL, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  if (result != Success)
        CantHappen();   /* we can't allow this to fail */
  ((PDevice)DummyXNullDevice)->procs = nullXProcs;

  return(nullXProcs);
}

/*****************************************************************************/
/*                                                                           */
/*  XNull_Create                                                             */
/*                                                                           */
/*****************************************************************************/

PUBLIC int XNull_Create (PDevice      *device_ptr_ptr,
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
  int volatile      status;
  XWinInfo         *xWinInfo;
  PDevMarker        pmarker;
  DCPixelArgs       pixelArgs;
  PFmStuff          fmStuff;
  Mtx               matrix;
  PImagePipelineConfig pipConfig;
  PSParamValue      pval;
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

  if (status != Success) {
        *device_ptr_ptr = DummyXNullDevice;
    return (status);
  }

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

  pixelArgs.depth          = (integer)          1;
  pixelArgs.firstColor     = (integer)          colorbase;
  pixelArgs.dcc            = (DevColorantClass) dccK;
  pixelArgs.gray.first     = (integer)          0;
  pixelArgs.gray.n         = (integer)          2;
  pixelArgs.gray.delta     = (integer)          1;
  pixelArgs.doConstPattern = (boolean)          false; /* doesn't matter */
  pixelArgs.isPlanar       = (boolean)          false;

  if (!AllocImageBuffers(&pixelArgs, 1, XDPS_await_pages, NULL))
  {
     *device_ptr_ptr = DummyXNullDevice;
     return (CANT_CREATE);
  }

  pmarker = PSXdpsMarker ( (PSCANTYPE*)    frameBase,         /* frameBase      */
                           (integer)       1,                 /* frameByteWidth */
                           (integer)       0,                 /* firstScanLine  */
                           (integer)       1,                 /* height         */
                           (PDCPixelArgs) &pixelArgs,         /* pixelArgs      */
                           (integer)       XDPS_NULL_MARKER); /* markerType     */

  if (pmarker == NIL) {
     *device_ptr_ptr = DummyXNullDevice;
     return (CANT_CREATE);
  }

  /*-------------------------------------------------------*/
  /* set the priv field in the marker to point to xwininfo */
  /*-------------------------------------------------------*/

  PSSetXdpsMarkerPriv ( pmarker, (DevPrivate *) xWinInfo );

  /*-------------------------------------------------*/
  /* set up the initial matrix and create the device */
  /*-------------------------------------------------*/

  matrix.a   =  1.0;
  matrix.b   =  0.0;
  matrix.c   =  0.0;
  matrix.d   = -1.0;
  matrix.tx  =  0.0;
  matrix.ty  =  0.0;

  pipConfig = (PImagePipelineConfig)sys_calloc(sizeof(ImagePipelineConfig), 1);

  if (pipConfig == NIL) {
    PSDestroyMarker( pmarker);
    return CANT_CREATE;
  }

  XDPS_SetupImagePipelineConfig (XDEVTYPE_NULL, &pixelArgs, pipConfig);

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
     *device_ptr_ptr = DummyXNullDevice;
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

  DURING
    XDPS_SetDevParams (xWinInfo,
                       Cleanup,
                       NoteGCChange,
                       DrawingFunctionChanged);
  HANDLER
         *device_ptr_ptr = DummyXNullDevice;
     status = CANT_CREATE;
  END_HANDLER

  if (status == CANT_CREATE)
        return (status);
  else
        return(Success);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_IsDummyXNullDevice                                                  */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean XDPS_IsDummyXNullDevice ARGDEF1(PDevice, device)
{
  if (device == (PDevice)DummyXNullDevice)
        return (true);
  else
        return (false);
}

