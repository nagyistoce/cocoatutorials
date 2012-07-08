/* @(#)CM_VerSion xdpsext.c 3011 1.3 21945.eco sum= 56413 3011.003 */
/* @(#)CM_VerSion xdpsext.c 3010 1.7 20436.eco sum= 21181 3010.103 */
/* @(#)CM_VerSion xdpsext.c 2017 1.5 16987.eco sum= 36086 2017.010 */
/* @(#)CM_VerSion xdpsext.c 2016 1.6 14560.eco sum= 47231 */
/* @(#)CM_VerSion xdpsext.c 2015 1.9 12669.eco sum= 37005 */
/* @(#)CM_VerSion xdpsext.c 2014 1.6 08466.eco sum= 19040 */
/* @(#)CM_VerSion xdpsext.c 2013 1.9 08124.eco sum= 40413 */
/*
  xdpsext.c

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

/*
*******************************************************************************
*                                                                             *
*  File: xdpsext.c                                                            *
*                                                                             *
*******************************************************************************
*                                                                             *
*  This file contains procedures for initializing the DPS extension, and for  *
*  dispatching the incoming requests.                                         *
*                                                                             *
*******************************************************************************
*                                                                             *
*  PRIVATE PROCEDURES:                                                        *
*                                                                             *
*    Priv_AbortExtension                                                      *
*    Priv_CantHappenProc                                                      *
*    Priv_ExitProc
*    Priv_ReportAbort                                                         *
*    Priv_GenerateStatusReport                                                *
*    Priv_GenerateReadyReport                                                 *
*    Priv_ReportDeath                                                         *
*    Priv_ReportStatusChange                                                  *
*    Priv_ReportReady                                                         *
*    Priv_ReplySwap                                                           *
*    Priv_EventSwap                                                           *
*    Priv_ReadyEventSwap                                                      *
*    Priv_ExtReset                                                            *
*    Priv_FreeContext                                                         *
*    Priv_SendOutputToClient                                                  *
*    Priv_FreeSpace                                                           *
*    Priv_NewContextID                                                        *
*    Priv_NewSpaceID                                                          *
*    Priv_XIDtoContextID                                                      *
*    Priv_XIDtoSpaceID                                                        *
*    Priv_ContextIDtoXID                                                      *
*    Priv_SetOverflow                                                         *
*    Priv_BlockHandler                                                        *
*    Priv_WakeupHandler                                                       *
*    Priv_RegisterBlockAndWakeupHandlers                                      *
*                                                                             *
*    RH_Init                                                                  *
*    RH_CreateContext                                                         *
*    RH_CreateSpace                                                           *
*    RH_GiveInput                                                             *
*    RH_GetStatus                                                             *
*    RH_SetStatusMask                                                         *
*    RH_DestroySpace                                                          *
*    RH_Reset                                                                 *
*    RH_NotifyContext                                                         *
*    RH_CreateContextFromID                                                   *
*    RH_XIDFromContext                                                        *
*    RH_ContextFromXID                                                        *
*    RH_CreateSecureContext                                                   *
*    RH_NotifyWhenReady                                                       *
*    RH_Dispatch                                                              *
*    RH_SwapDispatch                                                          *
*                                                                             *
*    dpsDispatch                                                              *
*    dpsSwapDispatch                                                          *
*    InitializeDisplayPostScript                                              *
*    InitializeExtension                                                      *
*    DeferredInitDispatch                                                     *
*    DeferredInitSwapDispatch                                                 *
*    prepare_for_startup                                                      *
*                                                                             *
*  PUBLIC PROCEDURES:                                                         *
*                                                                             *
*    XDPSExtensionInit         (called from Xserver  only)                    *
*    XDPSExtensionDeferredInit (called from Xserver  only)                    *
*    XDPSExtensionMaybeInit    (called from xatm     only)                    *
*    XDPSSetOverflow           (called from dpssys.c only)                    *
*                                                                             *
*  PUBLIC VARIABLES:                                                          *
*                                                                             *
*    int AnyClientsWithOverflow  (used by dpssys.c)                           *
*                                                                             *
*******************************************************************************
*/

/*-----------------------------------------*/
/* Public interfaces from PostScript world */
/*-----------------------------------------*/

#include PACKAGE_SPECS
#include ENVIRONMENT
#include PUBLICTYPES
#include XDPS_SUPPORT
#include DPSCUSTOMOPS
#include CANTHAPPEN
#include SERVER
#undef   GC      /* Avoid confusion with Garbage Collector */
#undef   SERVER  /* Avoid conflict with Xserver header files */

#include "winclip.h"
/* XYZ: Backdoor into device package, not clean */

#define XWININFO_FROM_DEVICE(device)    \
	( (XWinInfo*) ( (WinInfo*)(device)->priv.ptr )->window )

#ifndef MALLOC_TRACKING
#define MALLOC_TRACKING 0
#endif

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "xdpsext.h"
#include "dpssys.h"
#include "xdevice.h"
#include "X_server_internals.h"

/*----------------------------*/
/* Include files from X world */
/*----------------------------*/

#define  NEED_EVENTS
#include "X.h"
#include "Xproto.h"         /* USING [xEvent]			*/
#include "Xprotostr.h"
#include "os.h"
#include "extnsionst.h"     /* USING [ExtensionEntry]		*/
#include "dix.h"            /* USING [REQUEST]			*/
#include "dixstruct.h"
#include "input.h"
#include "resource.h"       /* USING [RESTYPE]			*/
#include "misc.h"           /* USING [ClientPtr]		*/
#include "DPS/XDPS.h"       /* USING [ContextXID ...]		*/
#include "DPS/XDPSproto.h"  /* USING [PSStatusEvent ...]	*/
#include "pixmapstr.h"
#include "Xos.h"            /* USING [struct timeval]		*/
#undef index
#undef rindex
#include "osdep.h"          /* USING [mskcnt]			*/

/*-----------*/
/* constants */
/*-----------*/

#ifndef DPS_DEF_TOKENTYPE
#if (SWAPBITS == 1)
#define DPS_DEF_TOKENTYPE DPS_LO_IEEE
#else
#define DPS_DEF_TOKENTYPE DPS_HI_IEEE	/* DDX */
#endif
#endif

#ifndef DPS_FORMATNAME
#define DPS_FORMATNAME "IEEE"		/* DDX */
#endif

#define	LastMask		PSFROZENMASK

#define XDPS_OS_INIT_PAGES_SIZE			3*1024*1024
#define XDPS_FIRST_DPS_PROTOCOL_VERSION		DPSPROTO_V08

/*--------*/
/* macros */
/*--------*/

#define	ValidMask(m)		((m) < (LastMask << 1))

#define	XDPSError(errID)	(xdpsg->errorBase + (errID))
#define InternalError(type) \
	FatalError("XDPS Extension: Internal Error %d\n", type);

#define	MarkCtxtXIDGone(entry)	(entry)->xid = None
#define	CtxtXIDIsGone(entry)	((entry)->xid == None)

#define	PSToXID(cid, cur, prev)					\
	for (cur = xdpsg->cxlist, prev = (XDPSIDEntry)NULL;	\
	  cur != (XDPSIDEntry)NULL;				\
	  prev = cur, cur = cur->next)				\
	  if (cur->id.dps_ctxid == cid)	break;			\

/*----------*/
/* typedefs */
/*----------*/

typedef enum
{
  ContextWentAway,
  CouldntSendEvents,
  DestroySpaceFailed,
  OutOfSpace
} InternalErrors;

typedef enum
{
  xdps_context,
  xdps_space
} XDPSRValType;

/*--------------------------------------------------------*/
/* Bind XDPSIDEntry(xdps_context) to context resource XID */
/* Bind XDPSIDEntry(xdps_space)   to space   resource XID */
/*--------------------------------------------------------*/

typedef struct _t_XDPSIDEntry
{
  struct _t_XDPSIDEntry *next;
  ClientPtr              client;
  XID                    xid;
  DPSSysSID              dps_spid;
  XID                    sxid;
  boolean                isZombie;
  XDPSRValType           type;
  union
  {
    DPSSysCID dps_ctxid;
    DPSSysSID dps_spid;
  } id;
  boolean                secure;                 /* L2-DPS/PROTO 9 addition */
  int                    fd;
} XDPSIDEntryRec, *XDPSIDEntry;

typedef int (*DispatchProc)ARGDECL1(ClientPtr, client);

