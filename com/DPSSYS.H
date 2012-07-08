/* @(#)CM_VerSion dpssys.h 3010 1.4 19928.eco sum= 63747 3010.102 */
/* @(#)CM_VerSion dpssys.h 2017 1.2 17334.eco sum= 25009 2017.011 */
/* @(#)CM_VerSion dpssys.h 2016 1.2 13575.eco sum= 57473 */
/* @(#)CM_VerSion dpssys.h 2015 1.4 11286.eco sum= 19871 */
/* @(#)CM_VerSion dpssys.h 2014 1.4 08790.eco sum= 10384 */
/* @(#)CM_VerSion dpssys.h 2013 1.5 08389.eco sum= 28980 */
/*
  dpssys.h

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

#ifndef _DPSSYS_H_
#define _DPSSYS_H_

/*
*******************************************************************************
*                                                                             *
*  File: dpssys.h                                                             *
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
extern procedure DPSSys_SetupSchedExtVal (integer *val_ptr);

		/* Called from PSClientXready() and PSClientSync in xdpsops.c.
                   Sets up the val_flag and val_array in the SchedExt record.
		   PSClientXready must pass a non-NULL pointer.
		   PSClientSync must pass a NULL pointer.  */

/*---------------------------------------------------------------------------*/
extern boolean DPSSys_ValidContext (DPSSysCID dps_ctxid);

		/* Returns 1 if the PostScript interpreter knows about the
		   specified PostScript context id, 0 otherwise. */

/*---------------------------------------------------------------------------*/
typedef void (*XdpsOutputProc)(DPSSysCID   dps_ctxid,
                               char       *buf,
                               integer     count);

extern boolean DPSSys_DestroySpace (DPSSysSID dps_spid, XdpsOutputProc outputProc);

		/* Destroy space identified by sid. Destroys all contexts
		   associated with space. Since other contexts may run when
		   we try to terminate contexts, outputProc is needed to
		   handle output. Returns false on error. */

/*---------------------------------------------------------------------------*/
extern DPSSysSID DPSSys_CreateSpace (void);

		/* Create a new unsecure space, and return its id. A return value of
		   0 indicates that an error occurred. */

/*---------------------------------------------------------------------------*/
extern void DPSSys_CreateContextFromID (DPSSysCID dps_ctxid, char *client);

		/* A forked context is being made to look as though it was
		   created via the Create protocol request. This proc adjusts
		   the lower level data structures to make this seem to be
		   the case. */

/*---------------------------------------------------------------------------*/
extern DPSSysCID DPSSys_CreateContext (char *client, DPSSysSID *Pdps_spid, char *device, boolean secure);

		/* Create a new interpreter context in the space defined by
		   the space id.  If sid == 0, create a new space, and store
		   the new id in *sid. The device parameter is an opaque
		   pointer to a DPS device created by an earlier call to
		   DPSSys_CreateDevice. The client parameter is an opaque
		   designator for a client used in dispatching status events.
		   The secure parameter indicates whether the context should
		   be created in a "secure" (Method 1) space or not.
		   Return the new context id.  A return value of 0 indicates
		   an error occurred. */

/*---------------------------------------------------------------------------*/
extern integer DPSSys_GiveInput (XdpsOutputProc outProc, DPSSysCID dps_ctxid, char *buf, short unsigned int count);

		/* Give input to the context specified by 'cid'. Input
		   consists of 'count' bytes contained in 'buf'. This routine
		   has a side-effect of scheduling all interpreter contexts
		   to run.  If any context has output, outputProc() is
		   called with a buffer of output data. Returns zero if
		   there is an error. */

/*---------------------------------------------------------------------------*/
typedef boolean (*XdpsReturnProc)(void);
extern void DPSSys_RegisterReturnProc (XdpsReturnProc returnControl);

		/* Register the procedure that is called by the kernel to
		   check if the kernel should return control to the window
		   system. This procedure is called when DPSSys_GiveInput
		   is called. */

/*---------------------------------------------------------------------------*/
extern void DPSSys_NotifyContext (DPSSysCID dps_ctxid, int notifyType);

		/* Notify context cid with the specified notification.
		   Values for notifyType defined in XDPS.h */

/*---------------------------------------------------------------------------*/
extern void DPSSys_NotifyWhenReady (XdpsOutputProc outProc, DPSSysCID dps_ctxid, integer *val);

		/* Add notification flag to context's input stream */

/*---------------------------------------------------------------------------*/
extern integer DPSSys_ResetContext (DPSSysCID dps_ctxid, XdpsOutputProc outProc);

		/* Cause the context to reset by placing an EOF on its input
		   stream. Send any output generated as a side effect of this
		   operation to outProc(). */

/*---------------------------------------------------------------------------*/
extern DPSSysSID DPSSys_ContextIDtoSpaceID (DPSSysCID dps_ctxid);

		/* Returns the PostScript Space ID for the context*. */

/*---------------------------------------------------------------------------*/
extern void DPSSys_SetStatusMask (DPSSysCID dps_ctxid, char *client, int set, int unset, int oneShot);
  
		/* Set status interest mask for the specified context on
		   behalf of the specified client. */

/*---------------------------------------------------------------------------*/
extern integer DPSSys_GetStatus (DPSSysCID dps_ctxid);

		/* Get current status of cid. Values of status are defined
		   in XDPS.h */

/*---------------------------------------------------------------------------*/
extern void DPSSys_Restart (void);

		/* Called to allow the lower layers to re-initialize when the
		   server first starts up and whenever it resets. */

/*---------------------------------------------------------------------------*/
extern void DPSSys_Reset (void);

		/* Called to let the lower layers know that a server reset is
		   in progress. These layers should do appropriatecleanup of
		   their own. */

/*---------------------------------------------------------------------------*/
typedef void (*XdpsStatusProc)(ContextID cid, char *client, Card32 status);
typedef void (*XdpsDeathProc)(ContextID cid);
typedef void (*XdpsReadyProc)(ContextID cid, char *client, integer *val); /* integer* is actually integer[4] */

extern integer DPSSys_Init (XdpsStatusProc  statusProc,
                            XdpsDeathProc   deathProc,
                            XdpsReadyProc   readyProc,
                            int            *xDPSReady_ptr);

		/* Initialize DPS. Returns 0 on failure. statusProc is a
		   procedure to call to inform interested clients of the
		   change in status of a context. deathProc is called
		   unconditionally to report the death of a context. 
		   readyProc is called either when a notification flag
		   set by a NotifyWhenReady request on a context's
		   input stream is consumed, or when the clientXready
		   operator is executed.  The four integer values were
		   either specified by the request, or by the operator.
		   readyProc returns 0 if the client has an incompatible
		   protocol, otherwise it returns 1.  */

#endif /* _DPSSYS_H_ */
