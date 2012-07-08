/* @(#)CM_VerSion xdevice.c 3011 1.2 22350.eco sum= 54018 3011.004 */
/* @(#)CM_VerSion xdevice.c 3010 1.6 20436.eco sum= 37160 3010.103 */
/* @(#)CM_VerSion xdevice.c 2017 1.3 17334.eco sum= 44213 2017.011 */
/* @(#)CM_VerSion xdevice.c 2016 1.4 13872.eco sum= 19770 */
/* @(#)CM_VerSion xdevice.c 2015 1.7 13214.eco sum= 60783 */
/* @(#)CM_VerSion xdevice.c 2014 1.3 08790.eco sum= 25925 */
/* @(#)CM_VerSion xdevice.c 2013 1.4 08578.eco sum= 26865 */
/*
  xdevice.c

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
*  File: xdevice.c                                                            *
*                                                                             *
*******************************************************************************
*                                                                             *
*  This file is the main module for the X device implementation.              *
*                                                                             *
*  - This file is separated into two major parts. The first can be            *
*    modified to select the device implementations that are used in this      *
*    version of DPS/X. The second section is system independent and need      *
*    not be modified to add new devices.                                      *
*                                                                             *
*  - The first section contains a list of external declarations for the       *
*    device initialization and creation procedures for each device            *
*    implementation to be included. To customize a system, place the names    *
*    of the device init procs here for each type of device used. Next,        *
*    there is an array of structures that gives the init proc and create      *
*    proc for each device implementation to be included.  The order in        *
*    which they are listed IS significant. This is the order in which the     *
*    init procedures will be called. This is also the order that the          *
*    assoc'd device create proc will be called when trying to create new      *
*    devices.                                                                 *
*                                                                             *
*  - DO NOT delete the entry for the null device, it must always be last!     *
*                                                                             *
*  - The second section contains a couple of utility routines that are        *
*    common to all X device implementations, the DPSSys_IniDevImpl proc       *
*    which is called to initialize the device package as a whole, and         *
*    DPSSys_CreateDevice() which is responsible for creating new devices.     *
*                                                                             *
*******************************************************************************
*                                                                             *
*  PRIVATE PROCEDURES:                                                        *
*                                                                             *
*    <none>                                                                   *
*                                                                             *
*  PUBLIC PROCEDURES:                                                         *
*                                                                             *
*    DPSSys_IsAnXDevice                                                       *
*    DPSSys_IsANullXDevice                                                    *
*    DPSSys_CreateDevice                                                      *
*    DPSSys_IniDevImpl                                                        *
*    DPSSys_CloneDevice                                                       *
*    DPSSys_GetDeviceSize                                                     *
*    DPSSys_GetDeviceResolution                                               *
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
#include DPSCUSTOMOPS
#include PARAMETERS
#include DEVICE

#include "flushclip.h"  /* USING SetFlushClipProc */
#include "winclip.h"    /* USING [TermDevWinClip] */
/* XYZ: Backdoor into device package, not clean */

#define XWININFO_FROM_DEVICE(device)    \
	( (XWinInfo*) ( (WinInfo*)(device)->priv.ptr )->window )

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "xdevice.h"
#include "xdirect.h"
#include "xwin.h"
#include "xnull.h"
#include "genxsupport.h"
#include "X_server_internals.h"

/*----------------------------*/
/* Include files from X world */
/*----------------------------*/

#define  NEED_EVENTS
#include "scrnintstr.h"	/* USING [_Screen, ScreenInfo]	*/
#include "windowstr.h"	/* USING [_Window]		*/
#include "pixmapstr.h"	/* USING [_Drawable]		*/
#include "resource.h"	/* USING [RC_DRAWABLE]		*/

/*-----------*/
/* constants */
/*-----------*/

#define LAST_DEVICE_ENTRY  nDevTypes - 1

/*-----------------------------------------------------------------*/
/* Section 1: Modify the extern declarations and devInitData array */
/*            below to add a new device implementation to DPS/X.   */
/*                                                                 */
/*             - Do NOT modify the DevInitData typedef.            */
/*                                                                 */
/*             - Add extern declarations for init- and create      */
/*               procedures when adding new devices, or include    */
/*               device implementation's header files.             */
/*                                                                 */
/*             - null device MUST be last in devInitData[]         */
/*                                                                 */
/*-----------------------------------------------------------------*/

typedef int (*XdpsCreateProc)(PDevice * device_ptr_ptr,
    DPSSysClient client, XID drawID, XID gcID, Card32 redmax,
    Card32 greenmax, Card32 bluemax, Card32 graymax, Card32 redmult,
    Card32 greenmult, Card32 bluemult, Card32 graymult,
    Card32 colorbase,Card32 graybase, Card32 actual, Colormap cmap,
    Int16 xOff, Int16 yOff);