typedef struct
{
  int              reqCode;
  int              eventBase;
  int              errorBase;
  XDPSIDEntry      cxlist;         /* cid <-> xid binding   */
  RESTYPE          ctxtResType;    /* context resource type */
  RESTYPE          spaceResType;   /* space   resource type */
  int              version[MAXCLIENTS];     /* from X:server/include/misc.h */
  boolean          extensionAborted;        /* L2-DPS/PROTO 9 addition */
  boolean          extensionInited;
  DispatchProc     dispatch;
  DispatchProc     swapDispatch;
} XDPSGlobalsRec, *XDPSGlobals;

/*-----------------*/
/* local variables */
/*-----------------*/

PRIVATE XDPSGlobals    xdpsg;
PRIVATE XDPSGlobalsRec xDPSGlobalsStruct;

/*
 * Code to support DPS scheduling based on X11R5 block and wakeup handlers
 *
 * Note: In previous versions of DPS, this was coded in-line in
 * WaitForSomething().  If your X server doesn't support block and wakeup
 * handlers, or for some other reason you want to use the older code, you
 * need to:
 * - define DPS_USE_SERVERCLIENT
 * - use the Adobe modified versions of
 *   server/os/io.c and server/os/WaitFor.c, or the equivalent.
 */

/*
 * The flag xDPSReady is set by the DPS scheduler when DPS yields back
 * to X and there are still runnable contexts (when xDPSReady is set it
 * indicates that there are runnable DPS contexts).
 */
PRIVATE int xDPSReady;

/*
 * These variables are used to implement DPS flow control.  When DPS notices
 * a context that is getting way behind on input, it marks the client for
 * that context as overflowed.  We will suppress that client's bit in the X
 * server's select mask so no more data will be read from it until the
 * context has caught up.
 */

PRIVATE long ClientsWithOverflow[mskcnt];
PUBLIC  int  AnyClientsWithOverflow;

/*--------------------*/
/* PRIVATE procedures */
/*--------------------*/

