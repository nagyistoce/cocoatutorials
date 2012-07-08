/* @(#)CM_VerSion genxsupport.c 3011 1.3 22350.eco sum= 36064 3011.004 */
/* @(#)CM_VerSion genxsupport.c 3010 1.9 20436.eco sum= 36578 3010.103 */
/* @(#)CM_VerSion genxsupport.c 2017 1.10 17841.eco sum= 43199 2017.013 */
/* @(#)CM_VerSion genxsupport.c 2016 1.7 14560.eco sum= 00999 */
/* @(#)CM_VerSion genxsupport.c 2015 1.15 13214.eco sum= 30804 */
/* @(#)CM_VerSion genxsupport.c 2014 1.6 08790.eco sum= 59576 */
/* @(#)CM_VerSion genxsupport.c 2013 1.11 08696.eco sum= 37664 */
/*
  genxsupport.c

(c) Copyright 1993, 1995 Adobe Systems Incorporated.
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
*  File: genxsupport.c                                                        *
*                                                                             *
*******************************************************************************
*                                                                             *
*  This file contains procedures for generic X device support.                *
*                                                                             *
*  - This module contains support routines which may be useful to many        *
*    types of X device implementations. That is, they are not specific        *
*    to any particular device (like the X primitives device).                 *
*    All such pan-galactic procedures begin with the prefix XDPS              *
*                                                                             *
*******************************************************************************
*                                                                             *
*  PRIVATE PROCEDURES:                                                        *
*                                                                             *
*    Drawable table procs                                                     *
*                                                                             *
*      FreeDrawableTbl                                                        *
*      GrowDrawableTbl                                                        *
*      LookupInDrawableTbl                                                    *
*      AddToDrawableTbl                                                       *
*      AddToXWinInfosAtDrawableSlot                                           *
*      ConsistentVisualData                                                   *
*      RemoveGCProc                                                           *
*      ForgetDrawable                                                         *
*      RevertToNullDevice                                                     *
*      HandleNullDrawable                                                     *
*      HandleNullGC                                                           *
*      DrawableGoAway                                                         *
*      GCGoAway                                                               *
*                                                                             *
*    Passthru gc wrappers                                                     *
*                                                                             *
*      WrapValidateGC                                                         *
*      WrapCopyGC                                                             *
*      WrapChangeClip                                                         *
*      WrapCopyClip                                                           *
*      WrapDestroyClip                                                        *
*      WrapChangeGC                                                           *
*                                                                             *
*    Device support procs                                                     *
*                                                                             *
*      GetGravityLoc                                                          *
*      GetDrawParams                                                          *
*      RegisterDrawable                                                       *
*      RegisterGCProc                                                         *
*      UpdateDrawableSize                                                     *
*      DeleteXWinInfo                                                         *
*      GetDeviceOriginFromxWinInfo                                            *
*      GetDeviceOriginFromDevice                                              *
*      ReplaceDevice                                                          *
*      CheckDrawableDepth                                                     *
*                                                                             *
*    Type2 image support procs                                                *
*                                                                             *
*      PackPixels                                                             *
*      DPSCopyArea                                                            *
*      CopySource                                                             *
*      XSampleProc                                                            *
*                                                                             *
*  PUBLIC PROCEDURES:                                                         *
*                                                                             *
*      XDPS_GetMono                                                           *
*      XDPS_AllocColor                                                        *
*      XDPS_Reset                                                             *
*      XDPS_InitSupport                                                       *
*      XDPS_ValidateVisInfo                                                   *
*      XDPS_ConvertToDPSRamp                                                  *
*      XDPS_Setup_xWinInfo                                                    *
*      XDPS_DrawingFunctionChanged                                            *
*      XDPS_GoAway                                                            *
*      XDPS_InitPage                                                          *
*      XDPS_RemakeDevice                                                      *
*      XDPS_SetDevParams                                                      *
*      XDPS_WinToDevTranslation                                               *
*      XDPS_WindowOffset                                                      *
*      XDPS_VisibleRegions                                                    *
*      XDPS_Type2ImageOK                                                      *
*      XDPS_SetupType2Image                                                   *
*      XDPS_CheckForOverlap                                                   *
*      XDPS_BPPMatch                                                          *
*      XDPS_ViewBounds                                                        *
*      XDPS_genxsupport_Reset                                                 *
*      XDPS_await_pages                                                       *
*                                                                             *
*    X customops support routines                                             *
*                                                                             *
*      XDPS_SetXGCDrawable                                                    *
*      XDPS_CurrentXGCDrawable                                                *
*      XDPS_SetXOffset                                                        *
*      XDPS_CurrentXOffset                                                    *
*      XDPS_SetXGCDrawableColor                                               *
*      XDPS_CurrentXGCDrawableColor                                           *
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
#include DEVICE
#include DPSCUSTOMOPS
#include DEVMARKER
#include CANTHAPPEN

#include "framedev.h"
#include "winclip.h"
/* XYZ: Backdoor into device package, not clean */

#define XWININFO_FROM_DEVICE(device)    \
        ( (XWinInfo*) ( (WinInfo*)(device)->priv.ptr )->window )

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "genxsupport.h"
#include "xdevice.h"
#include "xnull.h"
#include "actual.h"
#include "X_server_internals.h"

/*----------------------------*/
/* Include files from X world */
/*----------------------------*/

#define  NEED_EVENTS
#include "X.h"
#include "colormapst.h"	/* need _ColormapRec             */
#include "windowstr.h"  /* need WindowOptPtr             */
#include "servermd.h"	/* need PixmapWidthPaddingInfo[] */
#include "scrnintstr.h"	/* need VisualPtr                */
#include "resource.h"   /* USING [RESTYPE]               */
#include "colormap.h"   /* USING [ColormapPtr, Pixel]    */
#include "colormapst.h" /* USING [_ColormapRec]          */
#include "dixstruct.h"  /* USING [_Client]               */

/*--------------------------*/
/* constants / inline procs */
/*--------------------------*/

#define MASSAGE_DEPTH(d) ((d) == 24 ? 32 : ((d) == 12 ? 16: (d)))

#define FORCE_VALIDATE(xWinInfo)     \
	(xWinInfo)->lastDrawSerial = ~(xWinInfo)->drawable->serialNumber

#define BIT_GRAVITY(xWinInfo)                         \
                                                         \
   ( ( (xWinInfo)->drawable->type == DRAWABLE_WINDOW ) ? \
     ( (WindowPtr)((xWinInfo)->drawable) )->bitGravity : \
     SouthWestGravity                                    \
   )                                                     \

/* The pixel parameter to  AllocColor() is "unsigned long" in R5 and
   "Card32" ("Pixel") in R6. This only makes a difference on 64 bit
    machines (alpahosf1). To make this work on both R5 and R6 we make
    sure that the "pixel" variable in actual.c/XDPS_SetXRGBActual()
    is defined as unsigned long AND that it is initialized to zero.
    XDPS_SetXRGBActual() calls XDPS_AllocColor() passing a pointer to
    the pixel parameter. The pointer is now passed on to "AllocColor()"
    which fills in either 32 or 64 bits.
*/

#ifndef DPS_X11R5
#if (OS == os_osf1)
#define PIXEL_CAST (Card32*)
#endif
#endif

#ifndef PIXEL_CAST
#define PIXEL_CAST
#endif

#ifndef MY_ABS
#define MY_ABS(x) ((x) < 0 ? -(x) : (x))
#endif

/*-------------------------------------------------------------*/
#define DRAWABLE_TBL_GROW_INC   10
/* Number of entries by which to grow the drawableTbl when it
   gets full. this size is arbitrary and should be tuned */

/*-------------------------------------------------------------*/
#define GET_XWININFO(pGC)                                       \
        ( (XWinInfo*) ((pGC)->devPrivates[xdpsGCIndex].ptr) )

/*-------------------------------------------------------------*/
#define WRAPPER_PROLOGUE(pGC)                                   \
        register XWinInfo *xWinInfo = GET_XWININFO(pGC);        \
        (pGC)->funcs = xWinInfo->wrappedGCFuncs;

/*-------------------------------------------------------------*/
#define WRAPPER_EPILOGUE(pGC)                                   \
        xWinInfo->wrappedGCFuncs = (pGC)->funcs;                \
        (pGC)->funcs = &basicGCFuncs;

/*----------*/
/* typedefs */
/*----------*/

/*---------------------------------------*/
typedef struct _t_XWinInfoList
{
  struct _t_XWinInfoList *nextForDrawable;
  XWinInfo *xWinInfo;
} XWinInfoList;

/*---------------------------------------*/
typedef struct _t_DrawableTblEntry
{
  longinteger uid;              /* UID assigned as draw res val */
  XWinInfoList  *xWinInfos;     /* list of xWinInfo ptrs assc'd w/ draw */
} DrawableTblEntry;


/*----------------------*/
/* PRIVATE gloabal vars */
/*----------------------*/

PRIVATE char   *SourceBuffer;       /* buffer used to copy source (for Type2 images) */
PRIVATE Card32  SourceBufferSize;   /* size of buffer */

PRIVATE integer xdpsGCIndex;
  /* This is the index of the ptr in a gc's devPrivates */
  /* array that we use to point to the gc's wrapped     */
  /* funcs vector. No one else should touch this        */
  /* directly.                                          */

PRIVATE RESTYPE xDPSDrawableResType;

PRIVATE procedure WrapValidateGC  (GCPtr pGC, unsigned long changes, DrawablePtr pDrawable);
PRIVATE procedure WrapCopyGC      (GCPtr pGCSrc, unsigned long int mask, GCPtr pGCDst);
PRIVATE procedure WrapChangeClip  (GCPtr pGC, int type, pointer pvalue, int nrects);
PRIVATE procedure WrapCopyClip    (GCPtr pgcDst, GCPtr pgcSrc);
PRIVATE procedure WrapDestroyClip (GCPtr pGC);
PRIVATE procedure WrapChangeGC    (GCPtr gc, unsigned long changes);
PRIVATE procedure GCGoAway        (GCPtr gc);

PRIVATE GCFuncs basicGCFuncs =
  {
  WrapValidateGC,
  WrapChangeGC,
  WrapCopyGC,
  GCGoAway,
  WrapChangeClip,
  WrapDestroyClip,
  WrapCopyClip,
  };

PRIVATE longinteger drawableUid;
PRIVATE int drawableTblSize;
PRIVATE DrawableTblEntry *drawableTbl;

  /* A table of "uid, xWinInfo ptr" associations is used by
     DrawableGoAway to determine whether an xWinInfo structure is
     associated with a drawable when it is about to disappear.
     When a drawable is attached to a device, a unique id is assigned
     to it, and this uid is used as the value of the drawable's
     xDPSDrawableResType resource. When DrawableGoAway is called,
     the drawable's uid is passed. If an entry in the drawableTbl
     contains this uid, the xWinInfo ptr in the entry is valid for
     this drawable, and the necessary cleanup is performed. If no
     such uid is present in the drawableTbl, the drawable no longer
     has a DPS device associated with it. */

/*----------------------*/
/* PRIVATE structures   */
/*----------------------*/

