/* @(#)CM_VerSion xdpsext.h 3011 1.2 21945.eco sum= 31116 3011.003 */
/* @(#)CM_VerSion xdpsext.h 3010 1.3 19928.eco sum= 26967 3010.102 */
/*
  xdpsext.h

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

#ifndef _XDPSEXT_H_
#define _XDPSEXT_H_

/*
*******************************************************************************
*                                                                             *
*  File: xdpsext.h                                                            *
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

extern int AnyClientsWithOverflow;

/*---------------------------------------------------------------------------*/
#if (OS == os_irixV) || (OS == os_irix6)
extern int       XDPSExtensionInit (void);
#else
extern procedure XDPSExtensionInit (void);
#endif

/*---------------------------------------------------------------------------*/
extern procedure XDPSExtensionDeferredInit (void);

/*---------------------------------------------------------------------------*/
extern boolean XDPSExtensionMaybeInit (void);

/*---------------------------------------------------------------------------*/
extern procedure XDPSSetOverflow (DPSSysCID dps_ctxid, int val);

#endif /* _XDPSEXT_H_ */