/*****************************************************************************/
/*                                                                           */
/*  Priv_AbortExtension                                                      */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_AbortExtension ARGDEF0()
{
  xdpsg->extensionAborted = true;
  xDPSReady = 0;
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_CantHappenProc                                                      */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_CantHappenProc (Card32 errID)
{
#if STAGE == DEVELOP
	ErrorF("\nXDPS Extension CantHappen\n");
	return;
#endif

	if (CoreDump) {
		FatalError("XDPS Extension Error ID  0x%x", errID);
	}
	else {
		ErrorF("\nXDPS Extension Error ID 0x%x; extension disabled\n",
			errID);
		Priv_AbortExtension();
		PSYield(yield_other, (char *) NULL);
	}
}

PRIVATE procedure
Priv_ExitProc (void)
{
	FatalError("XDPS Extension Error");
}


/*****************************************************************************/
/*                                                                           */
/*  Priv_ReportAbort                                                         */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_ReportAbort (ClientPtr client)
{
  register XDPSIDEntry idEntry;
  ClientPtr cls[MAXCLIENTS];
  int i, last = 0;

  if (client == (ClientPtr)NULL)
  {
     for (idEntry = xdpsg->cxlist; idEntry != (XDPSIDEntry)NULL; idEntry = idEntry->next)
     {
        for (i = 0; i < last; ++i)
        {
           if (idEntry->client == cls[i])
              goto duped;
         }
         cls[last++] = idEntry->client;
duped:
         ;
      }
   }
   else
   {
      last = 1;
      cls[0] = client;
   }

   for (i = 0; i < last; ++i)
      if (cls[i] != serverClient)
         SendErrorToClient(cls[i], xdpsg->reqCode, 0, 0, XDPSError(PSERRORABORT));
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_GenerateStatusReport                                                */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_GenerateStatusReport (ClientPtr client, ContextXID cxid, int status)
{
    PSStatusEvent event;
    PSStatusEvent *pe = &event;
    int sts1;

    pe->type = xdpsg->eventBase + PSEVENTSTATUS;
    pe->status = (BYTE)status;
    pe->sequenceNumber = client->sequence;
    pe->cxid = cxid;
    sts1 = TryClientEvents( (ClientPtr)client, (xEvent *)pe, (int)1,
                            (Mask)0, (Mask)NoEventMask, (GrabPtr)NullGrab );
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_GenerateReadyReport                                                 */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_GenerateReadyReport (ClientPtr client, ContextXID cxid, integer *val)
{
    PSReadyEvent event;
    register PSReadyEvent *pe = &event;
    int sts1;

    pe->type = xdpsg->eventBase + PSEVENTREADY;
    pe->sequenceNumber = client->sequence;
    pe->cxid = cxid;
    pe->val1 = val[0];
    pe->val2 = val[1];
    pe->val3 = val[2];
    pe->val4 = val[3];
    sts1 = TryClientEvents( (ClientPtr)client, (xEvent *)pe, (int)1,
                            (Mask)0, (Mask)NoEventMask, (GrabPtr)NullGrab );
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_ReportDeath                                                         */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_ReportDeath (ContextID cid)
{
  XDPSIDEntry idEntry, prevIDEntry;

  PSToXID(cid, idEntry, prevIDEntry);
  if (idEntry == (XDPSIDEntry) NULL)
    return;	/* This is a forked context, no resource for it */

  if (CtxtXIDIsGone(idEntry))
  {
    if (prevIDEntry == (XDPSIDEntry)NULL) xdpsg->cxlist = idEntry->next;
    else prevIDEntry->next = idEntry->next;
    xfree(idEntry);
    return;
  }
  else idEntry->isZombie = true;
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_ReportStatusChange                                                  */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_ReportStatusChange (ContextID cid, char *opqClient, Card32 newStatus)
{
    XDPSIDEntry idEntry, prevIDEntry;

    PSToXID(cid, idEntry, prevIDEntry);
    if (idEntry == (XDPSIDEntry) NULL)
      InternalError(ContextWentAway);

    if (CtxtXIDIsGone(idEntry)) return;
    Priv_GenerateStatusReport((ClientPtr)opqClient, idEntry->xid, newStatus);
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_ReportReady (L2-DPS/PROTO 9 "readyproc" addition)                   */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_ReportReady (ContextID cid, char *opqClient, integer *val)
{
    XDPSIDEntry idEntry, prevIDEntry;
    ClientPtr client = (ClientPtr)opqClient;


    if (xdpsg->version[client->index] < DPSPROTO_V09)
        return; /*+++ return(0); +++*/

    PSToXID(cid, idEntry, prevIDEntry);
    if (idEntry == (XDPSIDEntry) NULL)
      InternalError(ContextWentAway);

    if (CtxtXIDIsGone(idEntry)) return; /*+++ return(1); +++*/
    Priv_GenerateReadyReport((ClientPtr)opqClient, idEntry->xid, val);
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_ReplySwap                                                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_ReplySwap (ClientPtr client, int size, char *pReply)
  /* Swap proc for replys. All short and long fields are byte	*/
  /* swapped 'in place' in the original reply buffer, which is	*/
  /* then forwarded to the client via WriteToClient. All types	*/
  /* of replies have the sequenceNumber and length fields in 	*/
  /* same place, so these fields are swapped regardless of	*/
  /* reply type. 						*/
{
#define REPLY(type) \
    register type *reply = (type *)pReply
    register char tmp;

    { /* Swap generic part of header */
    REPLY(xPSGetStatusReply);
    swaps(&reply->sequenceNumber, tmp);
    swapl(&reply->length, tmp);
    }

    switch (((xReq *)client->requestBuffer)->data) {
      case X_PSInit: {
	REPLY(xPSInitReply);
	SwapLongs(&reply->serverversion, 3);
	break;
	}
      case X_PSCreateContext: {
	REPLY(xPSCreateContextReply);
	swapl(&reply->cpsid, tmp);
	break;
	}
      case X_PSCreateContextFromID: {
	REPLY(xPSCreateContextFromIDReply);
	swapl(&reply->sxid, tmp);
	break;
	}
      case X_PSXIDFromContext: {
	REPLY(xPSXIDFromContextReply);
	swapl(&reply->cxid, tmp);
	swapl(&reply->sxid, tmp);
	break;
	}
      case X_PSContextFromXID: {
	REPLY(xPSContextFromXIDReply);
	swapl(&reply->cpsid, tmp);
	break;
	}
      /* L2-DPS/PROTO 9 addition */
      case X_PSCreateSecureContext: {
	REPLY(xPSCreateContextReply);    /* Use same struct at CreateContext */
	swapl(&reply->cpsid, tmp);
	break;
	}
      default:
	;
      }

    WriteToClient(client, size, pReply);
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_EventSwap                                                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_EventSwap (PSOutputEvent *pOrig, PSOutputEvent *pSwapped)
  /* Swap proc for events being delivered to client.  Normally,    */
  /* each type of event would have its own swap proc. The format   */
  /* of the message for PSEVENTOUTPUT and PSEVENTSTATUS events     */
  /* is identical with respect to the fields that must be swapped, */
  /* so a single procedure is used for both.                       */
{
    register char tmp;

    (void) os_bcopy(pOrig, pSwapped, sizeof(PSOutputEvent));
    swaps(&pSwapped->sequenceNumber, tmp);
    swapl(&pSwapped->cxid, tmp);
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_ReadyEventSwap (L2-DPS/PROTO 9 addition)                            */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_ReadyEventSwap (PSReadyEvent *pOrig, PSReadyEvent *pSwapped)
{
    register char tmp;

    (void) os_bcopy(pOrig, pSwapped, sizeof(PSReadyEvent));
    swaps(&pSwapped->sequenceNumber, tmp);
    SwapLongs(&(pSwapped->cxid), 5);
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_ExtReset                                                            */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_ExtReset (ExtensionEntry *extension)
{
  /* Assert: all clients are gone by the time this is called.	*/
  /* we could check this by seeing that all of our lists are	*/
  /* empty. We don't need to free any list items here as they	*/
  /* should have gone away as part of clients going away.	*/
  if (!xdpsg->extensionAborted && xdpsg->extensionInited)
    DPSSys_Reset();

#if (MALLOC_TRACKING == 1)
  sys_mallocFreeAll();
#endif

}

/*****************************************************************************/
/*                                                                           */
/*  Priv_FreeContext                                                         */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
Priv_FreeContext (pointer value, XID xid)
  /* Delete proc for context resource. This proc is called as a	*/
  /* side effect of a resource being freed. A context resource	*/
  /* is freed by X when a client goes away. When this proc is	*/
  /* called, the assoc'd context might be running, or it may	*/
  /* have already terminated (making it a zombie until the XID	*/
  /* is deallocated). If it is still running, we simply mark	*/
  /* the context's XDPSIDEntry indicating that the XID has been	*/
  /* deallocated. If the context is a zombie when we get here,	*/
  /* we free the XDPSIDEntry, which is the ultimate disposal of	*/
  /* a context.							*/
{
  XDPSIDEntry prevIDEntry;
  XDPSIDEntry idEntry = (XDPSIDEntry)value;

  /* fix for 2ps_xdps.321.bug */
  BITCLEAR(ClientsWithOverflow, idEntry->fd);
  AnyClientsWithOverflow = ANYSET(ClientsWithOverflow);

  if (idEntry->isZombie)
    {
    DPSSysCID dps_ctxid = idEntry->id.dps_ctxid;

    PSToXID(dps_ctxid, idEntry, prevIDEntry);
      /* PSToXID is a macro, can't use idEntry->id.dps_ctxid since it's	*/
      /* "passed" by name and will conflict with use of idEntry	*/
    if (prevIDEntry == (XDPSIDEntry)NULL) xdpsg->cxlist = idEntry->next;
    else prevIDEntry->next = idEntry->next;
    xfree(idEntry);
    return ( (int) 0 );
    }
  MarkCtxtXIDGone((XDPSIDEntry)value);
  return ( (int) 0 );
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_SendOutputToClient                                                  */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_SendOutputToClient (DPSSysCID dps_ctxid, char *buf, integer count)
{
    register PSOutputEvent *pe;
    register char *bb = buf;
    register unsigned ct = (unsigned)count;
    register ClientPtr client;
    BYTE eType = xdpsg->eventBase + PSEVENTOUTPUT;
    unsigned len = BYTESPEROUTPUTEVENT;
    PSOutputEvent *pEvents;
    unsigned nEvents;
    XDPSIDEntry t, prev;
    XID xid;
    int sts1;

    PSToXID(dps_ctxid, t, prev);
    if (t == (XDPSIDEntry) NULL) return;
      /* This is a forked context, no resource for it	*/

    if (CtxtXIDIsGone(t)) return;

    client = t->client;
    xid = t->xid;

    /* figure out how many events we need, and alloc them */
    if (buf == (char *)0)	/* Its an EOF, eat it	*/
      return;
    nEvents = (ct + (BYTESPEROUTPUTEVENT - 1)) / BYTESPEROUTPUTEVENT;
    pEvents = (PSOutputEvent *)ALLOCATE_LOCAL(nEvents*sizeof(PSOutputEvent));
    if (!pEvents)
        {
        InternalError(OutOfSpace);
        return;
        }

    /* chop buffer into events */
    for (pe = pEvents; ct != 0; ++pe)
        {
        len = min(len, ct);
        pe->type = eType;
        pe->length = len;
        pe->sequenceNumber = client->sequence;
        pe->cxid = xid;
        (void) os_bcopy(bb, pe->data, len);
        bb += len;
        ct -= len;
	/* Don't increase the sequence number; events shouldn't do this, only
           replys.							   */
        }
    sts1 = TryClientEvents( (ClientPtr)client, (xEvent *)pEvents, (int)nEvents,
                            (Mask)0, (Mask)NoEventMask, (GrabPtr)NullGrab);
    DEALLOCATE_LOCAL(pEvents);
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_FreeSpace                                                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
Priv_FreeSpace (pointer value, XID xid)
  /* Delete proc for space resource. This procedure is called	*/
  /* by X when the resource assoc'd with a DPS space is freed.	*/
  /* A space resource is freed by X when the client goes away.	*/
  /* We call a proc to destroy the space. It kills live ctxts	*/
  /* in the space. As each dies, its death is noted as a status	*/
  /* change. Priv_ReportStatusChange either makes the context a	*/
  /* zombie (the ctxt XID is still around) or frees the		*/
  /* XDPSIDEntry (ctxt XID has already been freed). If the ctxt	*/
  /* is made a zombie, it will ultimately be freed when the	*/
  /* client frees the context's XID. When all that is done, our	*/
  /* call to DPSSys_DestroySpace returns and we can free the	*/
  /* XDPSIDEntry for the space.	If the space's XDPSIDEntry is	*/
  /* marked as zombie, that means that the client previously	*/
  /* sent the DestroySpace request. In this case, all we need	*/
  /* to do is free the XDPSIDEntry.				*/
{
    register XDPSIDEntry sss = (XDPSIDEntry) value;

    /* fix for 2ps_xdps.321.bug */
    BITCLEAR(ClientsWithOverflow, sss->fd);
    AnyClientsWithOverflow = ANYSET(ClientsWithOverflow);

    if (!sss->isZombie)
      if (!DPSSys_DestroySpace(sss->id.dps_spid, Priv_SendOutputToClient))
        InternalError(DestroySpaceFailed);
    xfree(sss);
    return ( (int) 0 );
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_NewContextID                                                        */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_NewContextID  (XID xid, DPSSysCID dps_ctxid, DPSSysSID dps_spid,
        XID sxid, ClientPtr client, boolean secure)
{
    register XDPSIDEntry ccc;

    ccc = (XDPSIDEntry) xalloc(sizeof(XDPSIDEntryRec));
    ccc->next = (xdpsg->cxlist == (XDPSIDEntry) NULL)
      ? (XDPSIDEntry) NULL
      : xdpsg->cxlist;
    xdpsg->cxlist = ccc;
    ccc->type = xdps_context;
    ccc->dps_spid = dps_spid;
    ccc->sxid = sxid;
    ccc->id.dps_ctxid = dps_ctxid;
    ccc->xid = xid;	/* save the xid so we can get it from the cid */
    ccc->client = client;
    ccc->isZombie = false;
    ccc->secure = secure;
    ccc->fd = ((OsCommPtr)client->osPrivate)->fd;

    /* a context is a resource */
    if (!AddResource(xid, xdpsg->ctxtResType, (pointer)ccc))
        FatalError("Priv_NewContextID: AddResource failed\n");
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_NewSpaceID                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_NewSpaceID (XID xid, DPSSysSID dps_spid, ClientPtr client, boolean secure)
{
    register XDPSIDEntry sss;

    sss = (XDPSIDEntry) xalloc(sizeof(XDPSIDEntryRec));
    sss->type = xdps_space;
    sss->id.dps_spid = dps_spid;
    sss->xid = xid;	/* save the xid so we can it from the sid */
    sss->client = client;
    sss->isZombie = false;
    sss->secure = secure;
    sss->fd = ((OsCommPtr)client->osPrivate)->fd;

    /* a space is a resource */
    if (!AddResource(xid, xdpsg->spaceResType, (pointer)sss))
        FatalError("Priv_NewSpaceID: AddResource failed\n");
}

/*------------------------*/
/* ID conversion routines */
/*------------------------*/

/*****************************************************************************/
/*                                                                           */
/*  Priv_XIDtoContextID                                                      */
/*                                                                           */
/*****************************************************************************/

PRIVATE DPSSysCID
Priv_XIDtoContextID (XID xid)
{
    register XDPSIDEntry ccc;

    ccc = (XDPSIDEntry) LookupIDByType(xid, xdpsg->ctxtResType);
    if (ccc == (XDPSIDEntry) NULL)
        return((DPSSysCID) 0);
    return(ccc->id.dps_ctxid);
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_XIDtoSpaceID                                                        */
/*                                                                           */
/*****************************************************************************/

PRIVATE DPSSysSID
Priv_XIDtoSpaceID (XID xid)
{
    register XDPSIDEntry sss;

    sss = (XDPSIDEntry) LookupIDByType(xid, xdpsg->spaceResType);
    if (sss == (XDPSIDEntry) NULL)
        return((DPSSysCID) 0);
    return(sss->id.dps_spid);
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_ContextIDtoXID                                                      */
/*                                                                           */
/*****************************************************************************/

#if 0
PRIVATE XID
Priv_ContextIDtoXID (DPSSysCID dps_ctxid)
{
    register XDPSIDEntry t, prev;

    /* ASSERT: dps_ctxid's are unique during life of server */
    PSToXID(dps_ctxid, t, prev);
    if (t) return(t->xid);
    return((XID)None);
}
#endif

/*****************************************************************************/
/*                                                                           */
/*  Priv_SetOverflow - mark a DPS client as having or not having overflow    */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_SetOverflow (ClientPtr client, int val)
{
        register OsCommPtr oc = (OsCommPtr) client->osPrivate;

        if (val) {
                BITSET(ClientsWithOverflow, oc->fd);
                AnyClientsWithOverflow = TRUE;
        }
        else {
                BITCLEAR(ClientsWithOverflow, oc->fd);
                AnyClientsWithOverflow = ANYSET(ClientsWithOverflow);
        }
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_BlockHandler                                                        */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_BlockHandler (pointer data, struct timeval **wt, pointer LastSelectMask)
                                /* unused */
                            
                             
{
        static struct timeval nowait = { 0, 0 };

        /*
         * prevent X from reading more data from DPS clients that
         * have overflowed
         */
        if (AnyClientsWithOverflow) {
                UNSETBITS( ( (long int *)(LastSelectMask) ), ClientsWithOverflow);
        }

        /*
         * If DPS is ready, and there is no server grab in progress,
         * zero out the select timeout so that if there is no X activity
         * our wakeup handler will be called immediately.
         */
        if (xDPSReady &&
                GrabInProgress == 0) {
                if (*wt)
                        **wt = nowait;
                else
                        *wt = &nowait;
        }
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_WakeupHandler                                                       */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_WakeupHandler (pointer data, int i, pointer LastSelectMask)
                                /* unused */
              
                                /* unused */
{
        /*
         * Run DPS contexts if: there is no X activity, there is no
         * server grab in progress, and DPS is ready.
         */
        if (i == 0 &&
                xDPSReady &&
                GrabInProgress == 0) {
                DPSSys_GiveInput(Priv_SendOutputToClient, 0, (char *) 0, 0);
        }
}

/*****************************************************************************/
/*                                                                           */
/*  Priv_RegisterBlockAndWakeupHandlers                                      */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure
Priv_RegisterBlockAndWakeupHandlers (void)
{
        CLEARBITS(ClientsWithOverflow);
        AnyClientsWithOverflow = FALSE;

        RegisterBlockAndWakeupHandlers(
                Priv_BlockHandler,
                Priv_WakeupHandler,
                (pointer) 0);
}

/*------------------*/
/* Request Handlers */
/*------------------*/

/*****************************************************************************/
/*                                                                           */
/*  RH_Init                                                                  */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_Init (ClientPtr client)
{
    xPSInitReply reply;
    int len = strlen(DPS_FORMATNAME);
    REQUEST(xPSInitReq);

    REQUEST_SIZE_MATCH(xPSInitReq);

    /* Send BadMatch to clients with newer protocol than server,
       or with bogus protocol. */
    if (stuff->libraryversion > DPSPROTOCOLVERSION
     || stuff->libraryversion < XDPS_FIRST_DPS_PROTOCOL_VERSION) {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0, BadMatch);
      return(BadMatch);
      }

    /* Server MUST downgrade itself if client's protocol is older.
       Certain features are disabled if version doesn't match */
    xdpsg->version[client->index] = stuff->libraryversion;

    /* formulate reply */
    reply.type = X_Reply;
    reply.sequenceNumber = client->sequence;
    reply.serverversion = xdpsg->version[client->index];
    reply.preferredNumberFormat = DPS_DEF_TOKENTYPE;
    reply.floatingNameLength = len;
    reply.length = (len + 3) >> 2;
    WriteReplyToClient(client, sizeof(xPSInitReply), &reply);
    (void) WriteToClient(client, len, DPS_FORMATNAME);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_CreateContext                                                         */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_CreateContext (ClientPtr client)
{
    int result;
    char *device;
    xPSCreateContextReply reply;
    DPSSysCID dps_ctxid;
    DPSSysSID dps_spid;
    REQUEST(xPSCreateContextReq);

    REQUEST_SIZE_MATCH(xPSCreateContextReq);

    /* The XID for the context can not be in use	*/
    if (LookupIDByClass(stuff->cxid, RC_ANY) != None)
      {
      SendErrorToClient(
	client, xdpsg->reqCode, stuff->dpsReqType, 0, BadIDChoice);
      return(BadIDChoice);
      }

    if (stuff->sxid == None)
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0, BadIDChoice);
      return(BadIDChoice);
      }

    /* The XID for the space can be in use, but it must	*/
    /* be a space object, not a GC or something...	*/
    {
    XDPSIDEntry space = (XDPSIDEntry)LookupIDByType(stuff->sxid,
					xdpsg->spaceResType);

    if (space == (XDPSIDEntry)NULL)
      {
      if (LookupIDByClass(stuff->sxid, RC_ANY) != None)
        {
        SendErrorToClient(
	  client, xdpsg->reqCode, stuff->dpsReqType, 0, BadIDChoice);
        return(BadIDChoice);
        }
      }
    else if (xdpsg->version[client->index] >= DPSPROTO_V09
             && space->secure)
      {
        /* If we are PROTO 9 or newer, and this is a secure space,
           we can't create an unsecure context */
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0, BadAccess);
      return(BadAccess);
      }
    }

    /*-------------------------------*/
    /* Create the DPS device object. */
    /*-------------------------------*/

    result = DPSSys_CreateDevice ( (PDevice*)&device,
                                   client,
                                   stuff->drawable,
                                   stuff->gc,
                                   stuff->redmax,
                                   stuff->greenmax,
                                   stuff->bluemax,
                                   stuff->graymax,
                                   stuff->redmult,
                                   stuff->greenmult,
                                   stuff->bluemult,
                                   stuff->graymult,
                                   stuff->colorbase,
                                   stuff->graybase,
                                   stuff->actual,
                                   stuff->cmap,
                                   stuff->x,
                                   stuff->y);
    if (result != Success)
      {
      SendErrorToClient(
	client, xdpsg->reqCode, stuff->dpsReqType, 0, result);
      return(result);
      }

    dps_spid = Priv_XIDtoSpaceID(stuff->sxid);
    result = (dps_spid == 0);
    dps_ctxid = DPSSys_CreateContext((char *)client, &dps_spid, device, false);
    if (!dps_ctxid)
        {
        SendErrorToClient(
          client, xdpsg->reqCode, stuff->dpsReqType, 0, BadAlloc);
        return(BadAlloc);
        }
    Priv_NewContextID(stuff->cxid, dps_ctxid, dps_spid, stuff->sxid, client, false);
    if (result)
      Priv_NewSpaceID(stuff->sxid, dps_spid, client, false);

    /* don't try to change the GC on a null device */
    if (stuff->gc != None) {
       long int mask;
       XID vals[1];
       GSDrawingFunc func, *pFunc;

       mask = GCFunction;
       vals[0] = GXcopy;

       ChangeGC ( (GCPtr) ((XWININFO_FROM_DEVICE((PDevice)device))->gc), mask, vals);
       pFunc = (GSDrawingFunc *) PSGetGStateExt(NULL);
       func = GXcopy;
       SetDrawingFunction(pFunc, func);
    }

    /* formulate reply */
    reply.type = X_Reply;
    reply.sequenceNumber = client->sequence;
    reply.length = 0;
    reply.cpsid = dps_ctxid;
    WriteReplyToClient(client, sizeof(xPSCreateContextReply), &reply);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_CreateSpace                                                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_CreateSpace (ClientPtr client)
{
    REQUEST(xPSCreateContextReq);
    DPSSysSID dps_spid;

    /* The XID for the space can not be in use at all	*/
    if (LookupIDByClass(stuff->sxid, RC_ANY) != None)
      {
      SendErrorToClient(
	client, xdpsg->reqCode, stuff->dpsReqType, 0,
	XDPSError(PSERRORBADSPACE));
      return(XDPSError(PSERRORBADSPACE));
      }

    dps_spid = DPSSys_CreateSpace();
    if (dps_spid == 0)
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0, BadAlloc);
      return(BadAlloc);
      }
    Priv_NewSpaceID(stuff->sxid, dps_spid, client, false);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_GiveInput                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_GiveInput (ClientPtr client)
{
    XDPSIDEntry idEntry;
    char *buf;
    DPSSysCID dps_ctxid;
    REQUEST(xPSGiveInputReq);

    REQUEST_AT_LEAST_SIZE(xPSGiveInputReq);
    dps_ctxid = Priv_XIDtoContextID(stuff->cxid);
    if (dps_ctxid != 0)
      {
      idEntry = (XDPSIDEntry)LookupIDByType(stuff->cxid, xdpsg->ctxtResType);
      if (idEntry->isZombie)
        {
        Priv_GenerateStatusReport(client, stuff->cxid, PSZOMBIE);
        return(client->noClientException);
        }
      }
    /* the following cute trick works because stuff is a pointer to
       xPSGiveInputReq, thus &stuff[1] points to the first byte after the
       request structure. */
    buf = stuff->nunits ? (char *) &stuff[1] : (char *)0;
    DPSSys_GiveInput(Priv_SendOutputToClient, dps_ctxid, buf, stuff->nunits);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_GetStatus                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_GetStatus (ClientPtr client)
{
    XDPSIDEntry idEntry;
    xPSGetStatusReply reply;
    DPSSysCID dps_ctxid;
    int status;
    REQUEST(xPSGetStatusReq);

    if ((stuff->length << 2)!= 8)
      /* There was a snafu with the definition of this request, so
         some old clients may be using a request that is 8 bytes long.
         The correct length is 12, where the 'notifyIfChange' field
         must be 0. */
      REQUEST_SIZE_MATCH(xPSGetStatusReq);

    if ((stuff->length << 2) == sizeof(xPSGetStatusReq) &&
        stuff->notifyIfChange != 0)
      return(BadLength);

    dps_ctxid = Priv_XIDtoContextID(stuff->cxid);
    if (dps_ctxid == 0)
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0,
        XDPSError(PSERRORBADCONTEXT));
      return(XDPSError(PSERRORBADCONTEXT));
      }

    idEntry = (XDPSIDEntry)LookupIDByType(stuff->cxid, xdpsg->ctxtResType);
    if (idEntry == (XDPSIDEntry)NULL || idEntry->isZombie)
      {
      Priv_GenerateStatusReport(client, stuff->cxid, PSZOMBIE);
      status = PSZOMBIE;
      }
    else status = DPSSys_GetStatus(dps_ctxid);

    /* formulate reply */
    reply.type = X_Reply;
    reply.status = (BYTE)status;
    reply.sequenceNumber = client->sequence;
    reply.length = 0;
    WriteReplyToClient(client, sizeof(xPSGetStatusReply), &reply);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_SetStatusMask                                                         */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_SetStatusMask (ClientPtr client)
{
    XDPSIDEntry idEntry;
    DPSSysCID dps_ctxid;
    REQUEST(xPSSetStatusMaskReq);

    REQUEST_SIZE_MATCH(xPSSetStatusMaskReq);

    dps_ctxid = Priv_XIDtoContextID(stuff->cxid);
    if (dps_ctxid == 0)
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0,
        XDPSError(PSERRORBADCONTEXT));
      return(XDPSError(PSERRORBADCONTEXT));
      }

    idEntry = (XDPSIDEntry)LookupIDByType(stuff->cxid, xdpsg->ctxtResType);
    if (idEntry->isZombie)
      {
      Priv_GenerateStatusReport(client, stuff->cxid, PSZOMBIE);
      return(client->noClientException);
      }

    if (!ValidMask(stuff->enableMask)
    || !ValidMask(stuff->disableMask)
    || !ValidMask(stuff->nextMask)
    || (stuff->enableMask & stuff->disableMask)
    || (stuff->enableMask & stuff->nextMask)
    || (stuff->disableMask & stuff->nextMask))
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0, BadValue);
      return(BadValue);
      }

    DPSSys_SetStatusMask(
      dps_ctxid, (char *)client, (int)stuff->enableMask, (int)stuff->disableMask,
      (int)stuff->nextMask);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_DestroySpace                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_DestroySpace (ClientPtr client)
  /* It's our job to destroy the specified space, but	*/
  /* not to free the X resource. After destroying the	*/
  /* space, we set the XDPSIDEntry for the space to	*/
  /* zombie. Later, when the X space resource is freed	*/
  /* we'll delete the XDPSIDEntry. In the mean time, if	*/
  /* clients ask to use this space we'll see the zombie	*/
  /* flag and respond with an error.			*/
{
    XDPSIDEntry space;
    REQUEST(xPSDestroySpaceReq);

    REQUEST_SIZE_MATCH(xPSDestroySpaceReq);

    /* Validate the space parameter		*/
    space = (XDPSIDEntry)LookupIDByType(stuff->sxid, xdpsg->spaceResType);
    if (space == (XDPSIDEntry)NULL || space->isZombie)
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0,
        XDPSError(PSERRORBADSPACE));
      return(XDPSError(PSERRORBADSPACE));
      }

    if (!DPSSys_DestroySpace(space->id.dps_spid, Priv_SendOutputToClient))
      InternalError(DestroySpaceFailed);
    space->isZombie = true;
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_Reset                                                                 */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_Reset (ClientPtr client)
{
    DPSSysCID dps_ctxid;
    REQUEST(xPSResetReq);

    REQUEST_SIZE_MATCH(xPSResetReq);

    if ((dps_ctxid = Priv_XIDtoContextID(stuff->cxid)) == 0)
        {
        SendErrorToClient(
          client, xdpsg->reqCode, stuff->dpsReqType, 0,
          XDPSError(PSERRORBADCONTEXT));
        return(XDPSError(PSERRORBADCONTEXT));
        }

    DPSSys_ResetContext(dps_ctxid, Priv_SendOutputToClient);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_NotifyContext                                                         */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_NotifyContext (ClientPtr client)
{
    DPSSysCID dps_ctxid;
    XDPSIDEntry idEntry;
    REQUEST(xPSNotifyContextReq);

    REQUEST_SIZE_MATCH(xPSNotifyContextReq);

    if ((dps_ctxid = Priv_XIDtoContextID(stuff->cxid)) == 0)
      /* Invalid X context id				*/
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0,
        XDPSError(PSERRORBADCONTEXT));
      return(XDPSError(PSERRORBADCONTEXT));
      }

    if (stuff->notifyType > 3) /* 3 is an L2-DPS/PROTO 10 addition */
      /* notifyType has unnacceptable value		*/
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0, BadValue);
      return(BadValue);
      }

    idEntry = (XDPSIDEntry)LookupIDByType(stuff->cxid, xdpsg->ctxtResType);
    if (idEntry->isZombie)
      {
      Priv_GenerateStatusReport(client, stuff->cxid, PSZOMBIE);
      return(client->noClientException);
      }
    DPSSys_NotifyContext(dps_ctxid, stuff->notifyType);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_CreateContextFromID                                                   */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_CreateContextFromID (ClientPtr client)
{
    DPSSysSID dps_spid;
    XID sxid = 0;
    XDPSIDEntry cur;
    xPSCreateContextFromIDReply reply;
    REQUEST(xPSCreateContextFromIDReq);

    REQUEST_SIZE_MATCH(xPSCreateContextFromIDReq);

    if (!DPSSys_ValidContext(stuff->cpsid))
      /* This has got to be a context that PS has heard of	*/
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0, BadValue);
      return(BadValue);
      }

    dps_spid = DPSSys_ContextIDtoSpaceID(stuff->cpsid);
    for (cur = xdpsg->cxlist; cur != (XDPSIDEntry)NULL; cur = cur->next)
      {
      if (cur->id.dps_ctxid == stuff->cpsid) 
        {
        SendErrorToClient(
          client, xdpsg->reqCode, stuff->dpsReqType, 0, BadValue);
        return(BadValue);
        }
      else
        if (dps_spid == cur->dps_spid) sxid = cur->sxid;
      }

    if (Priv_XIDtoContextID(stuff->cxid) != 0)
      /* The X CID is already in use, can't reuse it.	*/
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0,
        XDPSError(PSERRORBADCONTEXT));
      return(XDPSError(PSERRORBADCONTEXT));
      }

    Priv_NewContextID(stuff->cxid, stuff->cpsid, dps_spid, sxid, client, false);

    DPSSys_CreateContextFromID(stuff->cpsid, (char*)client);

    /* formulate reply */
    reply.type = X_Reply;
    reply.sxid = sxid;
    reply.sequenceNumber = client->sequence;
    reply.length = 0;
    WriteReplyToClient(client, sizeof(xPSCreateContextFromIDReply), &reply);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_XIDFromContext                                                        */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_XIDFromContext (ClientPtr client)
{
    XDPSIDEntry cur, prev;
    xPSXIDFromContextReply reply;
    REQUEST(xPSXIDFromContextReq);

    REQUEST_SIZE_MATCH(xPSXIDFromContextReq);

    PSToXID(stuff->cpsid, cur, prev);
    if (cur == (XDPSIDEntry)NULL)
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0, BadValue);
      return(BadValue);
      }

    /* formulate reply */
    reply.type = X_Reply;
    reply.cxid = cur->xid;
    reply.sxid = cur->sxid;
    reply.sequenceNumber = client->sequence;
    reply.length = 0;
    WriteReplyToClient(client, sizeof(xPSContextFromXIDReply), &reply);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_ContextFromXID                                                        */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_ContextFromXID (ClientPtr client)
{
    DPSSysCID dps_ctxid;
    xPSContextFromXIDReply reply;
    REQUEST(xPSContextFromXIDReq);

    REQUEST_SIZE_MATCH(xPSContextFromXIDReq);

    /* Convert XID to PS CID	*/
    if ((dps_ctxid = Priv_XIDtoContextID(stuff->cxid)) == 0)
        {
        SendErrorToClient(
          client, xdpsg->reqCode, stuff->dpsReqType, 0,
          XDPSError(PSERRORBADCONTEXT));
        return(XDPSError(PSERRORBADCONTEXT));
        }
    /* formulate reply */
    reply.type = X_Reply;
    reply.cpsid = (CARD32)dps_ctxid;
    reply.sequenceNumber = client->sequence;
    reply.length = 0;
    WriteReplyToClient(client, sizeof(xPSContextFromXIDReply), &reply);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_CreateSecureContext (L2-DPS/PROTO 9 addition)                         */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_CreateSecureContext (ClientPtr client)
{
    int result;
    char *device;
    xPSCreateContextReply reply;         /* Use same reply as CreateContext */
    DPSSysCID dps_ctxid;
    DPSSysSID dps_spid;
    REQUEST(xPSCreateSecureContextReq);

    REQUEST_SIZE_MATCH(xPSCreateSecureContextReq);

    if (xdpsg->version[client->index] < DPSPROTO_V09)
      {
      SendErrorToClient(
	client, xdpsg->reqCode, stuff->dpsReqType, 0, BadRequest);
      return(BadRequest);
      }

    /* The XID for the context can not be in use	*/
    if (LookupIDByClass(stuff->cxid, RC_ANY) != None)
      {
      SendErrorToClient(
	client, xdpsg->reqCode, stuff->dpsReqType, 0, BadIDChoice);
      return(BadIDChoice);
      }

    if (stuff->sxid == None)
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0, BadIDChoice);
      return(BadIDChoice);
      }

    /* The XID for the space can be in use, but it must	*/
    /* be a space object, not a GC or something...	*/
    {
    XDPSIDEntry space = (XDPSIDEntry)LookupIDByType(stuff->sxid, 
					xdpsg->spaceResType);
    if (space == (XDPSIDEntry)NULL)
      {
      if (LookupIDByClass(stuff->sxid, RC_ANY) != None)
        {
        SendErrorToClient(
	  client, xdpsg->reqCode, stuff->dpsReqType, 0, BadIDChoice);
        return(BadIDChoice);
        }
      }
    else if (!space->secure)
      {
      /* If we are PROTO 9 or newer, and the space is not secure,
         we can't create a secure context in it. */
      SendErrorToClient(
	client, xdpsg->reqCode, stuff->dpsReqType, 0, BadAccess);
      return(BadAccess);
      }
    }

    /*-------------------------------*/
    /* Create the DPS device object. */
    /*-------------------------------*/

    result = DPSSys_CreateDevice ( (PDevice*)&device,
                                   client,
                                   stuff->drawable,
                                   stuff->gc,
                                   stuff->redmax,
                                   stuff->greenmax,
                                   stuff->bluemax,
                                   stuff->graymax,
                                   stuff->redmult,
                                   stuff->greenmult,
                                   stuff->bluemult,
                                   stuff->graymult,
                                   stuff->colorbase,
                                   stuff->graybase,
                                   stuff->actual,
                                   stuff->cmap,
                                   stuff->x,
                                   stuff->y);
    if (result != Success)
      {
      SendErrorToClient(
	client, xdpsg->reqCode, stuff->dpsReqType, 0, result);
      return(result);
      }

    dps_spid = Priv_XIDtoSpaceID(stuff->sxid);
    result = (dps_spid == 0);
    dps_ctxid = DPSSys_CreateContext((char *)client, &dps_spid, device, true);
    if (!dps_ctxid)
        {
        SendErrorToClient(
          client, xdpsg->reqCode, stuff->dpsReqType, 0, BadAlloc);
        return(BadAlloc);
        }
    Priv_NewContextID(stuff->cxid, dps_ctxid, dps_spid, stuff->sxid, client, true);
    if (result)
      Priv_NewSpaceID(stuff->sxid, dps_spid, client, true);

    /* don't try to change the GC on a null device */
    if (stuff->gc != None) {
       long int mask;
       XID vals[1];
       GSDrawingFunc func, *pFunc;

       mask = GCFunction;
       vals[0] = GXcopy;

       ChangeGC ( (GCPtr) ((XWININFO_FROM_DEVICE((PDevice)device))->gc), mask, vals);
       pFunc = (GSDrawingFunc *) PSGetGStateExt(NULL);
       func = GXcopy;
       SetDrawingFunction(pFunc, func);
    }

    /* formulate reply */
    reply.type = X_Reply;
    reply.sequenceNumber = client->sequence;
    reply.length = 0;
    reply.cpsid = dps_ctxid;
    WriteReplyToClient(client, sizeof(xPSCreateContextReply), &reply);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_NotifyWhenReady (L2-DPS/PROTO 9 addition)                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_NotifyWhenReady (ClientPtr client)
{
    DPSSysCID dps_ctxid;
    XDPSIDEntry idEntry;
    integer val[4];
    REQUEST(xPSNotifyWhenReadyReq);

    if (xdpsg->version[client->index] < DPSPROTO_V09)
      {
      SendErrorToClient(
	client, xdpsg->reqCode, stuff->dpsReqType, 0, BadRequest);
      return(BadRequest);
      }

    REQUEST_SIZE_MATCH(xPSNotifyWhenReadyReq);

    if ((dps_ctxid = Priv_XIDtoContextID(stuff->cxid)) == 0)
      /* Invalid X context id				*/
      {
      SendErrorToClient(
        client, xdpsg->reqCode, stuff->dpsReqType, 0,
        XDPSError(PSERRORBADCONTEXT));
      return(XDPSError(PSERRORBADCONTEXT));
      }

    idEntry = (XDPSIDEntry)LookupIDByType(stuff->cxid, xdpsg->ctxtResType);
    if (idEntry->isZombie)
      {
      Priv_GenerateStatusReport(client, stuff->cxid, PSZOMBIE);
      return(client->noClientException);
      }

    val[0] = stuff->val1;
    val[1] = stuff->val2;
    val[2] = stuff->val3;
    val[3] = stuff->val4;
    DPSSys_NotifyWhenReady(Priv_SendOutputToClient, dps_ctxid, val);
    return(client->noClientException);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_Dispatch                                                              */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_Dispatch (ClientPtr client)
{
  register int status;
  REQUEST(xReq);

  if (xdpsg->extensionAborted)
  {
     if (client != serverClient)
     {
        Priv_ReportAbort(client);
        return(XDPSError(PSERRORABORT));
     }
     else
        return (Success);
  }

  switch (stuff->data)
  {
     case X_PSInit:                status = RH_Init                (client); break;
     case X_PSCreateContext:       status = RH_CreateContext       (client); break;
     case X_PSCreateSpace:         status = RH_CreateSpace         (client); break;
     case X_PSGiveInput:           status = RH_GiveInput           (client); break;
     case X_PSGetStatus:           status = RH_GetStatus           (client); break;
     case X_PSDestroySpace:        status = RH_DestroySpace        (client); break;
     case X_PSReset:               status = RH_Reset               (client); break;
     case X_PSNotifyContext:       status = RH_NotifyContext       (client); break;
     case X_PSCreateContextFromID: status = RH_CreateContextFromID (client); break;
     case X_PSXIDFromContext:      status = RH_XIDFromContext      (client); break;
     case X_PSContextFromXID:      status = RH_ContextFromXID      (client); break;
     case X_PSSetStatusMask:       status = RH_SetStatusMask       (client); break;
     /* L2-DPS/PROTO 9 additions */
     case X_PSCreateSecureContext: status = RH_CreateSecureContext (client); break;
     case X_PSNotifyWhenReady:     status = RH_NotifyWhenReady     (client); break;
     default:
        SendErrorToClient(client, xdpsg->reqCode, stuff->data, 0, BadRequest);
        status = BadRequest;
        break;
  }

  if (xdpsg->extensionAborted)
  {
     Priv_ReportAbort ((ClientPtr)NULL);
     if (client == serverClient)
        status = Success;
     else
        status = XDPSError(PSERRORABORT);
  }
  return (status);
}

/*****************************************************************************/
/*                                                                           */
/*  RH_SwapDispatch                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE int
RH_SwapDispatch (ClientPtr client)
{
        register int status;
    REQUEST(xReq);
    register char n;

    if (xdpsg->extensionAborted) {
                if (client != serverClient) {
                        Priv_ReportAbort(client);
                        return(XDPSError(PSERRORABORT));
                }
                else
                        return (Success);
        }

        switch (stuff->data) {
                case X_PSInit:
                        {
                                register xPSInitReq *dpsReq = (xPSInitReq *)stuff;
                                swaps(&dpsReq->length, n);
                                swapl(&dpsReq->libraryversion, n);
                                status = RH_Init(client);
                        }
                        break;
                case X_PSCreateContext:
                        {
                                register xPSCreateContextReq *dpsReq =
                                        (xPSCreateContextReq *)stuff;
                                swaps(&dpsReq->length, n);
                                SwapLongs(&dpsReq->cxid, 4);
                                SwapShorts(&dpsReq->x, 2);
                                SwapLongs(&dpsReq->eventmask, 13);
                                status = RH_CreateContext(client);
                        }
                        break;
                case X_PSCreateSpace:
                        {
                                register xPSCreateSpaceReq *dpsReq =
                                        (xPSCreateSpaceReq *)stuff;
                                swaps(&dpsReq->length, n);
                                swapl(&dpsReq->sxid, n);
                                status = RH_CreateSpace(client);
                        }
                        break;
                case X_PSGiveInput:
                        {
                                register xPSGiveInputReq *dpsReq = (xPSGiveInputReq *)stuff;
                                swaps(&dpsReq->length, n);
                                swaps(&dpsReq->nunits, n);
                                swapl(&dpsReq->cxid, n);
                                status = RH_GiveInput(client);
                        }
                        break;
                case X_PSGetStatus:
                        {
                                register xPSGetStatusReq *dpsReq = (xPSGetStatusReq *)stuff;
                                swaps(&dpsReq->length, n);
                                swapl(&dpsReq->cxid, n);
                                status = RH_GetStatus(client);
                        }
                        break;
                case X_PSDestroySpace:
                        {
                                register xPSDestroySpaceReq *dpsReq =
                                        (xPSDestroySpaceReq *)stuff;
                                swaps(&dpsReq->length, n);
                                swapl(&dpsReq->sxid, n);
                                status = RH_DestroySpace(client);
                        }
                        break;
                case X_PSReset:
                        {
                                register xPSResetReq *dpsReq = (xPSResetReq *)stuff;
                                swaps(&dpsReq->length, n);
                                swapl(&dpsReq->cxid, n);
                                status = RH_Reset(client);
                        }
                        break;
                case X_PSNotifyContext:
                        {
                                register xPSNotifyContextReq *dpsReq =
                                        (xPSNotifyContextReq *)stuff;
                                swaps(&dpsReq->length, n);
                                swapl(&dpsReq->cxid, n);
                                status = RH_NotifyContext(client);
                        }
                        break;
                case X_PSCreateContextFromID:
                        {
                                register xPSCreateContextFromIDReq *dpsReq =
                                        (xPSCreateContextFromIDReq *)stuff;
                                swaps(&dpsReq->length, n);
                                swapl(&dpsReq->cpsid, n);
                                swapl(&dpsReq->cxid, n);
                                status = RH_CreateContextFromID(client);
                        }
                        break;
                case X_PSXIDFromContext:
                        {
                                register xPSXIDFromContextReq *dpsReq =
                                        (xPSXIDFromContextReq *)stuff;
                                swaps(&dpsReq->length, n);
                                swapl(&dpsReq->cpsid, n);
                                status = RH_XIDFromContext(client);
                        }
                        break;
                case X_PSContextFromXID:
                        {
                                register xPSContextFromXIDReq *dpsReq =
                                        (xPSContextFromXIDReq *)stuff;
                                swaps(&dpsReq->length, n);
                                swapl(&dpsReq->cxid, n);
                                status = RH_ContextFromXID(client);
                        }
                        break;
                case X_PSSetStatusMask:
                        {
                                register xPSSetStatusMaskReq *dpsReq =
                                        (xPSSetStatusMaskReq *)stuff;
                                swaps(&dpsReq->length, n);
                                swapl(&dpsReq->cxid, n);
                                swapl(&dpsReq->enableMask, n);
                                swapl(&dpsReq->nextMask, n);
                                swapl(&dpsReq->disableMask, n);
                                status = RH_SetStatusMask(client);
                        }
                        break;
                case X_PSCreateSecureContext:
                        {
                                register xPSCreateSecureContextReq *dpsReq =
                                        (xPSCreateSecureContextReq *)stuff;
                                swaps(&dpsReq->length, n);
                                SwapLongs(&dpsReq->cxid, 4);
                                SwapShorts(&dpsReq->x, 2);
                                SwapLongs(&dpsReq->eventmask, 13);
                                status = RH_CreateSecureContext(client);
                        }
                        break;
                case X_PSNotifyWhenReady:
                        {
                                register xPSNotifyWhenReadyReq *dpsReq =
                                        (xPSNotifyWhenReadyReq *)stuff;
                                swaps(&dpsReq->length, n);
                                SwapLongs(&dpsReq->cxid, 5);
                                status = RH_NotifyWhenReady(client);
                        }
                        break;
                default:
                        SendErrorToClient(client, xdpsg->reqCode, stuff->data, 0, BadRequest);
                        status = BadRequest;
                        break;
        }
        if (xdpsg->extensionAborted) {
                Priv_ReportAbort ((ClientPtr)NULL);
                if (client == serverClient)
                        status = Success;
                else
                        status = XDPSError(PSERRORABORT);
        }
        return (status);
}

/*------------------------------------*/
/* PRIVATE startup support procedures */
/*------------------------------------*/

/*****************************************************************************/
/*                                                                           */
/*  dpsDispatch                                                              */
/*                                                                           */
/*****************************************************************************/

PRIVATE int dpsDispatch (ClientPtr client)
{
  return ((*xdpsg->dispatch)(client));
}

/*****************************************************************************/
/*                                                                           */
/*  dpsSwapDispatch                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE int dpsSwapDispatch (ClientPtr client)
{
  return ((*xdpsg->swapDispatch)(client));
}

/*****************************************************************************/
/*                                                                           */
/*  InitializeDisplayPostScript                                              */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure InitializeDisplayPostScript (XDPSGlobals xdpsg)
{
  if (! xdpsg->extensionAborted) {
    if (!DPSSys_Init(Priv_ReportStatusChange, Priv_ReportDeath, Priv_ReportReady, &xDPSReady)) {
      ErrorF("XDPS Extension: PostScript kernel initialization failed.\n");
      xdpsg->extensionAborted = true;
    }
    else {
      RegisterExit(Priv_ExitProc);
      RgstCantHappenProc (Priv_CantHappenProc);
      xdpsg->extensionInited = true;
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  InitializeExtension                                                      */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure InitializeExtension (XDPSGlobals xdpsg)
{
  ExtensionEntry *extEntry;

  if (! xdpsg->extensionAborted)
  {
     xdpsg->ctxtResType  = CreateNewResourceType(Priv_FreeContext);
     xdpsg->spaceResType = CreateNewResourceType(Priv_FreeSpace);

     if ((xdpsg->ctxtResType == 0) || (xdpsg->spaceResType == 0))
     {
        ErrorF("XDPS Extension: no more resource types\n");
        xdpsg->extensionAborted = true;
        return;
     }

#if (OS == os_irixV) || (OS == os_irix6)

     {
     typedef int  (*XDPSentryPoint)       ARGDECL0();
     typedef int  (*XDPSdispatchProc)     ARGDECL1(ClientPtr, client);
     typedef int  (*XDPSswapDispatchProc) ARGDECL1(ClientPtr, client);
     typedef void (*XDPSclosedownProc)    ARGDECL1(ExtensionEntry *, extension);


     extern ExtensionEntry
     *AddXdpsExtension ARGDECL4(XDPSentryPoint,       entryPoint,
                                XDPSdispatchProc,     dispatchProc,
                                XDPSswapDispatchProc, swapDispatchProc,
                                XDPSclosedownProc,    closedownProc);

     extEntry = AddXdpsExtension (
                    XDPSExtensionInit,    /* register XDPS entry point with server */
                    dpsDispatch,          /* request dispatcher         */
                    dpsSwapDispatch,      /* swapped request dispatcher */
                    Priv_ExtReset);       /* extension closedown proc   */

     DPSSys_Restart();
     }
#else

     DPSSys_Restart();

     extEntry = AddExtension (
                    DPSNAME,              /* extension name             */
                    NPSEVENTS,            /* number of events           */
                    NPSERRORS,            /* number of errors           */
                    dpsDispatch,          /* request dispatcher         */
                    dpsSwapDispatch,      /* swapped request dispatcher */
                    Priv_ExtReset,        /* extension closedown proc   */
                    StandardMinorOpcode); /* minor opcode proc          */

#endif

     if (extEntry)
     {

#if (OS != os_irixV) && (OS != os_irix6)
        (void) MakeAtom(DPSNAME,    os_strlen(DPSNAME),    TRUE);
        (void) MakeAtom(DECDPSNAME, os_strlen(DECDPSNAME), TRUE);

        if (!AddExtensionAlias(DECDPSNAME, extEntry))
          ErrorF ("XDPS Extension: AddExtensionAlias failed.\n");
#endif

        xdpsg->reqCode   = extEntry->base;
        xdpsg->eventBase = extEntry->eventBase;
        xdpsg->errorBase = extEntry->errorBase;
        xdpsg->cxlist    = (XDPSIDEntry) NULL;

        EventSwapVector[extEntry->eventBase + PSEVENTSTATUS] = Priv_EventSwap;
        EventSwapVector[extEntry->eventBase + PSEVENTOUTPUT] = Priv_EventSwap;

        /* L2-DPS/PROTO 9 addition */

        EventSwapVector[extEntry->eventBase + PSEVENTREADY]  = Priv_ReadyEventSwap;
        ReplySwapVector[extEntry->base]                      = Priv_ReplySwap;

        Priv_RegisterBlockAndWakeupHandlers();
     }
     else
     {
        ErrorF("XDPS Extension: AddExtensions failed\n");
        xdpsg->extensionAborted = true;
     }
  }
  else
  {
     ErrorF("XDPS Extension: Extension initialization failed\n");
  }
}

/*****************************************************************************/
/*                                                                           */
/*  DeferredInitDispatch                                                     */
/*                                                                           */
/*****************************************************************************/

PRIVATE int DeferredInitDispatch (ClientPtr client)
{
  InitializeDisplayPostScript(xdpsg);
  xdpsg->dispatch     = RH_Dispatch;
  xdpsg->swapDispatch = RH_SwapDispatch;
  return ((*xdpsg->dispatch)(client));
}

/*****************************************************************************/
/*                                                                           */
/*  DeferredInitSwapDispatch                                                 */
/*                                                                           */
/*****************************************************************************/

PRIVATE int DeferredInitSwapDispatch (ClientPtr client)
{
  InitializeDisplayPostScript(xdpsg);
  xdpsg->dispatch     = RH_Dispatch;
  xdpsg->swapDispatch = RH_SwapDispatch;
  return ((*xdpsg->swapDispatch)(client));
}

/*****************************************************************************/
/*                                                                           */
/*  prepare_for_startup                                                      */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure prepare_for_startup (void)
{
  os_initPages        ( (char *) 0, XDPS_OS_INIT_PAGES_SIZE );
  InitPSMemoryHandles (sysID);

#if (MALLOC_TRACKING == 1)
  {
     boolean tst1;
     tst1 = sys_setMallocSegmentTracking(true);
     DebugAssert (tst1 == false);
  }
#endif

  {
     /* in products/adobe/xdps/sources/xdps_product.c */
     /* XYZ: clean it up */
     extern procedure XDPS_ProductDebugSleep ARGDECL0();
     XDPS_ProductDebugSleep();
  }
}

/*-------------------*/
/* PUBLIC procedures */
/*-------------------*/

/*****************************************************************************/
/*                                                                           */
/*  XDPSExtensionInit (exported through XDPS_SUPPORT)                        */
/*                                                                           */
/*****************************************************************************/

#if (OS == os_irixV) || (OS == os_irix6)
PUBLIC int       XDPSExtensionInit (void)
#else
PUBLIC procedure XDPSExtensionInit (void)
#endif
{
  prepare_for_startup ();

  if ( ! xdpsg )
  {
     xdpsg = &xDPSGlobalsStruct;
     xdpsg->extensionInited  = false;
     xdpsg->extensionAborted = false;
     xdpsg->dispatch         = RH_Dispatch;
     xdpsg->swapDispatch     = RH_SwapDispatch;

     InitializeDisplayPostScript (xdpsg);
  }
  InitializeExtension (xdpsg);

#if (OS == os_irixV) || (OS == os_irix6)

  if (xdpsg->extensionAborted)
     return 0;  /* error found, extension aborted */
  else
     return 1;  /* no errors,   extension initialized */

#endif

}

/*****************************************************************************/
/*                                                                           */
/*  XDPSExtensionDeferredInit (exported through XDPS_SUPPORT)                */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPSExtensionDeferredInit (void)
{

#if (OS == os_sun)
  FatalError("XDPS - Deferred initialization is not supported on this platform");
#endif

  prepare_for_startup();

  if ( ! xdpsg )
  {
     xdpsg = &xDPSGlobalsStruct;
     xdpsg->extensionInited  = false;
     xdpsg->extensionAborted = false;
     xdpsg->dispatch         = DeferredInitDispatch;
     xdpsg->swapDispatch     = DeferredInitSwapDispatch;
  }
  InitializeExtension (xdpsg);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPSExtensionMaybeInit (exported through XDPS_SUPPORT)                   */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean XDPSExtensionMaybeInit (void)
{
  if (!xdpsg)
    return (false);
  if (!xdpsg->extensionAborted && !xdpsg->extensionInited)
    InitializeDisplayPostScript (xdpsg);
  return ((boolean)(!xdpsg->extensionAborted && xdpsg->extensionInited));
}

/*****************************************************************************/
/*                                                                           */
/*  XDPSSetOverflow                                                          */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPSSetOverflow (DPSSysCID dps_ctxid, int val)
{
  register XDPSIDEntry idEntry, prevIDEntry;

  PSToXID(dps_ctxid, idEntry, prevIDEntry);
  if (idEntry == (XDPSIDEntry) NULL)
    InternalError(ContextWentAway);
  if (CtxtXIDIsGone(idEntry)) return;
  Priv_SetOverflow (idEntry->client, val);
}

