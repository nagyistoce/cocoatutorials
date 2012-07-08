/* @(#)CM_VerSion genxsupport.h 3011 1.2 22350.eco sum= 32074 3011.004 */
/* @(#)CM_VerSion genxsupport.h 3010 1.4 20436.eco sum= 34451 3010.103 */
/*
  genxsupport.h

(c) Copyright 1993-1994 Adobe Systems Incorporated.
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

#ifndef _GENXSUPPORT_H_
#define _GENXSUPPORT_H_

/*
*******************************************************************************
*                                                                             *
*  File: genxsupport.h                                                        *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Definitions for X Windows support for DPS Device                           *
*                                                                             *
*  - This file is the interface to a collection of routines which             *
*    support many flavors of X device. That is, the routines are not          *
*    specific to a particular X device like the direct frame buffer           *
*    access device.                                                           *
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
#include DEVPUBLIC

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "xwininfo.h"

/*---------------------------------------------------------------------------*/
extern procedure XDPS_GetMono (XWinInfo *xWinInfo, DevColorData *grayramp);

        /* This routine is responsible for finding black and white in   */
        /* the colormap and returning a grayramp that describes them    */

/*---------------------------------------------------------------------------*/
extern boolean XDPS_AllocColor (struct _t_XWinInfo *xWinInfo,
                                         unsigned short int *r,
                                         unsigned short int *g,
                                         unsigned short int *b,
                                         unsigned long  int *pixel);

          /* Try and allocate a color in the colormap. Return 1 if it   */
          /* works (color was allocated), 0 otherwise.                  */

/*---------------------------------------------------------------------------*/
extern procedure XDPS_Reset (void);

          /* Called at server reset time to give this layer a chance to */
          /* clean up any state it wishes to at this time.              */

/*---------------------------------------------------------------------------*/
extern procedure XDPS_InitSupport (void);

          /* Call at init time to get the support routines going        */

/*---------------------------------------------------------------------------*/
extern integer XDPS_ValidateVisInfo(DPSSysClient client,
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
                                    XID          cmapID);

          /* This procedure determines whether or not the paramaters    */
          /* are self consistent. For example, it tests whether the gc  */
          /* and drawable are on the same screen and have equal depth.  */
          /* Returns an X status value indicating what happened.        */

/*---------------------------------------------------------------------------*/
extern procedure XDPS_ConvertToDPSRamp (Card32 mx, Card32 mult, DevColorData *ramp);

/*---------------------------------------------------------------------------*/
extern integer XDPS_Setup_xWinInfo (XWinInfo     **xWinInfoPtr,
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
                                    Int16          yOff);

	  /* Called to create the generic part of an X device.		*/

/*---------------------------------------------------------------------------*/
extern procedure XDPS_DrawingFunctionChanged (PDevice       device,
                                              GSDrawingFunc old,
                                              GSDrawingFunc new);

	  /* This procedure should be called when the drawing function	*/
	  /* is changed. It gives the underlying device a chance to	*/
	  /* respond appropriately.					*/
	
/*---------------------------------------------------------------------------*/
extern procedure XDPS_GoAway (PDevice device, boolean abortmakingmask);

	  /* Performs tasks common to the GoAway proc of many X devs.	*/
	  /* Before doing other cleanup, the "cleanup" proc reg'd in	*/
	  /* XDPS_SetDevParams() is called to do dev specific cleanup.	*/

/*---------------------------------------------------------------------------*/
extern procedure XDPS_InitPage (PDevice device, DevColor clr);

	  /* Perform an InitPage on the specified device                */

/*---------------------------------------------------------------------------*/
extern procedure XDPS_RemakeDevice (struct _t_XWinInfo *xWinInfo);

	  /* This procedure creates a new device that is almost exactly	*/
	  /* like the current one. This is called when some critical	*/
	  /* device parameter changes (like we turn on the planemask).	*/
	  /* This goes thru the device creation path and finds a device	*/
	  /* implementation that can deal with the new parameter.	*/

