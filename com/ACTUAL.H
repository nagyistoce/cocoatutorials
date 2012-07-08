/* @(#)CM_VerSion actual.h 3010 1.4 19928.eco sum= 43573 3010.102 */
/* @(#)CM_VerSion actual.h 2017 1.2 17334.eco sum= 47240 2017.011 */
/* @(#)CM_VerSion actual.h 2015 1.4 11286.eco sum= 41851 */
/* @(#)CM_VerSion actual.h 2014 1.3 08466.eco sum= 58800 */
/* @(#)CM_VerSion actual.h 2013 1.3 07859.eco sum= 24969 */
/*
  actual.h

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

#ifndef _ACTUAL_H_
#define _ACTUAL_H_

/*
*******************************************************************************
*                                                                             *
*  File: actual.h                                                             *
*                                                                             *
*******************************************************************************
*/

/*-----------------------------------------*/
/* Public interfaces from PostScript world */
/*-----------------------------------------*/

#include PACKAGE_SPECS
#include ENVIRONMENT
#include PUBLICTYPES
#include DEVPUBLIC
#include DEVICETYPES

extern boolean XDPS_UpdateActualPattern ARGDECL2 (PDevice, old, PDevice, new);

extern boolean XDPS_SetXRGBActual ARGDECL3 (PDevice        , device,
                                            DevColor      *, devColor,
                                            DevInputColor *, devInputColor);

#endif /* _ACTUAL_H_ */
