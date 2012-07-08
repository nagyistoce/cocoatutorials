/* @(#)CM_VerSion X_server_internals.h 3011 1.2 21945.eco sum= 21485 3011.003 */
/* @(#)CM_VerSion X_server_internals.h 3010 1.5 19928.eco sum= 40681 3010.102 */
/* @(#)CM_VerSion X_server_internals.h 2017 1.2 17334.eco sum= 61366 2017.011 */
/* @(#)CM_VerSion X_server_internals.h 2016 1.4 14560.eco sum= 58254 */
/* @(#)CM_VerSion X_server_internals.h 2015 1.3 09954.eco sum= 59197 */
/* @(#)CM_VerSion X_server_internals.h 2014 1.3 08466.eco sum= 00116 */
/* @(#)CM_VerSion X_server_internals.h 2013 1.2 06671.eco sum= 21146 */
/*
  X_server_internals.h

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

#ifndef _X_SERVER_INTERNALS_H_
#define _X_SERVER_INTERNALS_H_

/*
*******************************************************************************
*                                                                             *
*  File: X_server_internals.h                                                 *
*                                                                             *
*******************************************************************************
*                                                                             *
*  The extern declarations in this file are defined inside the X server.      *
*                                                                             *
*******************************************************************************
*/

/*----------------------------*/
/* Include files from X world */
/*----------------------------*/

#define  NEED_EVENTS
#include "colormap.h"
#include "input.h"
#include "dix.h"
#include "extnsionst.h"

#ifdef DPS_X11R5
#include "Xproto.h"
#include "resource.h"
#endif

/*-----------------------------*/
/* From server/os/connection.c */
/*-----------------------------*/

extern int GrabInProgress; /* index of grabbing client */

/*------------------------*/
/* From server/os/utils.c */
/*------------------------*/

extern Bool CoreDump;

/*----------------------------*/
/* From server/dix/colormap.c */
/*----------------------------*/

extern int AllocColor (ColormapPtr     pmap,
                       unsigned short *pred,
                       unsigned short *pgreen,
                       unsigned short *pblue,
                       Pixel          *pPix,
                       int             client);

/*----------------------------*/
/* From server/dix/privates.c */
/*----------------------------*/

#if (OS == os_osf1) || (OS == os_irixV) || (OS == os_irix6)
extern int AllocateGCPrivateIndexForDPS(void);
#else
extern int AllocateGCPrivateIndex(void);
#endif

/*---------------------*/
/* From server/os/io.c */
/*---------------------*/

extern int WriteToClient (ClientPtr  who,
                          int        count,
                          char      *buf);

/*----------------------------*/
/* From server/dix/dixutils.c */
/*----------------------------*/

extern Bool RegisterBlockAndWakeupHandlers (void    (*blockHandler)(),
                                            void    (*wakeupHandler)(),
                                            pointer blockData);

/*-------------------------------------*/
/* From somewhere else in the X server */
/*-------------------------------------*/

extern void (* ReplySwapVector[])();
extern void (* EventSwapVector[])();

#ifdef DPS_X11R5

/*------------------------*/
/* From server/os/utils.c */
/*------------------------*/

extern unsigned long * Xalloc (unsigned long amount);
extern void            Xfree  (pointer ptr);

#if ( (OS == os_irixV) || (OS == os_irix6) || (OS == os_os_osf1) )
extern void ErrorF     (char *f, ...);
extern void FatalError (char *f, ...);
#endif

/*---------------------------*/
/* From server/dix/swapreq.c */
/*---------------------------*/

extern void SwapLongs  (CARD32 *list, unsigned long count);
extern void SwapShorts (short  *list, unsigned long count);

/*----------------------------*/
/* From server/dix/resource.c */
/*----------------------------*/

typedef int (*DeleteType)();
extern RESTYPE CreateNewResourceType (DeleteType deleteFunc);
extern pointer LookupIDByType        (XID id, RESTYPE rtype);
extern pointer LookupIDByClass       (XID id, RESTYPE classes);
extern Bool    AddResource           (XID id, RESTYPE type, pointer value);

/*----------------------------*/
/* From server/dix/colormap.c */
/*----------------------------*/

extern int QueryColors(ColormapPtr pmap,
                       int         count,
                       Pixel      *ppixIn,
                       xrgb       *prgbList);

/*------------------------*/
/* From server/dix/atom.c */
/*------------------------*/

extern Atom MakeAtom (char *newatom, unsigned len, Bool makeit);

/*----------------------------*/
/* From server/dix/dispatch.c */
/*----------------------------*/

extern int SendErrorToClient (ClientPtr       client,
                              unsigned        majorCode,
                              unsigned short  minorCode,
                              XID             resId,
                              int             errorCode);

/*----------------------*/
/* From server/dix/gc.c */
/*----------------------*/

extern int   CopyGC          (GC *pgcSrc, GC *pgcDst, BITS32 mask);
extern int   FreeGC          (GCPtr pGC, GContext gid);
extern int   ChangeGC        (GC *pGC, BITS32 mask, XID *pval);
extern GCPtr CreateGC        (DrawablePtr pDrawable, BITS32 mask, XID *pval, int *pStatus);
extern void  ValidateGC      (DrawablePtr pDraw, GC *pGC);
extern void  FreeScratchGC   (GCPtr pGC);
extern GCPtr CreateScratchGC (ScreenPtr pScreen, unsigned depth);

/*--------------------------*/
/* From server/dix/events.c */
/*--------------------------*/

extern int TryClientEvents (ClientPtr   client,
                            void       *pEvents,
                     /*+++  xEvent     *pEvents, +++ */
                            int         count,
                            Mask        mask,
                            Mask        filter,
                            GrabPtr     grab);

/*-----------------------------*/
/* From server/dix/extension.c */
/*-----------------------------*/

Bool AddExtensionAlias (char *alias, ExtensionEntry *ext);

ExtensionEntry *AddExtension (char           *name,
                              int            NumEvents,
                              int            NumErrors,
                              int            (* MainProc)(),
                              int            (* SwappedMainProc)(),
                              void           (* CloseDownProc)(),
                              unsigned short (* MinorOpcodeProc)() );

#endif /* DPS_X11R5 */

#endif /* _X_SERVER_INTERNALS_H_ */
