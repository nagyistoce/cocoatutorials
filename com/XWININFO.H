/* @(#)CM_VerSion xwininfo.h 3010 1.3 19928.eco sum= 25372 3010.102 */
/* @(#)CM_VerSion xwininfo.h 2017 1.2 17334.eco sum= 10582 2017.011 */
/* @(#)CM_VerSion xwininfo.h 2016 1.2 13304.eco sum= 60101 */
/* @(#)CM_VerSion xwininfo.h 2015 1.6 13214.eco sum= 53003 */
/* @(#)CM_VerSion xwininfo.h 2014 1.5 08790.eco sum= 09624 */
/* @(#)CM_VerSion xwininfo.h 2013 1.6 08578.eco sum= 40049 */
/*
  xwininfo.h

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

#ifndef _XWININFO_H_
#define _XWININFO_H_

/*
*******************************************************************************
*                                                                             *
*  File: xwininfo.h                                                           *
*                                                                             *
*******************************************************************************
*                                                                             *
*  This file contains the definitions of X data associated with a DPS device  *
*                                                                             *
*******************************************************************************
*/

/*-----------------------------------------*/
/* Public interfaces from PostScript world */
/*-----------------------------------------*/

#include PACKAGE_SPECS
#include ENVIRONMENT
#include PUBLICTYPES
#include FRAMEPIXELTYPES
#include POSTSCRIPT
/* XYZ: POSTSCRIPT causes circularity */
#undef   GC      /* Avoid confusion with Garbage Collector */

/*----------------------------*/
/* Include files from X world */
/*----------------------------*/

#define  NEED_EVENTS
#include "X.h"		/* USING [Colormap, XID]*/
#include "misc.h"	/* USING [pointer]	*/
#include "dix.h"        /* USING [ClientPtr]    */
#include "gc.h"		/* USING [GCPtr]	*/
#include "gcstruct.h"	/* USING [GCFuncs]	*/
#include "pixmap.h"	/* USING [DrawablePtr]	*/

/*-----------*/
/* constants */
/*-----------*/

#define CANT_CREATE -1
  /* Special status code returned by a device type's Create     */
  /* procedure to indicate that it:                             */
  /*   - doesn't support creation of specified device (xdirect  */
  /*     can't create a device using a window drawable if no    */
  /*     frame buffer exists).                                  */
  /*   - can't create the specified device due to a resource    */
  /*     allocation failure.                                    */

/*--------------------------------------------------------------*/
#define GetDrawingFunction(pFunc)       ( ((pFunc) && (*pFunc)) ? (*pFunc) - 1 : GXcopy)
#define SetDrawingFunction(pFunc, func) (*pFunc) = ( (func) + 1 )

/*--------------------------------------------------------------*/
typedef enum {constColor, tiled} PaintMode;

	  /* Indicates whether we're currently drawing with a solid	*/
	  /* color or drawing with a tile (pattern).			*/

/*----------*/
/* typedefs */
/*----------*/

typedef ContextID     DPSSysCID;
typedef ContextID     DPSSysSID;
typedef ClientPtr     DPSSysClient;
typedef PixmapPtr     DPSSysPixmap;
typedef long int      GSDrawingFunc;

struct _t_XWinInfo; /* incomplete type */

typedef void (*XdpsCleanupProc)(struct _t_XWinInfo *xWinInfo);

typedef void (*XdpsChangeProc)(struct _t_XWinInfo  *xWinInfo,
                               GCPtr gc, int changes);

typedef void (*XdpsFuncChgProc)(PDevice device,
                                GSDrawingFunc old,
                                GSDrawingFunc new);

/*--------------------------------------------------------------*/
typedef struct
{
	  /* The following fields define the color cube & gray ramp	*/

  Card32 colorBase;
  Card32 redMax, redMult,  greenMax, greenMult, blueMax, blueMult;
  Card32 grayBase;
  Card32 grayMax, grayMult;

	  /* The following fields define # of exact colors we should	*/
	  /* try and add to the color map, the color map itself, the	*/
	  /* total # of actual colors in use at the moment. And a hash	*/
	  /* table mapping rgb to pixel values.				*/

  Card32         actual;
  Colormap       colorMap;
  unsigned char  actualsInUse;
  Card32         *actualHash;

	  /* depth defines the number of bits/pixel for this drawable	*/

  unsigned short depth;

	  /* xRes, yRes defines the x and y resolution of the drawable	*/

  unsigned short xRes, yRes;

	  /* The following offsets define the vector from the upperLeft	*/
	  /* corner of the window to the device origin.			*/

  Int16 xOffset, yOffset;

} XDPSVisualInfo;