/*---------------------------------------------------------------------------*/
extern procedure XDPS_SetDevParams (struct _t_XWinInfo *xWinInfo,
                                    XdpsCleanupProc cleanup,
                                    XdpsChangeProc  gcProc,
                                    XdpsFuncChgProc drawingFuncProc);

	  /* A device implementation calls this procedure during device	*/
	  /* creation. It is used to supply device specific information	*/
	  /* that may not be known until after the generic part of dev	*/
	  /* creation is complete. The xWinInfo is the one filled in by	*/
	  /* XDPS_Setup_xWinInfo(). cleanup				*/
	  /* is a device specific procedure called when the device is	*/
	  /* going away. gcProc is called when there is an interesting	*/
	  /* change to the gc. drawingFuncProc is called when there is	*/
	  /* an interesting change to the drawing function.		*/

/*---------------------------------------------------------------------------*/
extern procedure XDPS_WinToDevTranslation (PDevice device, DevPoint *translation);

	  /* Return the translation from the window origin to the DPS	*/
	  /* device space origin.					*/

/*---------------------------------------------------------------------------*/
extern procedure XDPS_WindowOffset (PDevice device, DevPoint *offset);

	  /* Return the offset from the frame buffer origin to the 
	  drawable origin */

/*---------------------------------------------------------------------------*/
extern DevPrim *XDPS_VisibleRegions (PDevice device);

	  /* Returns a DevPrim that represents the "visable region" of the 
	     window, i.e. the window clip */

/*---------------------------------------------------------------------------*/
extern boolean XDPS_Type2ImageOK (PDevice           srcDev,
                                  PDevice           dstDev,
                                  boolean           pixelCopy,
                                  DevColorantClass *dcc);

	  /* Checks to see if it is ok to proceed with the type 2 dictionary
	  form of image */

/*---------------------------------------------------------------------------*/
extern procedure XDPS_SetupType2Image (PDevice srcDev, DevImage *image, boolean copy);

	  /* Initializes the device dependent parts of the image structure */

/*---------------------------------------------------------------------------*/
extern boolean XDPS_CheckForOverlap (DevPrim *dp, DevImage *image, PDevice device);

	  /* Returns true if the dp overlaps the bounds of image */

/*---------------------------------------------------------------------------*/
extern boolean XDPS_BPPMatch (PDevice device, integer bpp); 

 /* Returns true if bpp is the same as the bpp of the drawable in device */

/*---------------------------------------------------------------------------*/
extern procedure XDPS_ViewBounds (PDevice device, DevLBounds *bBox);

	  /* Return the bounding box of the current output device	*/

/*---------------------------------------------------------------------------*/
extern procedure XDPS_genxsupport_Reset (void);

	/* reinitializes PRIVATE data at server reset */

/*---------------------------------------------------------------------------*/
extern procedure XDPS_await_pages (void *hook);

	/* Dummy stub function needed by AllocImageBuffers() */

/*-------------------------------------------*/
/* Public procedures for X customops support */
/*-------------------------------------------*/

/*---------------------------------------------------------------------------*/
extern PDevice XDPS_SetXGCDrawable (PDevice dev,
                                    long int gc,
                                    long int drawable,
                                    long int x,
                                    long int y);

/*---------------------------------------------------------------------------*/
extern procedure XDPS_CurrentXGCDrawable (PDevice dev,
                                          long int *gc,
                                          long int *drawable,
                                          DevLong  *x,
                                          DevLong  *y);

/*---------------------------------------------------------------------------*/
extern PDevice XDPS_SetXOffset (PDevice dev, long int x, long int y);

/*---------------------------------------------------------------------------*/
extern procedure XDPS_CurrentXOffset (PDevice dev, DevLong  *x, DevLong  *y);

/*---------------------------------------------------------------------------*/
extern PDevice XDPS_SetXGCDrawableColor (PDevice dev,
                                         long int gc,
                                         long int drawable,
                                         long int x,
                                         long int y,
                                         long unsigned int *color) ;

/*---------------------------------------------------------------------------*/
extern procedure XDPS_CurrentXGCDrawableColor (PDevice dev,
                                               long int *gc,
                                               long int *drawable,
                                               DevLong  *x,
                                               DevLong  *y,
                                               long unsigned int *color);

#endif /* _GENXSUPPORT_H_ */