/*-------------------------*/
typedef struct _t_DevInitData
{
  PDevProcs (*initProc)ARGDECL0();
  XdpsCreateProc createProc;
} DevInitData;

/*-------------------------*/
DevInitData devInitData[] =
{
  {XDirect_Init, XDirect_Create},   /* direct device */
  {XWin_Init,    XWin_Create   },   /* window device */
  {XNull_Init,   XNull_Create  }    /* null   device */
};

/*---------------------------------------------------------------*/
/* Section 2: No code passed this point need be modified to add  */
/*            a new device impl. You should have added externs   */
/*            and DevInitData entry above, and that is all.      */
/*                                                               */
/*---------------------------------------------------------------*/

/*-----------------*/
/* local variables */
/*-----------------*/

PUBLIC int        nDevTypes;
PUBLIC PDevProcs *procsTable;

/*-------------------*/
/* Public procedures */
/*-------------------*/

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_IsAnXDevice                                                       */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean DPSSys_IsAnXDevice ARGDEF1(PDevice, d)
{
  integer i;
  
  for (i = 0; i < nDevTypes; i++)
    if (d->procs == procsTable[i])
      return(true);

  return(false);
}

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_IsANullXDevice                                                    */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean DPSSys_IsANullXDevice ARGDEF1(PDevice, d)
{
  return (d->procs == procsTable[LAST_DEVICE_ENTRY]);
}

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_CreateDevice                                                      */
/*                                                                           */
/*****************************************************************************/

PUBLIC integer DPSSys_CreateDevice (PDevice      *device_ptr_ptr,
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
  integer i;
  int status;
  DevInitData *cur;

  /*---------------------------------------------------*/
  /* First check to see whether we should be creating  */
  /* a null device. If so, do it and return...         */
  /*---------------------------------------------------*/

  if (drawID == None || gcID == None || graymult == 0)
  {
    /* validate drawID and gcID. No need to validate graymult (not used) */

    if (drawID != None)
    {
       if ((DrawablePtr)LookupIDByClass(drawID, RC_DRAWABLE) == (DrawablePtr)NIL)
          return(BadDrawable);
    }

    if (gcID != None)
    {
       if ((GCPtr)LookupIDByType(gcID, RT_GC) == (GCPtr)NIL)
          return(BadGC);
    }

    /* assumes that last entry in table is null device */

    status = (*devInitData[LAST_DEVICE_ENTRY].createProc)(device_ptr_ptr, client, drawID, gcID,
                                                          redmax,  greenmax,  bluemax,  graymax,
                                                          redmult, greenmult, bluemult, graymult,
                                                          colorbase,
                                                          graybase,
                                                          actual,
                                                          cmap,
                                                          xOff, yOff);

    ( (PDevice)(*device_ptr_ptr) )->procs = procsTable[LAST_DEVICE_ENTRY];

    return (status);
  }

  /*---------------------------------------------*/
  /* Do some generic parameter validation before */
  /* calling any of the device creation procs.   */
  /*---------------------------------------------*/

  status = XDPS_ValidateVisInfo (client, drawID, gcID,
                                 redmax,  greenmax,  bluemax,  graymax,
                                 redmult, greenmult, bluemult, graymult,
                                 colorbase,
                                 graybase,
                                 actual,
                                 cmap);
  if (status != Success)
    return (status);

  /*
   * force backing store on if requested (for debugging)
   */
  {
	register DrawablePtr drawable;
	PSParamValue pval;

	drawable = (DrawablePtr) LookupIDByClass(drawID, RC_DRAWABLE);
	if (drawable->type == DRAWABLE_WINDOW &&
		prm_GetParam("internal", "ForceBackingStore", &pval) &&
		pval.bval) {
		register WindowPtr pWin = (WindowPtr) drawable;

		pWin->backingStore = Always;
		(*drawable->pScreen->ChangeWindowAttributes)(pWin,
			CWBackingStore);
	}
  }

  /*-------------------------------------------------------------*/
  /* Call device creation procedures until one succeeds or       */
  /* returns an error status. If the special CANT_CREATE status  */
  /* is returned, try the next device type. Don't ever call      */
  /* the XNull device proc (last one in the list) - we check for */
  /* an explicit request for a null device at the beginning of   */
  /* this procedure.                                             */
  /*-------------------------------------------------------------*/

  for (cur = devInitData, i = 0; i < LAST_DEVICE_ENTRY; cur++, i++)
  {
     if (cur->createProc)
     {
        status = (*cur->createProc)(device_ptr_ptr, client, drawID, gcID,
                                    redmax,  greenmax,  bluemax,  graymax,
                                    redmult, greenmult, bluemult, graymult,
                                    colorbase,
                                    graybase, 
                                    actual, 
                                    cmap,
                                    xOff, yOff);
        if (status == Success)
        {
           ( (PDevice)(*device_ptr_ptr) )->procs = procsTable[i];
           return (Success);
        }
        else
        if (status != CANT_CREATE)
           return (status);
     }
  }
      
  /*-------------------------------------------------------------*/
  /* None of the device types could be created - assume this is  */
  /* due to some sort of resource allocation failure and return  */
  /* a BadAlloc status                                           */
  /*-------------------------------------------------------------*/

  return (BadAlloc);
}

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_IniDevImpl                                                        */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean DPSSys_IniDevImpl ARGDEF0()
{
  integer i;
  boolean status;
  DCPixelArgs pixelArgs;
  
  InitWinClip();

  nDevTypes = sizeof(devInitData)/sizeof(DevInitData);

  procsTable = (PDevProcs *)sys_calloc(sizeof(PDevProcs), nDevTypes);

  for (i = 0; i < nDevTypes; i++)
      if (devInitData[i].initProc)
         procsTable[i] = (*(devInitData[i].initProc))();

  SetFlushClipProc ( TermDevWinClip );

  /* Set up dummy pixelArgs for AllocImageBuffers */

  os_bzero (&pixelArgs, sizeof(pixelArgs));

  pixelArgs.depth          = 24;
  pixelArgs.firstColor     = 0;
  pixelArgs.dcc            = dccRGB;
  pixelArgs.isPlanar       = false;
  pixelArgs.doConstPattern = false;

  pixelArgs.red.n       = 256;
  pixelArgs.red.first   = 0;
  pixelArgs.red.delta   = 1;

  pixelArgs.green.n     = 256;
  pixelArgs.green.first = 0;
  pixelArgs.green.delta = 1;

  pixelArgs.blue.n      = 256;
  pixelArgs.blue.first  = 0;
  pixelArgs.blue.delta  = 1;

  pixelArgs.gray.n      = 256;
  pixelArgs.gray.first  = 0;
  pixelArgs.gray.delta  = 1;

  status = AllocImageBuffers(&pixelArgs, 1, XDPS_await_pages, NULL);

  return (status);
}

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_CloneDevice                                                       */
/*                                                                           */
/*****************************************************************************/