typedef struct _XProcData           /* data used by XSampleProc */
{
  ColormapPtr colormap;
  VisualPtr visual;
  PDevice device;
} XProcData;

/*--------------------*/
/* PRIVATE procedures */
/*--------------------*/

/*----------------------*/
/* Drawable table procs */
/*----------------------*/

/*****************************************************************************/
/*                                                                           */
/*  FreeDrawableTbl                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure FreeDrawableTbl (void)
{
  if (drawableTbl) {
    DrawableTblEntry *ent;

    /* free the xWinInfoList for each registered drawable */
    for (ent = drawableTbl;
         ent < drawableTbl + drawableTblSize;
         ent++) {
      if (ent->uid != 0) {
         XWinInfoList *cur, *next;
         for (cur = ent->xWinInfos;
              cur != (XWinInfoList*)NULL;
              cur = next) {
           next = cur->nextForDrawable;
           sys_free((char*)cur);
        }
      }
    }

    sys_free((char *)drawableTbl);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  GrowDrawableTbl                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure GrowDrawableTbl (void)
{
  DrawableTblEntry *ent;

  drawableTblSize += DRAWABLE_TBL_GROW_INC;
  if (! drawableTbl)
    drawableTbl = (DrawableTblEntry *)sys_malloc(
                    (long int)(sizeof(DrawableTblEntry) * drawableTblSize));
  else
    drawableTbl = (DrawableTblEntry *)sys_realloc((char *)drawableTbl,
                    (long int)(sizeof(DrawableTblEntry) * drawableTblSize));
  if (! drawableTbl)
    CantHappen();

  /* Initialize the new table entries */
  for (ent = drawableTbl + drawableTblSize - DRAWABLE_TBL_GROW_INC;
       ent < drawableTbl + drawableTblSize;
       ent++)
    ent->uid = 0;
  }

/*****************************************************************************/
/*                                                                           */
/*  LookupInDrawableTbl                                                      */
/*                                                                           */
/*****************************************************************************/

/* Returns index of DrawableTblEntry associated with given drawableID,
   or -1 if no such uid exists. */

PRIVATE int LookupInDrawableTbl (XID drawableID)
{
  int i;

  for (i = drawableTblSize - 1; i >= 0; i--)
  {
    /* skip empty slots */
    if (drawableTbl[i].uid == 0) continue;

    DebugAssert(drawableTbl[i].xWinInfos != NULL);
    if (drawableID == drawableTbl[i].xWinInfos->xWinInfo->drawableID) {
      break;
      }
  }
  return(i);
  }

/*****************************************************************************/
/*                                                                           */
/*  AddToDrawableTbl                                                         */
/*                                                                           */
/*****************************************************************************/

PRIVATE longinteger AddToDrawableTbl (XWinInfo *xWinInfo)
{
/* Add an entry for the specified xWinInfo ptr to the drawable table.
   The uid for this entry is generated and returned as the function
   result. This may only be called if the xWinInfo's drawable is not
   currently in the drawable table */

  DrawableTblEntry *ent;
  boolean added = false;

  while (! added) {
    for (ent = drawableTbl + drawableTblSize - 1; ent >= drawableTbl; ent--)
      if (ent->uid == 0) {
        ent->xWinInfos = (XWinInfoList*)sys_malloc(sizeof(XWinInfoList));
        if (ent->xWinInfos == NULL)
            CantHappen();
        ent->xWinInfos->nextForDrawable = NULL;
        ent->xWinInfos->xWinInfo = xWinInfo;
        ent->uid = ++drawableUid;
        if (drawableUid == 0)
          ent->uid = drawableUid = 1;
        added = true;
        break;
        }
    if (! added)
      /* Out of room in table - grow it and try again */
      GrowDrawableTbl();
    }
  return(ent->uid);
}

/*****************************************************************************/
/*                                                                           */
/*  AddToXWinInfosAtDrawableSlot                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure AddToXWinInfosAtDrawableSlot (int i, XWinInfo *xWinInfo)
{
  XWinInfoList *xWinInfoList = (XWinInfoList*)sys_malloc(sizeof(XWinInfoList));
  if (xWinInfoList == NULL)
     CantHappen();
  DebugAssert(drawableTbl[i].uid != 0);
  DebugAssert(drawableTbl[i].xWinInfos != NULL);
  DebugAssert(drawableTbl[i].xWinInfos->xWinInfo->drawableID
              == xWinInfo->drawableID);
  xWinInfoList->xWinInfo = xWinInfo;
  xWinInfoList->nextForDrawable = drawableTbl[i].xWinInfos;
  drawableTbl[i].xWinInfos = xWinInfoList;
}

/*****************************************************************************/
/*                                                                           */
/*  ConsistentVisualData                                                     */
/*                                                                           */
/*****************************************************************************/

PRIVATE int ConsistentVisualData (DrawablePtr drawable, ColormapPtr colormap)
{
  VisualID visual;
  ScreenPtr pScreen;
  int idepth, ivisual, depth;

  visual = colormap->pVisual->vid;
  pScreen = drawable->pScreen;
  depth = drawable->depth;

  for(idepth = 0; idepth < pScreen->numDepths; idepth++)
    {
    register DepthPtr pDepth = (DepthPtr) (&pScreen->allowedDepths[idepth]);

    if ((depth == pDepth->depth) || (depth == 0))
      for (ivisual = 0; ivisual < pDepth->numVids; ivisual++)
        if (visual == pDepth->vids[ivisual])
          return(1);
    }
  return(0);
  }

/*****************************************************************************/
/*                                                                           */
/*  RemoveGCProc                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure RemoveGCProc (XWinInfo *xWinInfo)

  /* This gc is no longer to be associated with DPS. Don't call it's gcProc anymore */
  /* Remove an xWinInfo struct from gc's chain. If removing the */
  /* entry makes the chain empty, remove GC's funcs wrapper.    */

{
  XWinInfo *cur, *prev;
  GCPtr gc = xWinInfo->origGC;

  for (
    cur = (XWinInfo*) (gc->devPrivates[xdpsGCIndex].ptr) ,
    prev = NULL;
    cur != NULL && cur != xWinInfo;
    prev = cur, cur = cur->next);
  if (cur == xWinInfo)
    {
    if (prev)
      prev->next = cur->next;
    else
      gc->devPrivates[xdpsGCIndex].ptr = (pointer)(cur->next);
    }
  else
    CantHappen();

  if (gc->devPrivates[xdpsGCIndex].ptr == NULL)
    gc->funcs = xWinInfo->wrappedGCFuncs;
}

/*****************************************************************************/
/*                                                                           */
/*  ForgetDrawable                                                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure ForgetDrawable (XWinInfo *xWinInfo)
{
  XWinInfoList **xWinInfoListPtr;
  XWinInfoList  *xWinInfoListCur;
  int found = 0;

  /* This drawable is no longer associated with DPS */

  int i;

  if ((i = LookupInDrawableTbl(xWinInfo->drawableID)) < 0)
    goto fatalerror;

  DebugAssert(drawableTbl[i].xWinInfos != NULL);

  /* find and remove from list the xWinInfoList element for this xWinInfo */

  for (xWinInfoListPtr = &drawableTbl[i].xWinInfos,
         xWinInfoListCur = drawableTbl[i].xWinInfos;
       xWinInfoListCur != NULL;
       xWinInfoListPtr = &(xWinInfoListCur->nextForDrawable),
         xWinInfoListCur = xWinInfoListCur->nextForDrawable)
    {
    if (xWinInfoListCur->xWinInfo == xWinInfo) {
      found = 1;
      *xWinInfoListPtr = xWinInfoListCur->nextForDrawable;
      sys_free(xWinInfoListCur);
      break;
      }
    }

  if (!found)
    goto fatalerror;

  /* if this was the last xWinInfo for this drawable, then free
     the drawableTbl slot, too. */

  if (drawableTbl[i].xWinInfos == NULL)
    drawableTbl[i].uid = 0;
  return;

 fatalerror:
    CantHappen();
}

/*****************************************************************************/
/*                                                                           */
/*  RevertToNullDevice                                                       */
/*                                                                           */
/*****************************************************************************/

/* XYZ: maybe this function should be moved to xdevice.c to avoid the externs */

extern int        nDevTypes;  /* in xdevice.c */
extern PDevProcs *procsTable; /* in xdevice.c */

PRIVATE procedure RevertToNullDevice (XWinInfo *xWinInfo)
{
  /* set devProcs to nullProcs */
  xWinInfo->device->procs = procsTable[nDevTypes-1];

  /* reset transformation matrix */
  ( (PFmStuff)(xWinInfo->device) )->gen.matrix.a  =  1.0;
  ( (PFmStuff)(xWinInfo->device) )->gen.matrix.b  =  0.0;
  ( (PFmStuff)(xWinInfo->device) )->gen.matrix.c  =  0.0;
  ( (PFmStuff)(xWinInfo->device) )->gen.matrix.d  = -1.0;
  ( (PFmStuff)(xWinInfo->device) )->gen.matrix.tx =  0.0;
  ( (PFmStuff)(xWinInfo->device) )->gen.matrix.ty =  0.0;
}

/*****************************************************************************/
/*                                                                           */
/*  HandleNullDrawable                                                       */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure HandleNullDrawable (XWinInfo *xWinInfo)
{
  /* A drawable has gone null, handle it by cleaning it up and  */
  /* replacing it with a nice new null device.                  */

  xWinInfo->drawableID = None;
  ForgetDrawable(xWinInfo);
  RevertToNullDevice (xWinInfo);

  /* reset DrawingFunctionChanged callback */
  xWinInfo->funcChg = NULL;
}

/*****************************************************************************/
/*                                                                           */
/*  HandleNullGC                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure HandleNullGC (XWinInfo *xWinInfo)

  /* A gc has gone null, handle it by cleaning it up and        */
  /* replacing it with a nice new null device.                  */
  /* This proc is called when the gc spec'd by the client	*/
  /* goes away. This happens if the client explicitly frees it	*/
  /* or the client goes away. We must perform two tasks:	*/
  /* 1. Let the assoc'd device clean up any resources it has.	*/
  /*    We must do cleanup now since we are going to overwrite	*/
  /*    the device with a null device. Hence when dev cleanup	*/
  /*    is eventually called, we won't know what to cleanup.	*/
  /* 2. We must ensure that no more DPS imaging happens. We do	*/
  /*    this by replacing the exisiting dev with a null X dev.	*/
  /* Note: a single gc can have many devices associated with it	*/
  /* so this procedure must iterate through all xWinInfo's	*/
  /* on the list hanging from the gc that is going away.	*/
  /* For each xWinInfo structure (and corresponding device) on	*/
  /* this list, we can be in 1 of 2 states:			*/
  /* 1. The device is already gone: This happens when a ctxt	*/
  /*    dies and the X resource for the context is freed before	*/
  /*    the X resource for the gc. In this case GCGoAway() is	*/
  /*    not called since we removed the request for gc deletion	*/
  /*    notification when we cleaned up the device.		*/
  /* 2. The normal case: We've got a normal X device that we	*/
  /*    want to turn into a null device. Do this by creating a	*/
  /*    new null device and replacing the existing device.	*/
{
  XWinInfo *nextWinInfo;

  for ( ; xWinInfo != NULL; xWinInfo = nextWinInfo)
  {
     nextWinInfo = xWinInfo->next;
     /* Get next list item before cleaning up current one      */
     xWinInfo->gcID = None;
     RemoveGCProc(xWinInfo);
     RevertToNullDevice (xWinInfo);

     /* reset DrawingFunctionChanged callback */
     xWinInfo->funcChg = NULL;

  }  /* for all xWinInfo structs on gc's list        */
}

