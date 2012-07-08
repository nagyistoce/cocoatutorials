/* @(#)CM_VerSion xdevice.h 3011 1.2 22350.eco sum= 39885 3011.004 */
/* @(#)CM_VerSion xdevice.h 3010 1.3 19928.eco sum= 62198 3010.102 */
/*
  xdevice.h

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

#ifndef _XDEVICE_H_
#define _XDEVICE_H_

/*
*******************************************************************************
*                                                                             *
*  File: xdevice.h                                                            *
*                                                                             *
*******************************************************************************
*/

/*-----------------------------------------*/
/* Public interfaces from PostScript world */
/*-----------------------------------------*/

#include PACKAGE_SPECS
#include ENVIRONMENT
#include PUBLICTYPES

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "xwininfo.h"

/*---------------------------------------------------------------------------*/
extern boolean DPSSys_IsAnXDevice (PDevice d);

		/* Returns true if the device is an X device. */

/*---------------------------------------------------------------------------*/
extern boolean DPSSys_IsANullXDevice (PDevice d);

		/*  Returns true if the device is a null X device. */

/*---------------------------------------------------------------------------*/
extern integer DPSSys_CreateDevice (PDevice      *device_ptr_ptr,
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
                                    Int16         yOff);

		/* Create a device associated with the given gc and drawable
		   and color specifying parameters. The first parameter is a
		   pointer to a place to store the resulting opaque pointer
		   to a DPS device object. This pointer is passed to
		   DPSSysCreateContext when a context is to be created. The
		   return value is an X result. It is Success if all went
		   as planned, or an appropriate error value otherwise. */

/*---------------------------------------------------------------------------*/
extern boolean DPSSys_IniDevImpl (void);

		/* Initialize all the global device tables and structures */
		/* Returns true if successful, otherwise false */

/*---------------------------------------------------------------------------*/
extern PDevice DPSSys_CloneDevice (void);
  /* DPSSys_CloneDevice is called only from the page device implementation
     for Window devices. It creates a new device whose characteristics
     are identical to the current device. A pointer to the new device
     is returned.
   */

/*---------------------------------------------------------------------------*/
extern procedure DPSSys_GetDeviceSize (integer *width, integer *height);
  /* DPSSys_GetDeviceSize is called only from the page device implementation
     for Window devices. It returns the width and height in pixels of 
     the window for the current device. If the current device is the
     null device or is a null X device (X device with null window or
     GC), a width and height of 0 is returned.
   */

/*---------------------------------------------------------------------------*/
extern procedure DPSSys_GetDeviceResolution (integer *xRes, integer *yRes);
  /* DPSSys_GetDeviceResolution is called only from the page device
     implementation for Window devices. It returns the x and y resolution 
     of the window for the current device. If the current device is the
     null device or is a null X device (X device with null window or
     GC), x and y resolutions of 72 dpi are returned.
   */

#endif /* _XDEVICE_H_ */