/*--------------------------------------------------------------*/
typedef struct _t_XWinInfo
{
	  /* This structure contains the window system info assoc'd w/	*/
	  /* a context. Some of the info was specified by the client,	*/
	  /* the rest is needed by the impl. This structure is pointed	*/
	  /* to by the "window" field of the WinInfo struct we hang off	*/
	  /* the PRIVATE field of the device object (see winclip.h)	*/
  
  struct _t_XWinInfo *next;

	  /* link to list of xWinInfo structs whose devs share a gc	*/
	  /* The first set of fields corresponds roughly to the info	*/
	  /* spec'd by the client. There is also some computed info.	*/

  DPSSysClient client;	/* The client who owns the device	*/
  XID gcID, drawableID;	/* XID's of client spec'd drawable & gc	*/
  XDPSVisualInfo vInfo;	/* color cube, gray ramp, depth, ...	*/
  GCPtr origGC;		/* GC to be assoc'd with device		*/
  DrawablePtr drawable;	/* Drawable to be assoc'd with device	*/

	  /* The next set of fields has to do with the drawable size	*/
	  /* and location. They are used in adjusting the client	*/
	  /* specified window to device translation in the event of a	*/
	  /* window resize. We need screen->window xlations since some	*/
	  /* of the X drawing prims require screen relative coords.	*/

  int xOff, yOff;	/* Current screen -> window translation	*/
  int xCornerOffset;	/* Offset from corner w/ gravity to...	*/ 
  int yCornerOffset;	/* ...device space origin		*/
  int w, h;		/* Size of drawable last time we looked	*/

	  /* The following fields are used in the imaging process. We	*/
	  /* have a shadow gc which tracks changes to the one spec'd by	*/
	  /* the client. All X imaging is done using this gc so we can	*/
	  /* set the color, tile, and fill mode without having to set &	*/
	  /* restore the values in the original GC.			*/

  GCPtr gc;		/* GC we can manipulate (e.g. set tile)	*/
  GCFuncs *wrappedGCFuncs; /* saved funcs from origGC		*/
  PixmapPtr maskPixmap;	/* Scratch pixmap used to render masks	*/

	    /* This field records the serial number of the drawable the	*/
	    /* last time we used it to mark. When we do our validation	*/
	    /* we check the new drawable serialNumber against this one	*/
	    /* to see if anything in the drawable has changed.		*/
  unsigned long lastDrawSerial;

  PaintMode paintMode;	/* Are we currently painting or tiling	*/
  long int curPatID;	/* Pat id assoc'd w/ installed tile	*/
  SCANTYPE curColor;	/* pat.value assoc'd w/ installed color	*/
  DevLong curOffsetY;	/* Y component of halftonephase	+ origin offset */
  int curBitGravity;	/* bit gravity as of last validation	*/

  /* set if backing store is allowable for the device */
  boolean backingStoreOK;

  /* set if the device can do clipping, e.g. the "xwin" device */
  boolean deviceCanClip;

  /* set if we actually want to use the device clip */
  boolean useDeviceClip;

  /* Note: keep all Boolean values together for (micro-)efficiency */

	  /* The following fields are used to render images. DPS asks	*/
	  /* for a scanline to render into. We supply the data field	*/
	  /* from the pixmap below. The next time DPS asks for a scan-	*/
	  /* line we dump the previous contents and then give back the	*/
	  /* the same scan line data area.				*/

  boolean scanlineInUse;	/* Is scanline full of data	*/
  char *scanline;	/* Pointer to scanline data of pixmap	*/
  integer scanSize;	/* Current size of the scanline buffer. */
  integer y, xl, xg;	/* Location for data in scanline	*/

  XdpsCleanupProc  cleanup;	/* Proc to call when gc or draw go away	*/
  XdpsChangeProc   gcProc;	/* Proc to call when gc changes		*/
  XdpsFuncChgProc  funcChg;	/* Proc to call when drawing func chgs	*/
  GCPtr pixmGC;			/* used to copy things with PutImage    */
  GCPtr maskGC;    		/* use separate GC for masks            */
  GCPtr imageGC;    		/* used by XwinDisplayScanLine()        */

	  /* The device field is a back pointer to the device that this	*/
	  /* structure is associated with.				*/

  PDevice device;

} XWinInfo;

#endif /* _XWININFO_H_ */
