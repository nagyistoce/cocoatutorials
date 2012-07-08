/* @(#)CM_VerSion xnull.h 3010 1.4 19928.eco sum= 32709 3010.102 */
/* @(#)CM_VerSion xnull.h 2017 1.2 17334.eco sum= 40237 2017.011 */
/* @(#)CM_VerSion xnull.h 2016 1.3 13872.eco sum= 13038 */
/* @(#)CM_VerSion xnull.h 2015 1.3 09954.eco sum= 11206 */
/* @(#)CM_VerSion xnull.h 2014 1.2 08466.eco sum= 37174 */
/* @(#)CM_VerSion xnull.h 2013 1.2 06671.eco sum= 59566 */
/*
  xnull.h

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

#ifndef _XNULL_H_
#define _XNULL_H_

/*
*******************************************************************************
*                                                                             *
*  File: xnull.h                                                              *
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

extern PDevProcs XNull_Init (void);

extern int XNull_Create (PDevice      *device_ptr_ptr,
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

extern boolean XDPS_IsDummyXNullDevice (PDevice device);

#endif /* _XNULL_H_ */
