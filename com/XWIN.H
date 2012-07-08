/* @(#)CM_VerSion xwin.h 3010 1.4 19928.eco sum= 53352 3010.102 */
/* @(#)CM_VerSion xwin.h 2017 1.2 17334.eco sum= 51726 2017.011 */
/* @(#)CM_VerSion xwin.h 2016 1.2 13575.eco sum= 04836 */
/* @(#)CM_VerSion xwin.h 2015 1.3 09954.eco sum= 61501 */
/* @(#)CM_VerSion xwin.h 2014 1.2 08466.eco sum= 02046 */
/* @(#)CM_VerSion xwin.h 2013 1.2 06671.eco sum= 10832 */
/*
  xwin.h

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

#ifndef _XWIN_H_
#define _XWIN_H_

/*
*******************************************************************************
*                                                                             *
*  File: xwin.h                                                               *
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

extern PDevProcs XWin_Init (void);

extern int XWin_Create (PDevice      *device_ptr_ptr,
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

#endif /* _XWIN_H_ */