/*****************************************************************************/
/*                                                                           */
/*  DrawableGoAway                                                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE int DrawableGoAway (pointer uid, XID xid)
  /* This proc is called when the drawable spec'd by the client */
  /* goes away. This happens if the client explicitly frees it  */
  /* or the client goes away. We must perform two tasks:        */
  /* 1. Let the assoc'd device clean up any resources it has.   */
  /*    We must do cleanup now since we are going to overwrite  */
  /*    the device with a null device. Hence when dev cleanup   */
  /*    is eventually called, we won't know what to cleanup.    */
  /* 2. We must ensure that no more DPS imaging happens. We do  */
  /*    this by replacing the exisiting dev with a null X dev.  */
  /* We can be in 1 of 2 states when this proc is called:       */
  /* 1. The device is already gone: This can happen when a ctxt */
  /*    dies if the X resource for the context is freed before  */
  /*    the X resource for the drawable. In this case we simply */
  /*    return. We detect this by noticing that there is no     */
  /*    longer an assoc between the drawable and an xWinInfo.   */
  /* 2. The normal case: We've got a normal X device that we    */
  /*    want to turn into a null device. Do this by creating a  */
  /*    new null device and replacing the existing device.      */

{
  int i;
  XWinInfo *xWinInfo;

  while ((i = LookupInDrawableTbl(xid )) >= 0)
  {
    /* there is still at lease one xWinInfo associated with this
     * drawable
     */
    DebugAssert(drawableTbl[i].xWinInfos != NULL);
    xWinInfo = drawableTbl[i].xWinInfos->xWinInfo;
    DebugAssert(xid == xWinInfo->drawableID);
    DebugAssert(xWinInfo == XWININFO_FROM_DEVICE(xWinInfo->device));

    HandleNullDrawable(xWinInfo);
  }
  return ( (int) 0 );
}

/*****************************************************************************/
/*                                                                           */
/*  GCGoAway                                                                 */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure GCGoAway (GCPtr gc)
{
  /* xWinInfo declared by WRAPPER_PROLOGUE */
  WRAPPER_PROLOGUE(gc)
  (gc->funcs->DestroyGC)(gc);

  HandleNullGC(xWinInfo);

  /* Can't just execute the WRAPPER_EPILOGUE here; if no more
     DPS devices are associated with this GC, we don't want to
     restore the xdps funcs wrapper. */

  if ((xWinInfo = GET_XWININFO(gc)) != NULL)
  {
    xWinInfo->wrappedGCFuncs = gc->funcs;
    gc->funcs = &basicGCFuncs;
  }
}

/*----------------------*/
/* Passthru gc wrappers */
/*----------------------*/

/*****************************************************************************/
/*                                                                           */
/*  WrapValidateGC                                                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure WrapValidateGC (GCPtr pGC, unsigned long changes, DrawablePtr pDrawable)
{
  WRAPPER_PROLOGUE (pGC);
  (*pGC->funcs->ValidateGC) (pGC, changes, pDrawable);

  /*
   * Must rewrap funcs in all xWinInfos on list, as ValidateGC
   * may have changed them.
   *
   * Note: xWinInfo declared in WRAPPER_PROLOGUE.
   */
  do {
        xWinInfo->wrappedGCFuncs = pGC->funcs;
        xWinInfo = xWinInfo->next;
  } while (xWinInfo != NULL);

  /* Note: cannot use WRAPPER_EPILOGUE here */
  pGC->funcs = &basicGCFuncs;
}

/*****************************************************************************/
/*                                                                           */
/*  WrapCopyGC                                                               */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure WrapCopyGC (GCPtr pGCSrc, long unsigned int mask, GCPtr pGCDst)
{
  WRAPPER_PROLOGUE (pGCDst);
  (*pGCDst->funcs->CopyGC) (pGCSrc, mask, pGCDst);
  WRAPPER_EPILOGUE (pGCDst);
}

/*****************************************************************************/
/*                                                                           */
/*  WrapChangeClip                                                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure WrapChangeClip (GCPtr pGC, int type, pointer pvalue, int nrects)
{
  WRAPPER_PROLOGUE (pGC);
  (*pGC->funcs->ChangeClip) (pGC, type, pvalue, nrects);
  WRAPPER_EPILOGUE (pGC);
}

/*****************************************************************************/
/*                                                                           */
/*  WrapCopyClip                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure WrapCopyClip (GCPtr pgcDst, GCPtr pgcSrc)
{
  WRAPPER_PROLOGUE (pgcDst);
  (* pgcDst->funcs->CopyClip)(pgcDst, pgcSrc);
  WRAPPER_EPILOGUE (pgcDst);
}

/*****************************************************************************/
/*                                                                           */
/*  WrapDestroyClip                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure WrapDestroyClip (GCPtr pGC)
{
  WRAPPER_PROLOGUE (pGC);
  (* pGC->funcs->DestroyClip)(pGC);
  WRAPPER_EPILOGUE (pGC);
}

/*****************************************************************************/
/*                                                                           */
/*  WrapChangeGC                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure WrapChangeGC (GCPtr gc, unsigned long changes)
{
  XWinInfo *nextWinInfo;
  WRAPPER_PROLOGUE(gc)
  (*gc->funcs->ChangeGC)(gc, changes);
  for ( ; xWinInfo != NULL; xWinInfo = nextWinInfo) {
    nextWinInfo = xWinInfo->next;
      /* Get next list item before calling current one's gc proc -
         gc change proc may cause device to be destroyed and recreated,
         thus invalidating the xWinInfo ptr. */
    if (xWinInfo->gcProc)
      (*xWinInfo->gcProc)(xWinInfo, gc, changes);
    }
  xWinInfo = GET_XWININFO(gc);  /* get head of xWinInfo chain again */
  WRAPPER_EPILOGUE(gc)
}

/*----------------------*/
/* Device support procs */
/*----------------------*/

/*****************************************************************************/
/*                                                                           */
/*  GetGravityLoc                                                            */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure GetGravityLoc (int bitGravity, int width, int ht, int *x, int *y)

  /* Given a bit gravity setting and the size of a drawable,	*/
  /* determine the drawable relative point which is the source	*/
  /* of the of the gravity and return the values in *x and *y.	*/
{
  switch (bitGravity) {
    case ForgetGravity:
    case StaticGravity:
    case NorthWestGravity:
      *x = *y = 0;
      break;
    case NorthGravity:
      *x = width/2; *y = 0;
      break;
    case NorthEastGravity:
      *x = width; *y = 0;
      break;
    case WestGravity:
      *x = 0; *y = ht/2;
      break;
    case CenterGravity:
      *x = width/2; *y = ht/2;
      break;
    case EastGravity:
      *x = width; *y = ht/2;
      break;
    case SouthWestGravity: 
      *x = 0; *y = ht;
      break;
    case SouthGravity:
      *x = width/2; *y = ht;
      break;
    case SouthEastGravity:
      *x = width; *y = ht;
      break;
    }
}