PUBLIC PDevice DPSSys_CloneDevice ARGDEF0()
{
  PDevice newDev, dev = PSGetDevice((PPSGState) NULL);
  XWinInfo *xWinInfo;
  XDPSVisualInfo *visInfo;

  if (DPSSys_IsAnXDevice(dev)) {
    xWinInfo = XWININFO_FROM_DEVICE(dev);
    visInfo = &xWinInfo->vInfo;
    if (DPSSys_CreateDevice (&newDev,
                             xWinInfo->client,
                             xWinInfo->drawableID,
                             xWinInfo->gcID,
                             visInfo->redMax,
                             visInfo->greenMax,
                             visInfo->blueMax,
                             visInfo->grayMax,
                             visInfo->redMult,
                             visInfo->greenMult,
                             visInfo->blueMult,
                             visInfo->grayMult,
                             visInfo->colorBase,
                             visInfo->grayBase,
                             visInfo->actual,
                             visInfo->colorMap,
                             visInfo->xOffset,
                             visInfo->yOffset ) == Success)
      return newDev;
    else
      return NULL;
  } else {
    /* Current device is nulldevice, so make a new one */
    return (*dev->procs->MakeNullDevice)(dev);
    }
}


/*****************************************************************************/
/*                                                                           */
/*  DPSSys_GetDeviceSize                                                     */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure DPSSys_GetDeviceSize ARGDEF2(integer *,width, integer *,height)
{
  PDevice dev = PSGetDevice((PPSGState) NULL);
  XWinInfo *xWinInfo;

  if (DPSSys_IsAnXDevice(dev) && ! DPSSys_IsANullXDevice(dev)) {
    xWinInfo = XWININFO_FROM_DEVICE(dev);
    *width = xWinInfo->w;
    *height = xWinInfo->h;
  } else {
    /* current device is nulldevice or null X device */
    *width = *height = 0;
    }
}


/*****************************************************************************/
/*                                                                           */
/*  DPSSys_GetDeviceResolution                                               */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure DPSSys_GetDeviceResolution ARGDEF2(integer *,xRes, integer *,yRes)
{
  PDevice dev = PSGetDevice((PPSGState) NULL);
  XWinInfo *xWinInfo;

  if (DPSSys_IsAnXDevice(dev) && ! DPSSys_IsANullXDevice(dev)) {
    xWinInfo = XWININFO_FROM_DEVICE(dev);
    *xRes = xWinInfo->vInfo.xRes;
    *yRes = xWinInfo->vInfo.yRes;
  } else {
    /* current device is nulldevice or null X device */
    *xRes = *yRes = 72;
    }
}
