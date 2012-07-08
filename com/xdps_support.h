/* @(#)CM_VerSion xdps_support.h 3011 1.2 21945.eco sum= 32411 3011.003 */
/* @(#)CM_VerSion xdps_support.h 3010 1.4 19928.eco sum= 16140 3010.102 */
/* @(#)CM_VerSion xdps_support.h 2015 1.3 12229.eco sum= 17474 */
/* @(#)CM_VerSion xdps_support.h 2014 1.2 07462.eco sum= 65023 */
/* @(#)CM_VerSion xdps_support.h 2013 1.3 06901.eco sum= 36215 */
/*
  xdps_support.h

(c) Copyright (c) 1992-1994 Adobe Systems Incorporated.
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

#ifndef	XDPS_SUPPORT_H
#define	XDPS_SUPPORT_H

#include PACKAGE_SPECS
#include ENVIRONMENT
#include PUBLICTYPES

/*---------------------------------------------------------------------------*/
#if (OS == os_irixV) || (OS == os_irix6)
extern int       XDPSExtensionInit (void);
#else
extern procedure XDPSExtensionInit (void);
#endif

  /* Called from Xserver only */

/*---------------------------------------------------------------------------*/
extern  procedure XDPSExtensionDeferredInit (void);

  /* Called from Xserver only */

/*---------------------------------------------------------------------------*/
extern boolean XDPSExtensionMaybeInit (void);

  /* Called from the xatm package to initialize the extension only if it
     hasn't yet been.
  */

/*---------------------------------------------------------------------------*/
extern procedure RegisterXDPSPageDevice (void);

  /* output device registration procedure */

#endif	/* XDPS_SUPPORT_H */