/*****************************************************************************/
/*                                                                           */
/*  GetDrawParams                                                            */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure GetDrawParams (XWinInfo *xWinInfo, int *bgX, int *bgY)
{
  /* Given an xWinInfo structure contain a client & drawableID, */
  /* returns x and y coordinates of drawable bit gravity and      */
  /* fills in the xWinInfo struct with the drawable's depth,      */
  /* resolution, width, height, and bitgravity indicator. */

  DrawablePtr drawable_ptr;
  XDPSVisualInfo *vInfo = &xWinInfo->vInfo;

  /*-----------------------------------------------------------*/
  /* The drawable_ptr might be NIL - this may be a null device */
  /*-----------------------------------------------------------*/

  drawable_ptr = (DrawablePtr) LookupIDByClass(xWinInfo->drawableID, RC_DRAWABLE);

  if (drawable_ptr == NIL)
    {
    vInfo->depth = 1;
    vInfo->xRes = vInfo->yRes = 72;
    xWinInfo->w = xWinInfo->h = 0;
    xWinInfo->curBitGravity = SouthWestGravity;
    *bgX = *bgY = 0;
    }
  else
  {
    vInfo->depth = drawable_ptr->bitsPerPixel;
    vInfo->xRes =
      (drawable_ptr->pScreen->width*254)/(drawable_ptr->pScreen->mmWidth*10);
    vInfo->yRes =
      (drawable_ptr->pScreen->height*254)/(drawable_ptr->pScreen->mmHeight*10);
    if (MY_ABS((int) vInfo->xRes - (int) vInfo->yRes) <= 1) {
      /* If x and y resolution differ by 1 pixel or less, force them to
         be the same by setting both to the larger of the two */
      int bigger = MAX(vInfo->xRes, vInfo->yRes);
      vInfo->xRes = vInfo->yRes = bigger;
      }
    xWinInfo->w = drawable_ptr->width;
    xWinInfo->h = drawable_ptr->height;

    if (drawable_ptr->type == DRAWABLE_WINDOW)
       xWinInfo->curBitGravity = ((WindowPtr)drawable_ptr)->bitGravity;
    else
       xWinInfo->curBitGravity = SouthWestGravity;

    GetGravityLoc (xWinInfo->curBitGravity,
                   drawable_ptr->width,
                   drawable_ptr->height,
                   bgX,
                   bgY);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  RegisterDrawable                                                         */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure RegisterDrawable (XWinInfo *xWinInfo)
{
  /* Perform any massaging of the drawable that is required */

  int i;

  if (xWinInfo->drawableID == None)
    return;

  xWinInfo->drawable = (DrawablePtr) LookupIDByClass (xWinInfo->drawableID, RC_DRAWABLE);

  /* We do this AddResource business so that we can be notified */
  /* when the drawable goes away. There is no other hook!       */

  /* It is necessary that this routine never call AddResource() if we are
   * being called indirectly from FreeClientResources() (in the X server).
   * This is because the MIT server is not prepared to handle the case of
   * an AddResource being called during FreeClientResources().
   *
   * This routine may be called (indirectly) from GCGoAway or DrawableGoAway,
   * which is called by FreeClientResources().
   *
   * If we are called (indirectly) by DrawableGoAway, we will create xWinInfo
   * with a 'None' drawableID, so we'll return in the first statement,
   * above.
   *
   * When this routine is called (indirectly) by GCGoAway, it is to register
   * a new xWinInfo having the same drawable as an old xWinInfo.  In this
   * case, this routine is called before calling ForgetDrawable()
   * on the old xWinInfo.  This is important, because it is what guarantees
   * that we will not call AddResource() in this case.  The
   * LookupInDrawableTbl() will succeed because the old xWinInfo is still
   * in the table.  (See HandleNullGC(), which does a DPSSys_CreateDevice()
   * before doing ReplaceDevice() on the old device).
   */
  if ((i = LookupInDrawableTbl(xWinInfo->drawableID)) >= 0) {
      AddToXWinInfosAtDrawableSlot(i, xWinInfo);
    }
  else
  if ( !AddResource( xWinInfo->drawableID, xDPSDrawableResType,
                     (pointer) AddToDrawableTbl(xWinInfo) )
     )
       CantHappen();
}

/*****************************************************************************/
/*                                                                           */
/*  RegisterGCProc                                                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure RegisterGCProc (XWinInfo *xWinInfo, XdpsChangeProc proc)
{
  /* Registers a proc to be called whenever the gc changes */

  XWinInfo *oldXWinInfo;
  GCPtr origGC;

  if (xWinInfo->gcID == None)
    return;

  xWinInfo->gcProc = proc;

  origGC = xWinInfo->origGC = xWinInfo->gc = (GCPtr) LookupIDByType(xWinInfo->gcID, RT_GC);
    /* For the direct access case, the gc we use in our */
    /* everyday work is just the original gc. We can do */
    /* this since we promise not to change it. A window */
    /* device with no frame buffer access will create   */
    /* a shadow gc and point xWinInfo->gc at it.        */

  /* Listen carefully - this is tricky! We must point the new xWinInfo
     at the "old" gc funcs that are wrapped by our "basicGCFuncs".
     There are two cases:
     1) The gc is not yet in use by any DPS contexts. The "old" functions
        to wrap are therefore currently in the gc; pull out the
        funcs ptr and stash it in the xWinInfo struct.
     2) The gc is in use by DPS (determined by whether our devPrivates
        ptr in the gc is non-null). The ptr to functions to wrap have
        already been stashed in all the xWinInfo structs hanging from
        the gc, so pick up the ptr from the first xWinInfo on the chain
        and stash it in the new xWinInfo. We MUST NOT get the funcs
        ptr from the gc in this case, as it has already been replaced
        with a ptr to our basicGCFuncs struct.
     We *may* also have to point the gc at our basicGCFuncs struct and
     this is also tricky. We can be in this procedure either because:
     1) a new device is being created as a direct result of
        a create device request, or setX{mumble} operation that replaces
        the current device.
     2) an existing device is being replaced due to a change in the gc.
        For instance, an "direct" device is being replaced with an Xwin
        device because the plane mask in the gc in changing and we now
        must do all rendering thru X primitives.
     If in the second case, we must not modify the gc->funcs because
     eventually, we'll wind back out the our WrapChangeGC proc which
     will execute the WRAPPER_EPILOGUE. If we've messed with gc->funcs,
     the wrong value for "wrappedGCFuncs" will get stuffed into the
     xWinInfo at the head of the chain. We can't really tell whether
     we in the second case, but we know that if there are already
     xWinInfo's on the gc chain, we don't need to change gc->funcs.
     It will either be set to the correct value by the WRAPPER_EPILOGUE,
     or it is already right (case 1 above). Thus, the only time
     we must update gc->funcs is when the new xWinInfo will be the
     only one on the gc's chain. Whew!
  */

  oldXWinInfo = (XWinInfo*) (origGC->devPrivates[xdpsGCIndex].ptr);
  if (oldXWinInfo == (XWinInfo*)NIL) {
    /* GC not yet in use by DPS - save its current funcs vector,
       point the gc to our wrapper struct */
    xWinInfo->wrappedGCFuncs = origGC->funcs;
    origGC->funcs = &basicGCFuncs;
  } else
    /* Funcs to wrap stored in current xWinInfo struct at list head */
    xWinInfo->wrappedGCFuncs =  oldXWinInfo->wrappedGCFuncs;

  origGC->devPrivates[xdpsGCIndex].ptr = (pointer) xWinInfo;
  xWinInfo->next = oldXWinInfo;
}

/*****************************************************************************/
/*                                                                           */
/*  UpdateDrawableSize                                                       */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure UpdateDrawableSize (XWinInfo *xWinInfo)
{
  /* Update the xWinInfo w and h fields based on the current      */
  /* size of the associated drawable.                             */

  /* This procedure is a no-op for extension side DPS - currently,
     GetDeviceOriginFromxWinInfo always updates the drawable size info in
     the xWinInfo struct. In a future version, we'll hook into
     window size changes and update immediately. */
}

/*****************************************************************************/
/*                                                                           */
/*  DeleteXWinInfo                                                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure DeleteXWinInfo (PDevice device)
{
  XWinInfo *xWinInfo = XWININFO_FROM_DEVICE(device);

  if (xWinInfo->scanline != NIL)
     sys_free (xWinInfo->scanline);

  TermWindow(device);
  if (xWinInfo->gcID != None)
    RemoveGCProc(xWinInfo);
  if (xWinInfo->drawableID != None)
    ForgetDrawable(xWinInfo);
  sys_free( (char*)xWinInfo );
}

/*****************************************************************************/
/*                                                                           */
/*   GetDeviceOriginFromxWinInfo                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure GetDeviceOriginFromxWinInfo (XWinInfo *xWinInfo, DevLong *x, DevLong *y)

  /* This routine is called when we're about to do some DPS     */
  /* marking. It calculates the device origin based on the      */
  /* window's dimensions and bit gravity.                       */

{
  DrawablePtr drawable = xWinInfo->drawable;

  if ((drawable->type == DRAWABLE_WINDOW))
  {
    int w = drawable->width;
    int h = drawable->height;
    register int gravityChanged = 0;
    register int drawableChanged;
    int bitGravity;
    int gx, gy;

    /* The drawable serial number doesn't change the window's bit gravity
       changes, so we track changes by keeping the last known value
       in the xWinInfo structure and comparing it each time this procedure
       is called (sigh). If it changes, we recalculate the gravity-to-device
       offset vector. If the window size has also changed, the device origin
        must be recalculated BEFORE recalculating the gravity-to-device offset
       vector; this vector defines how to drag the origin around when the
       window is resized.

       Applications are supposed to do something following a bit gravity
       change like redraw or do a currentXoffset. This allows us to
       revalidate and update our gravity-to-device offset vector. If
       we don't get a chance to update this vector before the next
       window resize occurs, the origin recalculation will be based on
       the last known bit gravity, and may therefore be incorrect. */

    drawableChanged = xWinInfo->lastDrawSerial != drawable->serialNumber;
    if (xWinInfo->curBitGravity !=
        (bitGravity = ((WindowPtr)drawable)->bitGravity))
      gravityChanged = 1;

    if (gravityChanged || drawableChanged)
      GetGravityLoc( ((WindowPtr)drawable)->bitGravity, w, h, &gx, &gy);

    if (drawableChanged && (xWinInfo->w != w || xWinInfo->h != h))
      /* If the window size has changed, we must recompute the  */
      /* window origin to device origin translation. We do this */
      /* by getting the window relative location of the         */
      /* gravity point (gx, gy) and adding gravity-to-device    */
      /* offset vector (xCornerOffset, yCornerOffset). Result   */
      /* is a vector from window origin to the device origin    */
      /* (vInfo.xOffset, vInfo.yOffset).                        */
    {
      xWinInfo->w = w; xWinInfo->h = h;
      xWinInfo->vInfo.xOffset = gx + xWinInfo->xCornerOffset;
      xWinInfo->vInfo.yOffset = gy + xWinInfo->yCornerOffset;
    }

    if (gravityChanged)
    {
      xWinInfo->curBitGravity = bitGravity;
      xWinInfo->xCornerOffset = xWinInfo->vInfo.xOffset - gx;
      xWinInfo->yCornerOffset = xWinInfo->vInfo.yOffset - gy;
    }
  }

  if ((drawable->type == DRAWABLE_WINDOW) && (xWinInfo->gc->miTranslate))
    /* If true, we must xlat coords for some X drawing prims    */
    /* from window coords to screen coords. We keep the screen  */
    /* to window translation around (xOff, yOff), so we can     */
    /* add it in for the X prims that want window coords.       */
    {
    xWinInfo->xOff = drawable->x;
    xWinInfo->yOff = drawable->y;
    }
  else xWinInfo->xOff = xWinInfo->yOff = 0;

  *x = xWinInfo->vInfo.xOffset;
  *y = xWinInfo->vInfo.yOffset;
}

/* XYZ: temporarily PUBLIC because xdpsmarker.c needs it */
PUBLIC procedure XDPS_GetDeviceOrigin (XWinInfo *xWinInfo, DevLong *x, DevLong *y)
{ 
   GetDeviceOriginFromxWinInfo (xWinInfo, x, y);
}

/*****************************************************************************/
/*                                                                           */
/*  GetDeviceOriginFromDevice                                                */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure GetDeviceOriginFromDevice (PDevice dev, DevLong  *x, DevLong  *y)
  /* Return the device origin in *x, *y; calls procedure to
     validate drawable if device is not null. */
{
  XWinInfo *xWinInfo = XWININFO_FROM_DEVICE(dev);

  if (DPSSys_IsANullXDevice(dev))
  {
     *x = xWinInfo->vInfo.xOffset;
     *y = xWinInfo->vInfo.yOffset;
  } else
     GetDeviceOriginFromxWinInfo(xWinInfo, x, y);
}

/*****************************************************************************/
/*                                                                           */
/*  ReplaceDevice                                                            */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure ReplaceDevice (PDevice oldDevice, PDevice newDevice)

  /* This procedure replaces the contents of an existing device	*/
  /* object with the contents of a new device object. It does	*/
  /* preserve the existing device object's ref count. The new	*/
  /* device object is freed after it has been copied.		*/
  {
  DevShort ref = oldDevice->ref;
  PFmStuff newFmStuff = (PFmStuff) newDevice;
  PFmStuff oldFmStuff = (PFmStuff) oldDevice;

  DeleteXWinInfo(oldDevice);
  if(!XDPS_IsDummyXNullDevice(newDevice))
    XWININFO_FROM_DEVICE(newDevice)->device = oldDevice;
  os_bcopy ( (char *)newDevice, (char *)oldDevice, (long int) sizeof(Device) );
  oldDevice->ref = ref;
  /* Note: not all of the device gets copied because it is a frame device
     (FmStuff). We need to copy the marker, and the image pipeline because the
     old marker and pipeline have been. We can't copy the whole FmStuff because we
     don't want to overwrite the tracking procs
  */
  oldFmStuff->marker              = newFmStuff->marker;
  oldFmStuff->imagePipelineConfig = newFmStuff->imagePipelineConfig;

  /* 2ps_xdps.492.bug */
  if (oldFmStuff->clrntRegistry != NIL)
     (*oldFmStuff->clrntRegistry->freeRegistry)(oldFmStuff->clrntRegistry);

   oldFmStuff->clrntRegistry = newFmStuff->clrntRegistry;

  if(!XDPS_IsDummyXNullDevice(newDevice))
    os_free( (char*) newDevice );
  }

/*****************************************************************************/
/*                                                                           */
/*  CheckDrawableDepth                                                       */
/*                                                                           */
/*****************************************************************************/

PUBLIC integer CheckDrawableDepth (int    depth,
                                   Card32 redmax,
                                   Card32 greenmax,
                                   Card32 bluemax,
                                   Card32 graymax,
                                   Card32 redmult,
                                   Card32 greenmult,
                                   Card32 bluemult,
                                   Card32 graymult,
                                   Card32 colorbase,
                                   Card32 graybase)
{
  register unsignedlonginteger maxrgb, maxgray;

  if (redmult != 0)
    maxrgb = redmax * redmult + greenmax * greenmult +
      bluemax * bluemult + 1 + colorbase;
  else maxrgb = 0;
  maxgray = graymax * graymult + 1 + graybase;
  if (maxgray > maxrgb) maxrgb = maxgray;
  if ( (1 << depth) < maxrgb)
    return(BadMatch);
  else return(Success);
}

/*---------------------------*/
/* Type2 image support procs */
/*---------------------------*/

/*****************************************************************************/
/*                                                                           */
/*  PackPixels                                                               */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure PackPixels (char           *pBuffer,
                              unsigned short  width,
                              unsigned short  height,
                              unsigned char   depth,
                              unsigned char   bpp,
                              ColormapPtr     cmap)
{
  /* Packs pixels into cannonical PostScript image sample format The only
     assumtion that is made about the pixels is that bitsPerPixel is a 
     multiple of 8. */

char *pSrc = pBuffer;
char *pData;
unsigned char bytesPerPixel;
unsigned int numPixels, i, j, endOfLinePad = 0;
unsigned int scanlinePad;
Card32  pixel, bitOffset = 0;
unsigned short column = 0;
unsigned int bpc; 	/* bits per component */
unsigned int componentShift;
unsigned char bytesPerDepth;
Pixel xPixel;
Card32 tmpPixel;
char *pPixel = (char *)&tmpPixel;
xrgb rgbVal;
short class;

	/* bpp has to be 1 or a multiple of 8 */
	if ((bpp % 8) && (depth != 1)) PSTypeCheck();		
	bytesPerPixel = bpp >> 3;
	bytesPerDepth = depth >> 3;
	numPixels = width * height;
	bpc = depth / 3;	/* assume equal sized color components */
	componentShift = 16 - bpc;
	/* scanlinePad is the number of pixels to pad scanline to */
	scanlinePad = PixmapWidthPaddingInfo[depth].padRoundUp + 1;
	if (width % scanlinePad)
		endOfLinePad = (scanlinePad - (width % scanlinePad)) * bytesPerPixel;
	if (cmap)
		class = cmap->pVisual->class;
	else
		/* if we've gone this far without a colormap, must be
		   a bitmap source */
		class = GrayScale;
		
	for (i=0; i<numPixels; i++) {
		/* Copy pixel from buffer to the temporary variable "pixel" */
		switch (class) {
			case DirectColor:
			case TrueColor:
				/* 3 component color */
				/* assign each byte of xPixel individually
				   to prvent misaligned access on some machines
				   (eg. alpha)
				*/
				for(j=0; j < bytesPerDepth; j++)
					pPixel[j] = pSrc[j];
				xPixel = (Pixel)tmpPixel;
				QueryColors(cmap, 1, &xPixel, &rgbVal);
				pixel = rgbVal.red >> componentShift;
				pixel |= (rgbVal.green >> componentShift) << bpc;
				pixel |= (rgbVal.blue >> componentShift) << (bpc << 1);
				break;
			default:
				/* 1 component color */
				pixel = 0L;
				for (j=0; j < bytesPerDepth; j++) 
					pixel |= (Card32)pSrc[j] << (j << 3);
				break;
			}
		/* Pixels that are not multiples of bytes can be non-byte aligned 
			when they are packed. To accomplish this, shift the pixel value 
			to get the correct bit alignment */
		pixel <<= (bitOffset % 8);
		/* set pData to point to the corrisponding byte offset for bitOffset */
		pData = pBuffer + (bitOffset >> 3);
		/* clear the bits we haven't set yet */
		*pData &= (unsigned char) 0xff >> (8 - (bitOffset % 8));
		/* or in the first byte */
		*pData |= (unsigned char)pixel & 0xff;
		pData++;
		pixel >>= 8;
		/* the rest of the bytes can write over existing data */
		for (j=1; j < bytesPerDepth; j++) {
			*pData = (unsigned char)pixel & 0xff;
			pData++;
			pixel >>= 8;
		}
		bitOffset += depth;
		pSrc += bytesPerPixel;
		column++;
		if (column == width) {
			column = 0;
			if (bitOffset % 8) /* need to pad end of row */
				bitOffset += 8 - (bitOffset % 8);
			pSrc += endOfLinePad;
		}
	}
}

/*****************************************************************************/
/*                                                                           */
/*  DPSCopyArea                                                              */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure DPSCopyArea (DrawablePtr srcDrawable,
                               DrawablePtr dstDrawable,
                               GCPtr       gc,
                               integer     srcX,
                               integer     srcY,
                               integer     w,
                               integer     h,
                               integer     dstX,
                               integer     dstY)
{
  RegionPtr pRgn;

  /* clip to srcDrawable */
  if (srcX < 0) {
  	dstX += -srcX;
	w -= -srcX;
    srcX = 0;
  }
  if (srcY < 0) {
    dstY += -srcY;
	h -= -srcY;
  	srcY = 0;
  }
  if ((srcX + w) > (integer)srcDrawable->width) 
    w = srcDrawable->width - srcX;
  if ((srcY + h) > (integer)srcDrawable->height) 
    h = srcDrawable->height - srcY;
  pRgn = (*gc->ops->CopyArea)(
	   srcDrawable, dstDrawable, gc,
           srcX, srcY, w, h, dstX, dstY);
  if (pRgn)
    (*gc->pScreen->RegionDestroy)(pRgn);

} /* DPSCopyArea */

/*****************************************************************************/
/*                                                                           */
/*  CopySource                                                               */
/*                                                                           */
/*****************************************************************************/

PRIVATE char *CopySource (PDevice     device,
                          DevImage   *image,
                          boolean     pack,
                          ColormapPtr cmap)
{
/*
 Copies the source samples into a buffer. This is needed when the source and
 the destination overlap, and CopyArea can't be called. If pack is true, it 
 means that the depth is not the same as the bits per pixel, so the pixels 
 need to be packed.
*/
  XWinInfo *srcXwinfo = XWININFO_FROM_DEVICE(device);
  DevPoint winOffset;
  int srcX, srcY;
  Mtx tMtx;
  real tx, ty;
  long int size;
  DevShort wbytes;
  
	wbytes = (DevShort)PixmapBytePad(image->source->width,
			srcXwinfo->drawable->depth);
	size = wbytes * image->source->height;
	if (SourceBuffer == NULL || size > SourceBufferSize) {
		if (SourceBuffer == NULL)
			SourceBuffer = (char *)sys_malloc(size);
		else
			SourceBuffer = (char *)sys_realloc(SourceBuffer, size);
		if (SourceBuffer == NULL) PSRangeCheck();
		SourceBufferSize = size;
	}
    (*device->procs->WindowOffset)(device, &winOffset);
    srcX = image->info.sourcebounds.x.l - winOffset.x;
    srcY = image->info.sourcebounds.y.l - winOffset.y;
	if (srcX < 0 || srcY < 0) PSRangeCheck();
	(*srcXwinfo->drawable->pScreen->GetImage)(srcXwinfo->drawable,
		srcX, srcY, image->source->width, image->source->height, ZPixmap, 
		(long)-1, SourceBuffer);
	
    /* now fix-up the DevImage structure to use the new source */
    tx = -image->info.sourcebounds.x.l;
    ty = -image->info.sourcebounds.y.l;
    TlatMtx(&tx, &ty, &tMtx);
    MtxCnct(image->info.mtx, &tMtx, image->info.mtx);
    image->info.sourcebounds.x.g -= image->info.sourcebounds.x.l;
    image->info.sourcebounds.y.g -= image->info.sourcebounds.y.l;
    image->info.sourcebounds.x.l = 0;
    image->info.sourcebounds.y.l = 0;
	image->source->wbytes = wbytes;
	if (pack) {
		PackPixels(SourceBuffer, image->source->width,
				image->source->height, srcXwinfo->drawable->depth,
				srcXwinfo->drawable->bitsPerPixel, cmap);
    	image->source->wbytes = ((DevShort)((srcXwinfo->drawable->depth 
			* image->source->width) + 7) >> 3);
	}
	return ( SourceBuffer );
}

/*****************************************************************************/
/*                                                                           */
/*  XSampleProc                                                              */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure XSampleProc ( integer    sample, 
                                void      *result, 
                                DevImage  *image,
                                boolean    cvtColor)
{
DevColor* devColorResult = (DevColor*) result; 
short class, bitsPerRGBValue;
DevInputColor inputColor;
xrgb	rgbVal;
XProcData *pData;
DevPrivate *priv;
ColormapPtr cmap;
integer shift, maxRGBValue;
Pixel xPixel;
/*
  This procedure is called by the devpattern code to convert a sample
  value to a DevInputColor value. In this case the sample is an X pixel
  value, so it uses the X color map to do the conversion. 
*/
  pData = (XProcData *)image->procData;
  class = pData->visual->class;
  bitsPerRGBValue = pData->visual->bitsPerRGBValue;
  cmap = pData->colormap;
  shift = 16 - bitsPerRGBValue;	/* X rgb vals must always be <= 16 bits */
  maxRGBValue = (1 << bitsPerRGBValue) - 1;
  xPixel=sample;
  if(QueryColors(cmap, 1, &xPixel, &rgbVal) != Success) PSTypeCheck();
  switch (class) {
    case PseudoColor:
    case StaticColor:
        inputColor.value.rgb.red   = (real)((integer)rgbVal.red   >> shift)/maxRGBValue;
        inputColor.value.rgb.green = (real)((integer)rgbVal.green >> shift)/maxRGBValue;
        inputColor.value.rgb.blue  = (real)((integer)rgbVal.blue  >> shift)/maxRGBValue;
	break;
    case GrayScale:
    case StaticGray:
        inputColor.value.gray = (real)((integer)rgbVal.red >> shift)/maxRGBValue;
	break;
    default:
        PSTypeCheck();
  }
  priv = (DevPrivate *) PSGetGStateExt((PPSGState) NULL);
  (*pData->device->procs->ConvertColor)(
    pData->device, &image->source->colorSpace, &inputColor,
    image->transfer, image->r, priv, devColorResult);
}

/*-------------------*/
/* Public procedures */
/*-------------------*/

/*****************************************************************************/
/*                                                                           */
/*  XDPS_GetMono                                                             */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_GetMono (XWinInfo *xWinInfo, DevColorData *grayramp)
{

  XDPSVisualInfo *vInfo = &(xWinInfo->vInfo);
  ClientPtr client = xWinInfo->client;
  unsigned short xr, xg, xb;
  unsigned long black, white;
  ColormapPtr cmapPtr;

  if (vInfo->colorMap == (XID)None) return;
  cmapPtr = (ColormapPtr)LookupIDByType(vInfo->colorMap, RT_COLORMAP);
  if (cmapPtr == (ColormapPtr)NIL) return;

  black = 0; /* required because of PIXEL_CAST */
  white = 0; /* required because of PIXEL_CAST */

  xr = 0; xg = 0; xb = 0;
  AllocColor(cmapPtr, &xr, &xg, &xb, PIXEL_CAST &black, client->index);

  xr = 0xffff; xg = 0xffff; xb = 0xffff;
  AllocColor(cmapPtr, &xr, &xg, &xb, PIXEL_CAST &white, client->index);

  grayramp->n = 2;
  grayramp->first = white;
  grayramp->delta = black - white;
  }

/*****************************************************************************/
/*                                                                           */
/*  XDPS_AllocColor                                                          */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean XDPS_AllocColor (XWinInfo *xWinInfo, unsigned short int *r,
                                                    unsigned short int *g,
                                                    unsigned short int *b,
                                                    unsigned long  int *pixel)
{
  ColormapPtr cmapPtr;

  if (xWinInfo->vInfo.colorMap == (XID)None)
    return(false);

  cmapPtr = (ColormapPtr) LookupIDByType(xWinInfo->vInfo.colorMap, RT_COLORMAP);

  if (cmapPtr == (ColormapPtr)NIL)
    return(false);

  *pixel = 0; /* required because of PIXEL_CAST */

  if (AllocColor(cmapPtr, r, g, b, PIXEL_CAST pixel, xWinInfo->client->index) != Success)
    return(false);
  else
    return(true);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_Reset                                                               */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_Reset (void)
{
  FreeDrawableTbl();
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_InitSupport                                                         */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_InitSupport (void)
{
  integer i;

  xDPSDrawableResType = CreateNewResourceType(DrawableGoAway);

  /* Allocate an index into a GC's devPrivate area to hang the
     wrapped func vector from. No need to have CreateGC allocate
     an extra structure - we'll just use gc->devPrivates[xdpsGCIndex].ptr
     to point to the wrapped func vector.

     IMPORTANT:

     If the X-server uses deferred loading of the DPS extension it must
     do the following at the start of each server generation, before
     any graphics contexts have been created:

     1) The server must pre-allocate a private GC index for DPS. This pre-
        allocated GC index must be returned by AllocateGCPrivateIndexForDPS().

     2) The server must also call AllocateGCPrivate() with the pre-allocated
        GC index for each screen.
  */

#if (OS == os_osf1) || (OS == os_irixV) || (OS == os_irix6)
  xdpsGCIndex = AllocateGCPrivateIndexForDPS();
#else
  xdpsGCIndex = AllocateGCPrivateIndex ();

  for (i = 0; i < screenInfo.numScreens; i++)
    AllocateGCPrivate(screenInfo.screens[i], xdpsGCIndex, 0);
#endif

  drawableUid = 0;
  drawableTblSize = 0;
  drawableTbl = NULL;
  GrowDrawableTbl();
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_ValidateVisInfo                                                     */
/*                                                                           */
/*****************************************************************************/

PUBLIC integer XDPS_ValidateVisInfo(DPSSysClient client,
                                    XID          drawID,
                                    XID          gcID,
                                    Card32       redmax,
                                    Card32       greenmax,
                                    Card32       bluemax,
                                    Card32       graymax,
                                    Card32       redmult,
                                    Card32       greenmult,
                                    Card32       bluemult,
                                    Card32       graymult,
                                    Card32       colorbase,
                                    Card32       graybase,
                                    Card32       actual,
                                    XID          cmapID)
{
  integer result;
  ColormapPtr cmapPtr;
  DrawablePtr drawable;
  GCPtr gcPtr;

  /* Check for bad parameter values...                  */
  /* The XID given for the drawable must be valid...    */
  drawable = (DrawablePtr)LookupIDByClass(drawID, RC_DRAWABLE);
  if (drawable == (DrawablePtr)NIL)
     return(BadDrawable);

  result = CheckDrawableDepth (drawable->depth,
                               redmax,
                               greenmax,
                               bluemax,
                               graymax,
                               redmult,
                               greenmult,
                               bluemult,
                               graymult,
                               colorbase,
                               graybase);
  if (result != Success)
    return(result);

  /* The XID given for the gc must be valid...          */
  gcPtr = (GCPtr)LookupIDByType(gcID, RT_GC);
  if (gcPtr == (GCPtr)NIL)
     return(BadGC);

  /* The drawable must in fact be DRAWABLE...           */
  if (drawable->type == UNDRAWABLE_WINDOW)
    return(BadIDChoice);

  /* The gc and drawable must be on the same screen &   */
  /* they must match in depth...                        */
  if (gcPtr->depth != drawable->depth
  || (gcPtr->pScreen != drawable->pScreen))
    return(BadMatch);

  if (cmapID == None)
    {
    /* If no color map was specified, then actual       */
    /* doesn't make sense.                              */
    if (actual != 0)
      return(BadMatch);
    }
  else
    {
    cmapPtr = (ColormapPtr) LookupIDByType(cmapID, RT_COLORMAP);
    if (cmapPtr == (ColormapPtr)NIL)
       return(BadIDChoice);

    if ((drawable->type == DRAWABLE_WINDOW || drawable->type == DRAWABLE_PIXMAP)
    && !ConsistentVisualData(drawable, cmapPtr))
      return(BadMatch);
    /* cmap must match GC and drawable in depth, else   */
    /* BadMatch. cmap must also have the same visual as */
    /* drawable (if drawable is window) else BadMatch.  */
    }

  /* The gray ramp (and color cube if specified) must   */
  /* make sense, or a BadMatch error is returned. This  */
  /* seems like an odd choice but it's the error code   */
  /* that will ultimately be turned into a rangecheck   */
  /* error and that's the PS error we want to raise.    */
  if ((graymax <= 0) ||
      ((redmult > 0 ) &&
       ((redmax <= 0) || (greenmax <= 0) || (bluemax <= 0))))
    return(BadMatch);

  return(Success);
  }

/*****************************************************************************/
/*                                                                           */
/*  XDPS_ConvertToDPSRamp                                                    */
/*                                                                           */
/*****************************************************************************/

PUBLIC  procedure XDPS_ConvertToDPSRamp (Card32 mx, Card32 mult, DevColorData *ramp)
{
  ramp->n     = mx + 1;
  ramp->delta = -mult;
  ramp->first = mx * mult;
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_Setup_xWinInfo                                                      */
/*                                                                           */
/*****************************************************************************/

PUBLIC integer XDPS_Setup_xWinInfo (XWinInfo     **xWinInfoPtr,
                                    DPSSysClient   client,
                                    XID            drawID,
                                    XID            gcID,
                                    Card32         redmax,
                                    Card32         greenmax,
                                    Card32         bluemax,
                                    Card32         graymax,
                                    Card32         redmult,
                                    Card32         greenmult,
                                    Card32         bluemult,
                                    Card32         graymult,
                                    Card32         colorbase,
                                    Card32         graybase,
                                    Card32         actual,
                                    Colormap       cmap,
                                    Int16          xOff,
                                    Int16          yOff)
{
  int x, y;
  XWinInfo *xWinInfo;

  /*---------------------------------------------------*/
  /* Allocate an XWinInfo structure and stash away all */
  /* of the interesting parameters...                  */
  /*---------------------------------------------------*/

  xWinInfo = (XWinInfo *) sys_calloc (1, sizeof(XWinInfo));

  if (xWinInfo == NIL)
     return (CANT_CREATE);

/*xWinInfo->next               = initialized by calloc */
  xWinInfo->client             = client;
  xWinInfo->gcID               = gcID;
  xWinInfo->drawableID         = drawID;

  xWinInfo->vInfo.colorBase    = colorbase;
  xWinInfo->vInfo.redMax       = redmax;
  xWinInfo->vInfo.redMult      = redmult;
  xWinInfo->vInfo.greenMax     = greenmax;
  xWinInfo->vInfo.greenMult    = greenmult;
  xWinInfo->vInfo.blueMax      = bluemax;
  xWinInfo->vInfo.blueMult     = bluemult;
  xWinInfo->vInfo.grayBase     = graybase;
  xWinInfo->vInfo.grayMax      = graymax;
  xWinInfo->vInfo.grayMult     = graymult;
  xWinInfo->vInfo.actual       = actual;
  xWinInfo->vInfo.colorMap     = cmap;
  xWinInfo->vInfo.actualsInUse = 0;
/*xWinInfo->vInfo.actualHash   =  initialized by calloc */
/*xWinInfo->vInfo.depth        =  initialized by GetDrawParams() */
/*xWinInfo->vInfo.xRes         =  initialized by GetDrawParams() */
/*xWinInfo->vInfo.yRes         =  initialized by GetDrawParams() */
  xWinInfo->vInfo.xOffset      = xOff;
  xWinInfo->vInfo.yOffset      = yOff;

  GetDrawParams(xWinInfo, &x, &y);

  xWinInfo->vInfo.depth        = MASSAGE_DEPTH (xWinInfo->vInfo.depth);

  xWinInfo->xCornerOffset      = xWinInfo->vInfo.xOffset - x;
  xWinInfo->yCornerOffset      = xWinInfo->vInfo.yOffset - y;
/*xWinInfo->origGC             = initialized by calloc */
/*xWinInfo->drawable           = initialized by calloc */
/*xWinInfo->xOff               = initialized by calloc */
/*xWinInfo->yOff               = initialized by calloc */
/*xWinInfo->w                  = initialized by GetDrawParams() */
/*xWinInfo->h                  = initialized by GetDrawParams() */
/*xWinInfo->gc                 = initialized by calloc */
/*xWinInfo->wrappedGCFuncs     = initialized by calloc */
/*xWinInfo->maskPixmap         = initialized by calloc */
/*xWinInfo->lastDrawSerial     = initialized by calloc */
/*xWinInfo->paintMode          = initialized by calloc */
/*xWinInfo->curPatID           = initialized by calloc */
/*xWinInfo->curColor           = initialized by calloc */
/*xWinInfo->curOffsetY         = initialized by calloc */
/*xWinInfo->curBitGravity      = initialized by GetDrawParams() */
/*xWinInfo->scanline           = initialized by calloc */
/*xWinInfo->scanlineInUse      = initialized by calloc */
/*xWinInfo->scanSize	       = initialized by calloc */
/*xWinInfo->y                  = initialized by calloc */
/*xWinInfo->xl                 = initialized by calloc */
/*xWinInfo->xg                 = initialized by calloc */
/*xWinInfo->cleanup            = initialized by calloc */
/*xWinInfo->gcProc             = initialized by calloc */
/*xWinInfo->funcChg            = initialized by calloc */
/*xWinInfo->pixmGC             = initialized by calloc */
/*xWinInfo->maskGC             = initialized by calloc */
/*xWinInfo->imageGC            = initialized by calloc */
/*xWinInfo->device             = initialized by calloc */

  *xWinInfoPtr = xWinInfo;

  return(Success);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_DrawingFunctionChanged                                              */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_DrawingFunctionChanged (PDevice       device,
                                              GSDrawingFunc old,
                                              GSDrawingFunc new)
{
  if (old != new)
    {
    XWinInfo *xWinInfo = XWININFO_FROM_DEVICE(device);

    if (xWinInfo->funcChg)
      (*xWinInfo->funcChg)(device, old, new);
    }
  }

/*****************************************************************************/
/*                                                                           */
/*  XDPS_GoAway                                                              */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_GoAway (PDevice device, boolean abortmakingmask)

  /* Here is another case of cleanup complexity due to everyone	*/
  /* wanting to be in control. We must deal with the case	*/
  /* where the client goes away and its context or space XID is	*/
  /* freed before the drawable ID. In that case this proc will	*/
  /* be called before the DrawableGoAway routine. Unfortunately	*/
  /* there is no way of stopping that proc from being called in	*/
  /* the future, or of changing what procedure is called, or of	*/
  /* changing the parameter it is passed (a pointer to this	*/
  /* device's xWinInfo). To avoid making that guy go off the	*/
  /* deep end, we don't free the xWinInfo struct. Instead we	*/
  /* set xWinInfo->deviceto NIL so that the DrawableGoAway code	*/
  /* knows to simply free the xWinInfo structure and return.	*/
  /* NOTE: this proc won't be called if the device is already	*/
  /* null since null X devices have different GoAway procs.	*/
{
  XWinInfo *xWinInfo = XWININFO_FROM_DEVICE(device);

  if (XDPS_IsDummyXNullDevice(device))
        return; /* we never want to free this device */

  if (xWinInfo->cleanup)
    (*xWinInfo->cleanup)(xWinInfo);

  DeleteXWinInfo(device);

  /*---------------------------------------------------------------------*/
  /* Since the device is subclassed from the framedevice call its GoAway */
  /* procedure. The framedevice GoAway procedure will call ps_free to    */
  /* free the device record.                                             */
  /*---------------------------------------------------------------------*/

  (*fmProcs->GoAway) (device, abortmakingmask);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_InitPage                                                            */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_InitPage (PDevice device, DevColor clr)
{
  DevMarkInfo   info;
  DevTrap       trap;
  DevPrim       prim;
  DevLBounds    tbb;
  DevColorSpace dcs;

  os_bzero (&dcs, sizeof(dcs));

  dcs.priv              = NIL;
  dcs.colorSpace        = DEVRGB_COLOR_SPACE;
/*dcs.cie               = not applicable here */
/*dcs.devNInfo          = not applicable here */
/*dcs.createDevNColor   = not applicable here */
/*dcs.addDevNColorRef   = not applicable here */
/*dcs.remDevNColorRef   = not applicable here */

  info.halftone         = NIL;
  info.screenphase.x    = 0;
  info.screenphase.y    = 0;
  info.color            = clr;
  info.colorSpace       = &dcs;
  info.priv.ptr         = PSGetGStateExt(NULL);
  info.overprint        = false;
  info.cachedPattern    = NIL;
  info.transfer         = NIL;
  info.shading          = false;
  info.xfer16Info       = NIL;
  info.shColorHdr       = NIL;

  (*device->procs->DefaultBounds)(device, &tbb);

  trap.y.l  = prim.bounds.y.l = tbb.y.l;
  trap.y.g  = prim.bounds.y.g = tbb.y.g;

  trap.l.xl = trap.l.xg = prim.bounds.x.l = tbb.x.l;
  trap.g.xl = trap.g.xg = prim.bounds.x.g = tbb.x.g;

  prim.type       = trapType;
  prim.next       = NIL;
  prim.offset.x   = 0;
  prim.offset.y   = 0;
  prim.priv.ptr   = NIL;
  prim.items      = 1;
  prim.maxItems   = 1;
  prim.value.trap = &trap;

  (*device->procs->Mark)(device, &prim, NULL, &info);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_RemakeDevice                                                        */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_RemakeDevice (XWinInfo *xWinInfo)
{
  PDevice newDevice, oldDevice;
  XDPSVisualInfo *visInfo = &xWinInfo->vInfo;
  int result;

  oldDevice = xWinInfo->device;
  result = DPSSys_CreateDevice (&newDevice,
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
                                visInfo->yOffset);

    switch (result) {
    case BadDrawable:
    case BadGC:
    case BadIDChoice:
      PSTypeCheck();
      break;
    case BadMatch:
      PSRangeCheck();
      break;
    case CANT_CREATE:
      PSLimitCheck();
      break;
    default:    /* Everything went ok */
      break;
    }

  XDPS_UpdateActualPattern(oldDevice, newDevice);

  ReplaceDevice(oldDevice, newDevice);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_SetDevParams                                                        */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_SetDevParams (XWinInfo        *xWinInfo,
                                    XdpsCleanupProc  cleanup,
                                    XdpsChangeProc   gcProc,
                                    XdpsFuncChgProc  drawingFuncProc)
{
  DevPrim     *winclip;
  DevPoint     origin;
  DevBounds    db;
  DevLBounds  *dlb_ptr;

  xWinInfo->cleanup = cleanup;
  xWinInfo->funcChg = drawingFuncProc;

  origin.x = xWinInfo->vInfo.xOffset;
  origin.y = xWinInfo->vInfo.yOffset;

  /* add device bounding box to the winclip. Need to copy device's bbox because
     AddBoxDevPrim wants a pointer to a "DevBounds" rather than a "DevLBounds".
  */

  dlb_ptr = &((PFmStuff)xWinInfo->device)->gen.bbox;

  db.x.l = dlb_ptr->x.l;
  db.y.l = dlb_ptr->y.l;
  db.x.g = dlb_ptr->x.g;
  db.y.g = dlb_ptr->y.g;

  winclip = AddBoxDevPrim ( InitDevPrim(NewDevPrim(), (DevPrim *)NULL), &db);

  SetWindow (xWinInfo->device, origin, winclip, xWinInfo);

  RegisterDrawable(xWinInfo);
  RegisterGCProc(xWinInfo, gcProc);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_WinToDevTranslation                                                 */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_WinToDevTranslation (PDevice device, DevPoint *translation)
{
  WinInfo *w = (WinInfo*) (device->priv.ptr);
  XWinInfo *xWinInfo = XWININFO_FROM_DEVICE(device);

  GetDeviceOriginFromxWinInfo(xWinInfo, &w->origin.x, &w->origin.y);
  translation->x = -(w->origin.x);
  translation->y = -(w->origin.y);
  }

/*****************************************************************************/
/*                                                                           */
/*  XDPS_WindowOffset                                                        */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_WindowOffset (PDevice device, DevPoint *offset)
{
  WinInfo *w = (WinInfo*) (device->priv.ptr);
  XWinInfo *xWinInfo = XWININFO_FROM_DEVICE(device);

  GetDeviceOriginFromxWinInfo(xWinInfo, &w->origin.x, &w->origin.y);
  offset->x = xWinInfo->xOff;
  offset->y = xWinInfo->yOff;
  }

/*****************************************************************************/
/*                                                                           */
/*  XDPS_VisibleRegions                                                      */
/*                                                                           */
/*****************************************************************************/

PUBLIC DevPrim *XDPS_VisibleRegions (PDevice device)
{
  WinInfo *w = device->priv.ptr;
  return (w->winclip);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_Type2ImageOK                                                        */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean XDPS_Type2ImageOK (PDevice           srcDev,
                                  PDevice           dstDev,
                                  boolean           pixelCopy,
                                  DevColorantClass *dcc)
{

/* Check to see if it's ok to do image. Return the DevColorantClass of the
   source, so the colorspace can be set up for the image.  */

  *dcc = ((PFmStuff)(srcDev))->marker->dcc;
  if(pixelCopy) {
    XWinInfo *srcInfo, *dstInfo;
    srcInfo = XWININFO_FROM_DEVICE(srcDev);
    dstInfo = XWININFO_FROM_DEVICE(dstDev);
    if(srcInfo->drawable->depth != dstInfo->drawable->depth) return (false);
  }
  return (true);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_SetupType2Image                                                     */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_SetupType2Image (PDevice srcDev, DevImage *image, boolean copy)
{
XWinInfo *srcWinInfo = XWININFO_FROM_DEVICE(srcDev);
unsigned depth = (unsigned) srcWinInfo->drawable->depth;
Device *dstDevice = (Device *)PSGetDevice((PPSGState) NULL);
boolean packPixels = false;
Colormap cid;
ColormapPtr cmap = NULL;
WindowOptPtr optional;
short class = 0;

/* Initialize the device/window system specific parts of the DevImage */
  if(srcWinInfo->drawable->type == DRAWABLE_PIXMAP) {
		cid = srcWinInfo->vInfo.colorMap;
		if (cid == (Colormap) NULL) {
			if (depth == 1) {
				cmap = NULL;
				class = GrayScale;
			} else
				PSTypeCheck();
		} else {
  			cmap = (ColormapPtr)LookupIDByType(cid, RT_COLORMAP);
  			if (cmap == NULL) PSTypeCheck();
			class = cmap->pVisual->class;
		}
  } else {
		if(optional = ((WindowPtr)(srcWinInfo->drawable))->optional)
			cid = optional ->colormap;
		else
			cid = srcWinInfo->drawable->pScreen->defColormap;
  		cmap = (ColormapPtr)LookupIDByType(cid, RT_COLORMAP);
  		if (cmap == NULL) PSTypeCheck();
		class = cmap->pVisual->class;
  }

  if(!image->source->pixelCopy) {
    /* if it's not a pixelCopy, we need to set up the sampleProc */
    XProcData *pImageData;
	switch (class) {
		case PseudoColor:
		case StaticColor:
		case GrayScale:
		case StaticGray:
			if ((depth != 1) && (depth != 2) && (depth != 4) 
				&& (depth != 8) && (depth != 12)) PSTypeCheck();
			if (depth == 1 && cid == (Colormap) NULL) {
				image->sampleProc = NULL;
				image->procData = NULL;
			} else {
				image->sampleProc = XSampleProc;
				pImageData = 
					(XProcData *) sys_sureMalloc (sizeof(XProcData));
				pImageData->colormap = cmap;
				pImageData->visual = cmap->pVisual;
				pImageData->device = dstDevice;
				image->procData = (char *) pImageData;
			}
			image->source->nComponents = 1;
  			image->source->bitspersample = depth;
			break;
		case DirectColor:
			copy = true;
		case TrueColor:
			if ((depth != 3) && (depth != 6) && (depth != 12) 
				&& (depth != 24) && (depth != 32)) PSTypeCheck();
			image->sampleProc = NULL;
			image->procData = NULL;
			image->source->nComponents = 3;
  			image->source->bitspersample = depth / 3;
			break;
		default:
			PSTypeCheck();
			break;
	}
	if (depth != srcWinInfo->drawable->bitsPerPixel) {
		copy = true;
		packPixels = true;
	}
#if (!SWAPBITS)
    /* On big endian machines we only need to copy the samples 
	   in case the src and dst overlap, or if pixel packing is required.
	   On little endian machines we always need the copy to get the
	   pixels in a big endian format
	 */
    if(copy)
#endif
		image->source->samples[0] = (unsigned char *) CopySource(
			srcDev, image, packPixels, cmap);
  } else { /* pixelCopy */
    /* PixelCopy means just copy the pixels, possibly with a transformation */
    Mtx *mtx = image->info.mtx;
	float epsilon = 0.0001;
    if((mtx->a < (1.0 + epsilon)) && (mtx->a > (1.0 - epsilon))
		&& (mtx->b < (0.0 + epsilon)) && (mtx->b > (0.0 - epsilon))
		&& (mtx->c < (0.0 + epsilon)) && (mtx->c > (0.0 - epsilon))
		&& (mtx->d < (1.0 + epsilon)) && (mtx->d > (1.0 - epsilon))) {
      /* The matrix has virtually no scale or rotation, so we can use 
	  CopyArea to do the image */
      integer srcX, srcY, w, h, dstX, dstY;
      DevPoint winOffset, winToDev;
      XWinInfo *dstWinInfo = XWININFO_FROM_DEVICE((Device *)dstDevice);
      if (dstWinInfo->gc->serialNumber != dstWinInfo->drawable->serialNumber)
        ValidateGC(dstWinInfo->drawable, dstWinInfo->gc);
      XDPS_WindowOffset(srcDev, &winOffset);
      srcX = image->info.sourcebounds.x.l - winOffset.x;
      srcY = image->info.sourcebounds.y.l - winOffset.y;
      w = image->source->width - 1;
      h = image->source->height - 1;
      XDPS_WinToDevTranslation(dstDevice, &winToDev);
      dstX = image->info.trap->l.xl - winToDev.x;
      dstY = image->info.trap->y.l - winToDev.y;
      DPSCopyArea(srcWinInfo->drawable, dstWinInfo->drawable, dstWinInfo->gc,
          srcX, srcY, w, h, dstX, dstY);
      /* no samples to image - CopyArea has done the image already */
      image->source->samples[0] = NULL; 
    } else {
      /* not using CopyArea, so may need to make our own copy of samples */
	  image->sampleProc = NULL;
	  image->procData = NULL;
	  image->source->nComponents = 1;
	  /* since we're just copying pixels, bitspersample = bitsPerPixel */
	  image->source->bitspersample = srcWinInfo->drawable->bitsPerPixel;
      if(copy) image->source->samples[0] = 
	  	(unsigned char *) CopySource(srcDev, image, packPixels, cmap);
	}
  }
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_CheckForOverlap                                                     */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean XDPS_CheckForOverlap (DevPrim *dp, DevImage *image, PDevice device)
{
DevBounds imageBounds;
DevPoint tlat;
WinInfo *w;
/* Check to see if the source and the destionation overlap */
  w = device->priv.ptr;
  if (w) {
    tlat.x = w->origin.x;
    tlat.y = w->origin.y;
  } else {
    tlat.x = 0;
    tlat.y = 0;
  }
  imageBounds = image->info.sourcebounds;
  /* sourcebounds has window translation in it, dp->bounds doesn't */
  imageBounds.x.l -= tlat.x;
  imageBounds.x.g -= tlat.x;
  imageBounds.y.l -= tlat.y;
  imageBounds.y.g -= tlat.y;
  if(BoundsCompare(&dp->bounds, &imageBounds) != outside)
    return (true);
  else
    return (false);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_BPPMatch                                                            */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean XDPS_BPPMatch (Device *device, integer bpp)
{
  XWinInfo *xWinInfo = XWININFO_FROM_DEVICE(device); 
  return (xWinInfo->drawable->bitsPerPixel == bpp);
 }

/*****************************************************************************/
/*                                                                           */
/*  XDPS_ViewBounds                                                          */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_ViewBounds (PDevice device, DevLBounds *bBox)
{
  WinInfo *w = device->priv.ptr;
  XWinInfo *xWinInfo = XWININFO_FROM_DEVICE(device);
  UpdateDrawableSize(xWinInfo);
  GetDeviceOriginFromxWinInfo(xWinInfo, &w->origin.x, &w->origin.y);

  bBox->x.l = -(w->origin.x);
  bBox->y.l = -(w->origin.y);
  bBox->x.g = bBox->x.l + xWinInfo->w;
  bBox->y.g = bBox->y.l + xWinInfo->h;
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_genxsupport_Reset                                                   */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_genxsupport_Reset (void)
{
	if (SourceBuffer)
		sys_free(SourceBuffer);
	SourceBuffer = NULL;
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_await_pages - Dummy stub function needed by AllocImageBuffers()     */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_await_pages (void *hook)
{
   /* do nothing */
}


/*-------------------------------------------*/
/* Public procedures for X customops support */
/*-------------------------------------------*/

/*****************************************************************************/
/*                                                                           */
/*  XDPS_SetXGCDrawable                                                      */
/*                                                                           */
/*****************************************************************************/

PUBLIC PDevice XDPS_SetXGCDrawable (PDevice  dev,
                                    long int gc,
                                    long int drawable,
                                    long int x,
                                    long int y)
{
  int result;
  PDevice newDevice;
  register XDPSVisualInfo visInfo;

  if (!DPSSys_IsAnXDevice(dev))
    {
    PSUndefined();
    return((PDevice)NIL);
    }

  visInfo = XWININFO_FROM_DEVICE(dev)->vInfo;
    /* Assign this after we know it's an X device       */

  /* If the ref count on the device is not > 1, we can  */
  /* modify the existing device. It's a bit simpler to  */
  /* create a new one, so we do that in all cases.      */
  result = DPSSys_CreateDevice (&newDevice,
                                XWININFO_FROM_DEVICE(dev)->client,
                                drawable,
                                gc,
                                visInfo.redMax,
                                visInfo.greenMax,
                                visInfo.blueMax,
                                visInfo.grayMax,
                                visInfo.redMult,
                                visInfo.greenMult,
                                visInfo.blueMult,
                                visInfo.grayMult,
                                visInfo.colorBase,
                                visInfo.grayBase,
                                visInfo.actual,
                                visInfo.colorMap,
                                (Int16)x,
                                (Int16)y);

    switch (result) {
    case BadDrawable:
    case BadGC:
    case BadIDChoice:
      PSTypeCheck();
      break;
    case BadMatch:
      PSRangeCheck();
      break;
    case CANT_CREATE:
      PSLimitCheck();
      break;
    default:    /* Everything went ok */
      break;
    }

  return(newDevice);
  }

/*****************************************************************************/
/*                                                                           */
/*  XDPS_CurrentXGCDrawable                                                  */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_CurrentXGCDrawable (PDevice   dev,
                                          long int *gc,
                                          long int *drawable,
                                          DevLong  *x,
                                          DevLong  *y)
{
  XWinInfo *xWinInfo;

  if (!DPSSys_IsAnXDevice(dev))
  {
    PSUndefined();
    return;
  }

  xWinInfo = XWININFO_FROM_DEVICE(dev);
    /* Assign this after we know it's an X device       */

  *gc = xWinInfo->gcID;
  *drawable = xWinInfo->drawableID;
  GetDeviceOriginFromDevice(dev, x, y);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_SetXOffset                                                          */
/*                                                                           */
/*****************************************************************************/

PUBLIC PDevice XDPS_SetXOffset (PDevice dev, long int x, long int y)
{
  XWinInfo *xWinInfo;

  if (!DPSSys_IsAnXDevice(dev))
    {
    PSUndefined();
    return((PDevice)NIL);
    }

  xWinInfo = XWININFO_FROM_DEVICE(dev);
    /* Assign this after we know it's an X device       */

  if (dev->ref > 1)     /* Must make a new device       */
    {
    PDevice newDevice = XDPS_SetXGCDrawable(
      dev, xWinInfo->gcID, xWinInfo->drawableID, x, y);
    return(newDevice);
    }
  else
    { 
    int gx, gy;
    xWinInfo->vInfo.xOffset = x;
    xWinInfo->vInfo.yOffset = y;
    if (xWinInfo->drawableID != None)
    {
       GetGravityLoc( BIT_GRAVITY(xWinInfo),
                      xWinInfo->w,
                      xWinInfo->h,
                      &gx,
                      &gy);

       FORCE_VALIDATE(xWinInfo);
         /* Force a full validation before we draw again.. */
    }
    else
       gx = gy = 0;

    xWinInfo->xCornerOffset = xWinInfo->vInfo.xOffset - gx;
    xWinInfo->yCornerOffset = xWinInfo->vInfo.yOffset - gy;

    return(dev);
    }
}  

/*****************************************************************************/
/*                                                                           */
/*  XDPS_CurrentXOffset                                                      */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_CurrentXOffset (PDevice dev, DevLong *x, DevLong *y)
{
  if (!DPSSys_IsAnXDevice(dev))
  {
    PSUndefined();
    return;
  }
  GetDeviceOriginFromDevice(dev, x, y);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_SetXGCDrawableColor                                                 */
/*                                                                           */
/*****************************************************************************/

PUBLIC PDevice XDPS_SetXGCDrawableColor (PDevice            dev,
                                         long int           gc,
                                         long int           drawable,
                                         long int           x,
                                         long int           y,
                /* 12 element array */   long unsigned int *color)
  {
  int result;
  PDevice newDevice;

  if (!DPSSys_IsAnXDevice(dev))
    {
    PSUndefined();
    return((PDevice)NIL);
    }

  /* If the ref count on the device is not > 1, we can  */
  /* modify the existing device. It's a bit simpler to  */
  /* create a new one, so we do that in all cases.      */
  result = DPSSys_CreateDevice (&newDevice,
                                XWININFO_FROM_DEVICE(dev)->client,
                                drawable,
                                gc,
                                color[3],
                                color[5],
                                color[7],
                                color[0],
                                color[4],
                                color[6],
                                color[8],
                                color[1],
                                color[9],
                                color[2],
                                color[11],
                                color[10],
                                (Int16)x,
                                (Int16)y);

    switch (result) {
    case BadDrawable:
    case BadGC:
    case BadIDChoice:
      PSTypeCheck();
      break;
    case BadMatch:
      PSRangeCheck();
      break;
    case CANT_CREATE:
      PSLimitCheck();
      break;
    default:    /* Everything went ok */
      break;
    }

  return(newDevice);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_CurrentXGCDrawableColor                                             */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_CurrentXGCDrawableColor (PDevice            dev,
                                               long int          *gc,
                                               long int          *drawable,
                                               DevLong           *x,
                                               DevLong           *y,
                      /* 12 element array */   long unsigned int *color)
{
  XWinInfo *xWinInfo;

  if (!DPSSys_IsAnXDevice(dev))
    {
    PSUndefined();
    return;
    }

  xWinInfo = XWININFO_FROM_DEVICE(dev);
    /* Assign this after we know it's an X device       */

  *gc = xWinInfo->gcID;
  *drawable = xWinInfo->drawableID;
  GetDeviceOriginFromDevice(dev, x, y);
  color[0] = xWinInfo->vInfo.grayMax;
  color[1] = xWinInfo->vInfo.grayMult;
  color[2] = xWinInfo->vInfo.grayBase;
  color[3] = xWinInfo->vInfo.redMax;
  color[4] = xWinInfo->vInfo.redMult;
  color[5] = xWinInfo->vInfo.greenMax;
  color[6] = xWinInfo->vInfo.greenMult;
  color[7] = xWinInfo->vInfo.blueMax;
  color[8] = xWinInfo->vInfo.blueMult;
  color[9] = xWinInfo->vInfo.colorBase;
  color[10] = xWinInfo->vInfo.colorMap;
  color[11] = xWinInfo->vInfo.actual;
}
