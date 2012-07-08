/* @(#)CM_VerSion xatmbackdoor.h 3010 1.2 19510.eco sum= 54560 3010.101 */
/*
  X_internals.h

(c) Copyright 1997 Adobe Systems Incorporated.
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

#ifndef _XATMBACKDOOR_H_
#define _XATMBACKDOOR_H_

/*
*******************************************************************************
*                                                                             *
*  File: xatmbackdoor.h                                                       *
*                                                                             *
*******************************************************************************
*                                                                             *
*  extern declarations of sensitive backdoor interfaces into the PostScript   *
*  interpreter. These should be eliminated in a future version.               *
*                                                                             *
*******************************************************************************
*/

/* in vm/gcmisc.c */
extern procedure Ctxt_StopAllCtxts    (void);
extern procedure Ctxt_RestartAllCtxts (void);

/* in postscript/postscriptpriv.h */
extern PSContext CreateContext (PSSpace space,
                                Stm     in,
                                Stm     out,
                                Object  startup);

#endif /* _XATMBACKDOOR_H_ */
