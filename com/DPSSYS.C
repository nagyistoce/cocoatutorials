/* @(#)CM_VerSion dpssys.c 3011 1.3 22350.eco sum= 35191 3011.004 */
/* @(#)CM_VerSion dpssys.c 3010 1.6 20436.eco sum= 46325 3010.103 */
/* @(#)CM_VerSion dpssys.c 2017 1.6 17334.eco sum= 16147 2017.011 */
/* @(#)CM_VerSion dpssys.c 2016 1.5 13872.eco sum= 40910 */
/* @(#)CM_VerSion dpssys.c 2015 1.12 13214.eco sum= 33757 */
/* @(#)CM_VerSion dpssys.c 2014 1.7 08790.eco sum= 29226 */
/* @(#)CM_VerSion dpssys.c 2013 1.11 08696.eco sum= 08898 */
/*
  dpssys.c

(c) Copyright 1989-1993 Adobe Systems Incorporated.
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
*  File: dpssys.c                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
*  The PUBLIC procedures in this file are declared extern in dpssys.h         *
*                                                                             *
*  This file contains procedures for context and client management, and for   *
*  managing flow control.                                                     *
*                                                                             *
*******************************************************************************
*                                                                             *
*  PRIVATE PROCEDURES:                                                        *
*                                                                             *
*    Client management procedures                                             *
*                                                                             *
*      AddClient                                                              *
*      NoteContextClientRelationship                                          *
*      DeleteClientRec                                                        *
*                                                                             *
*    Buffer & context management procedures                                   *
*                                                                             *
*      ReleaseBuf                                                             *
*      GetBuf                                                                 *
*      StartContext                                                           *
*      SimpleOutput                                                           *
*      DoPostScriptCleanup                                                    *
*      ExecuteStmInContext                                                    *
*      ExecuteStartupFile                                                     *
*                                                                             *
*    Scheduler extension procedures                                           *
*                                                                             *
*      EnteringSched                                                          *
*      CleanupSchedExt                                                        *
*      ReportStatusChange                                                     *
*      ReportClientXready                                                     *
*      AboutToSchedule                                                        *
*      NoteBlockedContext                                                     *
*      ContextYielded                                                         *
*      AddXSchedInfo                                                          *
*      FlowControlReleaseInput                                                *
*                                                                             *
*  PUBLIC PROCEDURES:                                                         *
*                                                                             *
*    DPSSys_SetupSchedExtVal                                                  *
*    DPSSys_ValidContext                                                      *
*    DPSSys_DestroySpace                                                      *
*    DPSSys_CreateSpace                                                       *
*    DPSSys_CreateContextFromID                                               *
*    DPSSys_CreateContext                                                     *
*    DPSSys_GiveInput                                                         *
*    DPSSys_RegisterReturnProc                                                *
*    DPSSys_NotifyContext                                                     *
*    DPSSys_NotifyWhenReady                                                   *
*    DPSSys_ResetContext                                                      *
*    DPSSys_SpaceIDFromContextID                                              *
*    DPSSys_SetStatusMask                                                     *
*    DPSSys_GetStatus                                                         *
*    DPSSys_Restart                                                           *
*    DPSSys_Reset                                                             *
*    DPSSys_Init                                                              *
*                                                                             *
*  PUBLIC VARIABLES:                                                          *
*                                                                             *
*    <none>                                                                   *
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
#include DEVCREATE
#include SERVER
#include ATMINIT
#include CANTHAPPEN
#include STODEV
#include FILTERS
#include DEVICE
#include DEVICEIO     /* USING os_cacheLineSize */
#include PSXDPSMARKER

#include <unistd.h>  /* USING write() */

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "xdpsext.h"
#include "dpssys.h"
#include "xdevice.h"
#include "genxsupport.h"
#include "xdpsops.h"

/*----------------------------*/
/* Include files from X world */
/*----------------------------*/

#define  NEED_EVENTS
#include "X.h"		/* USING [XID]	-- For XDPS.h				*/
#include "DPS/XDPS.h"	/* USING [PSKILL, PSINTERRUPT, PSUNFREEZE, PSFREEZE]	*/

/*-----------*/
/* constants */
/*-----------*/

/*---------------------------------------------------------------*/
/* This is a threshhold for context input.  The client will be   */
/* ignored if count of bytes in the context input stream exceeds */
/* this level                                                    */
/*---------------------------------------------------------------*/

#define XDPS_CONTEXT_INPUT_HIGH_LEVEL   64*1024

#define XDPS_MAX_BUF_BYTES_TO_KEEP      8000
#define XDPS_MAX_BUFS_IN_FREE_LIST      8

/*-----------------------------------------------------------*/
/* This is the startup string used for context creation      */
/*-----------------------------------------------------------*/
#define XDPS_CONTEXT_STARTUP_STRING                           \
                                                              \
   "{/DefaultCIE /ColorRendering findresource} stopped        \
      {currentglobal 3 1 roll true setglobal                  \
	globaldict /_DfltColorRenderingDict get               \
	exch defineresource                                   \
	exch setglobal} if                                    \
    setcolorrendering                                         \
    {/InitProc /ContextInit findresource} stopped	      \
      {currentglobal 3 1 roll true setglobal                  \
        [] cvx exch defineresource pop                        \
	setglobal                                             \
      } {exec} ifelse                                         \
    mark {1183615869 internaldict /idiomRecogInit get exec }  \
    stopped {$error /newerror false put} if cleartomark       \
    $error /cleanup get exec                                  \
    $error /command null put                                  \
    /start load stopped pop systemdict                        \
    /quit get exec"                                           \


/*-----------------------------------------------------------*/
/* This is the cleanup string used for context cleanup       */
/*-----------------------------------------------------------*/

#define XDPS_CONTEXT_CLEANUP_STRING                           \
                                                              \
   "true setshared                                            \
    SharedFontDirectory dup length dict copy                  \
    {pop dup /Font resourcestatus                             \
      {pop 1 eq {/Font undefineresource}{pop} ifelse}         \
      {pop}                                                   \
    ifelse}                                                   \
    forall                                                    \
    2 vmreclaim                                               \
    systemdict /quit get exec"                                \

/*-----------------------------------------------------------*/
/* This is the startup string used for stm context creation  */
/*-----------------------------------------------------------*/

#define XDPS_STM_CONTEXT_STARTUP_STRING                       \
                                                              \
   " {run} stopped                                            \
     {/handleerror load stopped pop} if                       \
     systemdict /quit get exec"                               \

/*-----------------------------------------------------------*/
/* When supplied as the returnControl parm, this value tells */
/* ServicePostScript that DPS should return immediately.     */
/*-----------------------------------------------------------*/

#define	ALWAYSRETURN	((XdpsReturnProc) -2 )

#define	ReportContextDeath(ctx)	(*priv_ctxtDeathProc)(PSContextToID(ctx))

#define GetClient(ctx)	(((SchedExt *)GetSchedExt(ctx))->extClientRecord)

/*----------*/
/* typedefs */
/*----------*/

/*------------------------------*/
typedef struct _t_ExtClientRecord
{
  char    *client;                 /* opaque client pointer               */
  integer  refCount;               /* How many ctxts refer to this client */
  integer  blockedCount;           /* Number of contexts for this client  */
                                   /* that are blocked on clientsync, a   */
                                   /* monitor, orcondition variable.      */
  ContextID whichCtxtOverran;	   /* If a context input buffer is	  */
                                   /* being overrun, then we turn off its */
                                   /* client. We keep track of which ctxt */
                                   /* had the overrun so we don't un-     */
                                   /* flow-control the client because a   */
                                   /* diff. ctxt falls below the high     */
                                   /* water mark. This field is only      */
                                   /* meaningful when the client is flow- */
                                   /* controlled.                         */
  struct _t_ExtClientRecord *next; /* Next item in list                   */
} ExtClientRecord;

/*------------------------------*/
typedef struct _t_StatusInfo
{
  char    *client;                 /* opaque client pointer               */
  Card16   xMode;                  /* mask of interesting status changes  */
  Card16   oneShots;               /* mask chngs that are intersting once */
  boolean  markedForDelete;        /* entry should be removed from chain  */
  struct _t_StatusInfo *next;      /* next client in chain                */
} StatusInfo;

/*------------------------------*/
typedef struct _t_SchedExt          /* Extension to scheduler ctxt        */
{
  Card16           xStatus;         /* current status                     */
  StatusInfo      *clients;         /* clients interested in status       */
  boolean          markedForDelete; /* extension should be deleted        */
  ExtClientRecord *extClientRecord; /* Ptr to client related data         */
  int              paused_flag;     /* L2-DPS/PROTO 10 addition           */
  boolean          val_flag;        /* true if clientXready               */
                                    /* false if clientsync                */
  integer          val_array[4];    /* clientXready parameters            */
} SchedExt;

/*------------------------------*/
typedef struct _t_BufHeader
{
  longinteger  size;          /* always used */
  integer *val_ptr;           /* always used */
  struct _t_BufHeader *next;  /* only used when buffer is on free list */
  struct _t_BufHeader *prev;  /* only used when buffer is on free list */
} BufHeader;


#define STATIC_BUFHEADER_SIZE  ( sizeof(longinteger) + sizeof(integer*) )
#define VAL_ARRAY_SIZE         ( 4 * sizeof(integer) )

/*-----------------*/
/* local variables */
/*-----------------*/

PRIVATE ExecStartupFile;
PRIVATE maskBitForType[] =
{
  PSRUNNINGMASK,    /* PSRUNNING    */
  PSNEEDSINPUTMASK, /* PSNEEDSINPUT */
  PSZOMBIEMASK,     /* PSZOMBIE     */
  PSFROZENMASK,     /* PSFROZEN     */
};

PRIVATE XdpsReadyProc priv_readyProc;

/*------------------------------------------------------------*/
/* This is the value we'll pass as the returnControl param to */
/* ServicePostScript. Settable by DPSSys_RegisterReturnProc.  */
/*------------------------------------------------------------*/

PRIVATE boolean (*gReturnControl)ARGDECL0() = ALWAYSRETURN;

/*-----------------------------------------------------------*/
/* This is the procedure to call if the status of a context  */
/* changes and the client is interested. Set in DPSSys_Init. */
/*-----------------------------------------------------------*/

PRIVATE XdpsStatusProc priv_statusChangeProc;

/*----------------------------------------------------------*/
/* This is the procedure to call when a context terminates. */
/* This proc is called in addition to informing clients of  */
/* the change of ctxt status (see statusChangeProc). Set in */
/* DPSSys_Init()                                            */
/*----------------------------------------------------------*/

PRIVATE XdpsDeathProc priv_ctxtDeathProc;

PRIVATE ContextID         null_ctxid;
PRIVATE ExtClientRecord  *headOfClientList;

PRIVATE BufHeader        *freeBufs;
PRIVATE integer           totalSizeOnHand;
PRIVATE integer           bufsInFreeList;

/*--------------------*/
/* PRIVATE procedures */
/*--------------------*/

/*------------------------------*/
/* Client management procedures */
/*------------------------------*/

/*****************************************************************************/
/*                                                                           */
/*  AddClient                                                                */
/*                                                                           */
/*****************************************************************************/

PRIVATE ExtClientRecord *AddClient ARGDEF1(char *,client)
               		/* opaque ptr to client			*/
  {
  ExtClientRecord *cur;

  for (cur = headOfClientList; cur; cur = cur->next)
    if (cur->client == client) break;
  if (cur != NULL)
    {
    cur->refCount++;
    return(cur);
    }
  /* We didn't find it and we want to add it as a new element	*/
  cur = (ExtClientRecord *)sys_malloc(sizeof(ExtClientRecord));
  cur->client = client;
  cur->blockedCount = 0;
  cur->refCount = 1;
  if (headOfClientList == NULL)
    {
    headOfClientList = cur;
    cur->next = NULL;
    }
  else
    {
    cur->next = headOfClientList;
    headOfClientList = cur;
    }
  return(cur);
  }

/*****************************************************************************/
/*                                                                           */
/*  NoteContextClientRelationship                                            */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure NoteContextClientRelationship ARGDEF2(ContextID, ctxid, char *,client)
{
  ExtClientRecord *cr;
  SchedExt *se;

  se = (SchedExt *)GetSchedExt(IDToPSContext(ctxid));
  se->clients->client = client;

  cr = AddClient(client);
  se->extClientRecord = cr;
  }

/*****************************************************************************/
/*                                                                           */
/*  DeleteClientRec                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure DeleteClientRec ARGDEF1(ExtClientRecord *,extClientRecord)
{
  ExtClientRecord *cur, *prev;

  if (--extClientRecord->refCount != 0)
    return;

  /* refCount has fallen to zero, really remove the entry	*/
  for (cur = headOfClientList, prev = NULL; cur; prev = cur, cur = cur->next)
    if (cur == extClientRecord) break;
  Assert(cur != NULL);
  if (prev == NULL)	/* We're deleting the first item	*/
    headOfClientList = cur->next;
  else
    prev->next = cur->next;

  sys_free(cur);
  }

/*----------------------------------------*/
/* Buffer & context management procedures */
/*----------------------------------------*/

/*****************************************************************************/
/*                                                                           */
/*  ReleaseBuf                                                               */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure ReleaseBuf ARGDEF1(char *,buf)
{
  BufHeader *hdr = (BufHeader *)((char*)buf - STATIC_BUFHEADER_SIZE);

  if (totalSizeOnHand + hdr->size >= XDPS_MAX_BUF_BYTES_TO_KEEP
                || bufsInFreeList == XDPS_MAX_BUFS_IN_FREE_LIST)
    sys_free((char *)hdr);
  else
  {
     bufsInFreeList++;
     hdr->prev = NIL;
     hdr->next = freeBufs;

     if (hdr->next != NIL)
        hdr->next->prev = hdr;

     freeBufs = hdr;
     totalSizeOnHand += hdr->size;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  GetBuf                                                                   */
/*                                                                           */
/*****************************************************************************/

PRIVATE char *GetBuf ARGDEF3(char *,src, int, count, integer *,val_ptr)
{
  char *buf;
  BufHeader *cur = freeBufs;
  integer   tmp_count;

  /* If (val_ptr != NIL) this buffer will be used for the out-of-band
     ready-event notification. In this case we assert that count==0
     and we store the val array in the buffer itself (but we still
     set the count to zero).
  */

  while (cur)
  {
     if (cur->size >= count + STATIC_BUFHEADER_SIZE)
     {
        /* remove buffer from free list */

        if (cur->prev == NIL)	/* First element in list */
        {
           freeBufs = cur->next;
  	   if (cur->next)
              cur->next->prev = NIL;
  	}
        else
        {
           cur->prev->next = cur->next;
           if (cur->next)
              cur->next->prev = cur->prev;
  	}

        /* adjust bookkeeping variables */

        totalSizeOnHand -= cur->size;
        bufsInFreeList--;

        goto FILL_BUF;
     }
     cur = cur->next;
  }

  /* We allocate at least STATIC_BUFHEADER_SIZE + VAL_ARRAY_SIZE.
     It is a bit generous, but it removes the need for complicated
     buffer size checking when it is back on the free list. */

  if (count < VAL_ARRAY_SIZE )
     tmp_count = STATIC_BUFHEADER_SIZE + VAL_ARRAY_SIZE;
  else
     tmp_count = count + STATIC_BUFHEADER_SIZE;

  cur = (BufHeader*) sys_malloc (tmp_count);

  if (cur)
  {
     cur->size = tmp_count; /* tmp_count is the real size of the buffer */
     goto FILL_BUF;
  }
  else
     return ((char *)cur);

FILL_BUF:

  buf = ((char *)cur) + STATIC_BUFHEADER_SIZE;

  if (val_ptr)
  {
     Assert (count == 0);
     os_bcopy (val_ptr, buf, VAL_ARRAY_SIZE);
     cur->val_ptr = (integer*) buf;
  }
  else
  {
     os_bcopy(src, buf, count);
     cur->val_ptr = (integer*) NIL;
  }

  return (buf);
}

/*****************************************************************************/
/*                                                                           */
/*  StartContext                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE ContextID StartContext ARGDEF3(PSSpace, space, PDevice, device, char *,start)



  /*  and start a context with a new set of I/O streams.	*/
  {
  Stm in, out;
  PSContext context;

  if (!PSNewContextStms(&in, &out))
    CantHappen();

  context = CreatePSContext(space, in, out, device, start);
  if (context == (PSContext)NULL) return(null_ctxid);
  else return(PSContextToID(context));
  }

/*****************************************************************************/
/*                                                                           */
/*  SimpleOutput                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure SimpleOutput ARGDEF3 (ContextID, ctxid, char *,buf, integer, nChars)

  /* Output procedure supplied to ServicePostScript. Only used	*/
  /* when executing the startup file. It simply dumps output to	*/
  /* stderr.                                                    */
  {
     int nb;

     if (nChars >= 0)
       nb = write(2, buf, (size_t)nChars); /* 2ps_xdps.498 */
  }

/*****************************************************************************/
/*                                                                           */
/*  DoPostScriptCleanup                                                      */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure DoPostScriptCleanup ARGDEF0()
  /* Execute the PS code necessary to cleanup shared VM, Shared	*/
  /* FontDirectory and so on. We do this at server reset time.	*/
  {
  ContextID ctxid;
  PSSpace tmp_space;
  char *cleanupString = XDPS_CONTEXT_CLEANUP_STRING;

  tmp_space = CreatePSSpace();
  ctxid = StartContext(tmp_space, CreateNullDevice(), cleanupString);

  do {
    ServicePostScript(
      SimpleOutput, null_ctxid, (char *)NIL, (integer)0,
      (XdpsReturnProc)NIL);
  } while (IDToPSContext(ctxid) != NIL);

  DestroyPSSpace (tmp_space);
  }

/*****************************************************************************/
/*                                                                           */
/*  ExecuteStmInContext                                                      */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure ExecuteStmInContext ARGDEF1(char *,filename)

  /* Create a temporary space and a context, passing the file	*/
  /* as input.The file is closed when the context goes away.	*/
  {
  ContextID ctxid;
  PSSpace tmp_space;
  char *stmstartupString = XDPS_STM_CONTEXT_STARTUP_STRING;
  char *psString;

  psString = sys_malloc(os_strlen(filename) + os_strlen(stmstartupString) + 3);
  psString[0] = '(';
  psString[1] = (char) 0;
  psString = os_strcat(psString, filename);
  psString = os_strcat(psString, ")");
  psString = os_strcat(psString, stmstartupString);
  tmp_space = CreatePSSpace();
  ctxid = StartContext(tmp_space, CreateNullDevice(), psString);

   do {
    ServicePostScript(
      SimpleOutput, null_ctxid, (char *)NIL, (integer)0,
      (XdpsReturnProc)NIL);
  } while (IDToPSContext(ctxid) != NIL);

  DestroyPSSpace (tmp_space);
  }

/*****************************************************************************/
/*                                                                           */
/*  ExecuteStartupFile                                                       */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure ExecuteStartupFile ARGDEF0()
  /* Look for special startup file and then create a space and	*/
  /* context to execute it.					*/
{
  char *startupFile;

  startupFile = (char *) ResourceStoDevGetFile("InitFile", "default");

  if (startupFile != NIL)
  {
     ExecStartupFile = true;
     ExecuteStmInContext(startupFile);	/* Closes Stm when done	*/
     ExecStartupFile = false;
     sys_free(startupFile);
  } else {
     ExecStartupFile = false;
  }
}

/*--------------------------------*/
/* Scheduler extension procedures */
/*--------------------------------*/

/*****************************************************************************/
/*                                                                           */
/*  EnteringSched                                                            */
/*                                                                           */
/*****************************************************************************/

PRIVATE void EnteringSched ARGDEF1(XdpsReturnProc, retControl)
{
  PRIVATE YieldLocType *yield0, *yield1;

#if (OS == os_aix)

#ifdef AIXV4

   /* For AIX 4.1 and later */
   typedef struct {
       YieldLocType *p1, *p2;
       void (*processInputEvents)();
   } OtherInputs;
 
   extern OtherInputs otherInputs[];
 
   /* TRUC: should 2 button mouse get effected here ? */
   if ((yield0 != (YieldLocType *)otherInputs[0].p1) ||
       (yield1 != (YieldLocType *)otherInputs[0].p2)) {
     yield0 = (YieldLocType *)otherInputs[0].p1;
     yield1 = (YieldLocType *)otherInputs[0].p2;
     PSSetYieldLocations(yield0, yield1);
   }

#else /* AIXV4 */

   /* For AIX 3.2.5 and earlier */
   extern YieldLocType hftPending;
   PRIVATE YieldLocType constantZero = 0;
   if ((yield0 != (&hftPending)) || (yield1 != &constantZero)) {
     yield0 = &hftPending;
     yield1 = &constantZero;
     PSSetYieldLocations(yield0, yield1);
   }

#endif /* AIXV4 */

#else /* (OS == os_aix) */

#if (OS != os_hpux)
   extern YieldLocType *checkForInput[2];
   if ((yield0 != (YieldLocType *)checkForInput[0]) ||
       (yield1 != (YieldLocType *)checkForInput[1])) {
     yield0 = (YieldLocType *)checkForInput[0];
     yield1 = (YieldLocType *)checkForInput[1];
     PSSetYieldLocations(yield0, yield1);
   }
#endif /* (OS != os_hpux) */

#endif /* (OS == os_aix) */

  PSSetYieldChecking((integer)retControl == -1 || (integer)retControl == -2);
}

/*****************************************************************************/
/*                                                                           */
/*  CleanupSchedExt                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE void CleanupSchedExt ARGDEF1(SchedExt *,schedExt)
{
  StatusInfo *cur, *prev;

  if (schedExt->markedForDelete)
    {
    if (schedExt->extClientRecord != (ExtClientRecord *)NIL)
      DeleteClientRec(schedExt->extClientRecord);
    cur = schedExt->clients;
    sys_free((char *)schedExt);
    }
  else cur = schedExt->clients->next;	/* Don't delete the 1st	*/

  prev = NIL;
  while (cur != NIL)
    {
    if (cur->markedForDelete)
      {
      register StatusInfo *next;

      next = cur->next;
      sys_free((char *)cur);
      cur = next;
      if (prev != NIL) prev->next = next;
      /* if (prev == NIL) then we're deleting the first item.	*/
      /* Normally in this case we'd update the list head. But,	*/
      /* if we're deleting the first, we're deleting all. In	*/
      /* that case there is no need to update the list head.	*/
      }
    else {prev = cur; cur = cur->next;}
    }
  }

/*****************************************************************************/
/*                                                                           */
/*  ReportStatusChange                                                       */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure ReportStatusChange ARGDEF3(PSContext, context, SchedExt *,schedExt, Card32, type)
{
  StatusInfo *cur;
  ContextID ctxid =  PSContextToID(context);
  boolean forceDelete = (type == PSZOMBIE);
  Card32 bitForType = maskBitForType[type-1];

  for (cur = schedExt->clients; cur != NIL; cur = cur->next)
    {
    if ((cur->xMode | cur->oneShots) & bitForType)
      (*priv_statusChangeProc)(ctxid, cur->client, type);
    if (cur->oneShots & bitForType)	/* One shot deal...	*/
      {
      cur->oneShots &= ~bitForType;
      if ( (cur->xMode | cur->oneShots) == 0 )
        cur->markedForDelete = true;
      }
    if (forceDelete) cur->markedForDelete = true;
    }
  schedExt->markedForDelete = forceDelete;
}

/*****************************************************************************/
/*                                                                           */
/*  ReportClientXready                                                       */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure ReportClientXready ARGDEF2(PSContext, context, SchedExt *,schedExt)
{
  StatusInfo *cur;
  ContextID ctxid =  PSContextToID(context);

  for (cur = schedExt->clients; cur != NIL; cur = cur->next)
    (*priv_readyProc) (ctxid, cur->client, schedExt->val_array);
}

/*****************************************************************************/
/*                                                                           */
/*  AboutToSchedule                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE void AboutToSchedule ARGDEF3(PSContext, context, char *,sInfo, YieldReason, reason)
{
  SchedExt *schedExt = (SchedExt *)sInfo;

  if (reason == yield_wait || reason == yield_other)
     if (schedExt->extClientRecord != (ExtClientRecord *)NIL)
     schedExt->extClientRecord->blockedCount--;
  }

/*****************************************************************************/
/*                                                                           */
/*  NoteBlockedContext                                                       */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure NoteBlockedContext ARGDEF2(ContextID, ctxid, SchedExt *,schedExt)
{
  if (schedExt->extClientRecord == NIL)
    return;

  schedExt->extClientRecord->blockedCount++;
  /* If the client for this context is already "turned off",	*/
  /* then turn it back on again. Normally when we un-flow-	*/
  /* control a client we do it only if the context that had	*/
  /* overflowed in the first place is now ok. This is an	*/
  /* exception. That context may still be overflowed, but we	*/
  /* want to avoid deadlock so we unblock the client anyway.	*/
  if (AnyClientsWithOverflow)
    XDPSSetOverflow(ctxid, 0);
  }

/*****************************************************************************/
/*                                                                           */
/*  ContextIsPaused (L2-DPS/PROTO 10 addition)                               */
/*                                                                           */
/*****************************************************************************/

PRIVATE boolean ContextIsPaused ARGDEF1(char *, sInfo)
{
   return ( ((SchedExt*)sInfo)->paused_flag == 2 );
}

/*****************************************************************************/
/*                                                                           */
/*  ContextYielded                                                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE void ContextYielded ARGDEF3(PSContext, context, char *,sInfo, YieldReason, reason)
{
  SchedExt *schedExt = (SchedExt *)sInfo;
  ContextID ctxid = PSContextToID(context);

  switch (reason) {
    case yield_terminate:
      {
      schedExt->paused_flag = 0;
      ReportStatusChange(context, schedExt, PSZOMBIE);
      ReportContextDeath(context);
      break;
      }

    case yield_wait:
      {
      NoteBlockedContext(ctxid, schedExt);
      break;
      }

    case yield_other:
      {
         if (schedExt->val_flag)
         {
            /* this is the result of CtxFilBuf or PSClientXready yielding */
            /* Here we just want to report back the ready event, then
               make the context runnable again, so we shouldn't call 
               NoteBlockedContext() like in the PSClientSync case */
            schedExt->val_flag = false;
            PSMakeRunnable(context);
            ReportClientXready (context, schedExt);
         }
         else
         {
            /* this is the result of PSClientSync yielding */
            NoteBlockedContext(ctxid, schedExt);
            ReportStatusChange(context, schedExt, PSFROZEN);
            schedExt->xStatus = PSFROZEN;
         }
         break;
      }

    default:
/*
    case yield_by_request:
    case yield_time_limit:
    case yield_stdout:
    case yield_fflush:
    case yield_stdin:
*/
      {
         if (schedExt->paused_flag == 1)
         {
            /* L2-DPS/PROTO 10 addition */
            schedExt->paused_flag = 2;
            PSRemoveFromReadyList(context);
            NoteBlockedContext(ctxid, schedExt);
            ReportStatusChange(context, schedExt, PSFROZEN);
            schedExt->xStatus = PSFROZEN;
         }
         else
         if ( (reason = yield_stdin) && (schedExt->xStatus != PSNEEDSINPUT) )
         {
           ReportStatusChange(context, schedExt, PSNEEDSINPUT);
           schedExt->xStatus = PSNEEDSINPUT;
         }
         break;
      }
    }

    CleanupSchedExt(schedExt);
  }

/*****************************************************************************/
/*                                                                           */
/*  AddXSchedInfo                                                            */
/*                                                                           */
/*****************************************************************************/

PRIVATE char *AddXSchedInfo ARGDEF0()
{
  SchedExt *s = (SchedExt *)sys_sureMalloc(sizeof(SchedExt));

  s->markedForDelete = false;
  s->xStatus = PSSTATUSERROR;	/* context not real yet */
  s->clients = (StatusInfo *)sys_sureMalloc(sizeof(StatusInfo));

  s->extClientRecord = (ExtClientRecord *)NIL;
    /* doing this indicates that we have not called	*/
    /* NoteContextClientRelationship for this ctxt.	*/
    /* That means that the ctxt was forked, not created	*/
    /* via a Create Protocol request.			*/

  /* The first client in the list is always the owner	*/
  /* DPSSys_CreateContext will note the relationship	*/
  /* between the context and client...			*/
  /* s->clients->client is set by DPSSys_CreateContext	*/
  s->clients->markedForDelete = false;
  s->clients->xMode = 0;
  s->clients->oneShots = 0;
  s->clients->next = (StatusInfo *)NIL;
  s->paused_flag = 0; /* L2-DPS/PROTO 10 addition */
  s->val_flag = false;
  s->val_array[0] = 0;
  s->val_array[1] = 0;
  s->val_array[2] = 0;
  s->val_array[3] = 0;
  return((char *)s);
  }

/*****************************************************************************/
/*                                                                           */
/*  FlowControlReleaseInput                                                  */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure FlowControlReleaseInput ARGDEF3(PSContext, context, char *, buf, integer, count)
{
    register longinteger qTotal;

    if (buf != NIL)
        {
        ReleaseBuf(buf);
        if (AnyClientsWithOverflow && context != NIL && count > 0)
            {
	    ContextID ctxid = PSContextToID(context);
            register Stm in = context->in;
	    if ( (ctxid == GetClient(context)->whichCtxtOverran)
            && (in != NIL && (qTotal = favail(in)) > 0))
                {
	        if (qTotal <= XDPS_CONTEXT_INPUT_HIGH_LEVEL)
		    XDPSSetOverflow(ctxid, 0);
	        }
            }
        }
}

/*-------------------*/
/* Public procedures */
/*-------------------*/

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_SetupSchedExtVal                                                  */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure DPSSys_SetupSchedExtVal ARGDEF1 (integer *,val_ptr)
{
  /*-------------------------------------------------------------*/
  /* Called from PSClientXready() and PSClientSync in xdpsops.c. */
  /* Also called from CtxFilBuf() in contextstm.c                */
  /* Sets up the val_flag and val_array in the SchedExt record.  */
  /* PSClientSync must pass a NULL pointer.                      */
  /* PSClientXready must pass a non-NULL pointer.                */
  /* CtxFilBuf must pass a non-NULL pointer.                     */
  /*-------------------------------------------------------------*/

  SchedExt *schedExt;

  schedExt = (SchedExt *)GetSchedExt(currentPSContext);

  if (val_ptr)
  {
     schedExt->val_flag = true;
     schedExt->val_array[0] = val_ptr[0];
     schedExt->val_array[1] = val_ptr[1];
     schedExt->val_array[2] = val_ptr[2];
     schedExt->val_array[3] = val_ptr[3];
  }
  else
  {
     schedExt->val_flag = false;
     schedExt->val_array[0] = 0;
     schedExt->val_array[1] = 0;
     schedExt->val_array[2] = 0;
     schedExt->val_array[3] = 0;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_ValidContext                                                      */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean DPSSys_ValidContext ARGDEF1(DPSSysCID, dps_ctxid)
{
  if (IDToPSContext((ContextID)dps_ctxid) == NULL)
     return(false);
  else
     return(true);
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_DestroySpace                                                      */
/*                                                                           */
/*****************************************************************************/

PUBLIC boolean DPSSys_DestroySpace ARGDEF2(DPSSysSID, dps_spid, XdpsOutputProc, outputProc)

                     /* DPSSysCID dps_ctxid, unsigned char *buf, int count */  
  /* Destroy the specified space if it still exists. Return true */
  /* upon successful completion, false otherwise.		 */
  {
  SpaceID spid;
  PSSpace dead_space;
  ContextID dummy_ctxid;

  spid = dps_spid;
  dead_space = IDToPSSpace(spid);

  if (dead_space == NIL) return(false);

  TerminatePSSpace(dead_space);

  /* Keep scheduling contexts until our target is dead */
  dummy_ctxid = 0;
  while (!DestroyPSSpace(dead_space))
    ServicePostScript(outputProc, dummy_ctxid, NIL, 0, ALWAYSRETURN);

  return(true);
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_CreateSpace                                                       */
/*                                                                           */
/*****************************************************************************/

PUBLIC DPSSysSID DPSSys_CreateSpace ARGDEF0()
{
  PSSpace space = NIL;
  SpaceID spid;

  space = CreatePSSpace();
  if (space == NIL) return(0);	/* CreateSpace failed */
  else
    {
    spid = PSSpaceToID(space);
    return(spid);
    }
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_CreateContextFromID                                               */
/*                                                                           */
/*****************************************************************************/

PUBLIC void DPSSys_CreateContextFromID ARGDEF2(DPSSysCID, dps_ctxid, char *,client)
{
  ContextID ctxid;

  ctxid = dps_ctxid;
  NoteContextClientRelationship(ctxid, client);
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_CreateContext                                                     */
/*                                                                           */
/*****************************************************************************/

PUBLIC DPSSysCID DPSSys_CreateContext ARGDEF4(char *,client, DPSSysSID *,Pdps_spid, char *,device, boolean, secure)




  /* Create a context in specified space with the given device.	*/
  /* If space is NIL, create a new space.			*/
  {
  PSSpace space = NIL;
  SpaceID spid;
  ContextID ctxid;
  char *startupString = XDPS_CONTEXT_STARTUP_STRING; 

  spid = *Pdps_spid;

  if (spid == 0)	/* Must create a space			*/
    {
    if (secure)
      space = Create2PSSpace(NIL, true);
    else
      space = CreatePSSpace();
    if (space == NIL) return(0); /* error */
    else
      {
      spid = PSSpaceToID(space);
      *Pdps_spid = spid;
      }
    }
  else space = IDToPSSpace(spid);

  ctxid = StartContext(space, (PDevice)device, startupString);
  if (ctxid != 0)
    {
    NoteContextClientRelationship(ctxid, client);
    return(ctxid);
    }

  /* Failure creating context, if we created a space, free it.	*/
  if (space != NIL) DestroyPSSpace(space);
  return((DPSSysCID)0);
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_GiveInput                                                         */
/*                                                                           */
/*****************************************************************************/

PUBLIC integer DPSSys_GiveInput ARGDEF4(XdpsOutputProc, outProc, DPSSysCID, dps_ctxid, char *,buf, short unsigned int, count)
                  /* DPSSysCID dps_ctxid; unsigned char *buf; int count */  



  /* Pass the input buffer to the specified context. Empty bufs	*/
  /* and NIL contexts are both ok and have semantics defined in	*/
  /* server.h         			*/
  {
  int stat;
  ContextID ctxid;
  register integer ct = count;
  char *newBuf;
  PSContext context;
  SchedExt *schedExt = NIL;

  ctxid = dps_ctxid;
  if ((context = IDToPSContext(ctxid)) != NIL)
    schedExt = (SchedExt *)GetSchedExt(context);
  if (buf != NIL)
    {
    /* give the DPS kernel a copy of the buffer */
    if ( (newBuf = GetBuf(buf, ct, NIL)) == NIL)
      return(0); /* error */

    /* if the context is currently waiting for input, we're about
       to make it run. May need to generate a PSRUNNING status event. */
    if (context != NIL) {
      if (schedExt->xStatus == PSNEEDSINPUT) {
        ReportStatusChange(context, schedExt, PSRUNNING);
        schedExt->xStatus = PSRUNNING;
	}
      }
    }
  else newBuf = NIL;

  /* Deal with flow control	*/
  {
  register Stm in;
  if (context != NIL && (in = context->in) != NIL)
    {
    if (!schedExt->extClientRecord->blockedCount && favail(in) > XDPS_CONTEXT_INPUT_HIGH_LEVEL)
        {
	schedExt->extClientRecord->whichCtxtOverran = ctxid;
	XDPSSetOverflow(ctxid, 1);
	}
    }
  }

  stat = ServicePostScript(outProc, ctxid, newBuf, ct, gReturnControl);
  return(!stat);
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_RegisterReturnProc                                                */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure DPSSys_RegisterReturnProc ARGDEF1(XdpsReturnProc, returnControl)

  /* Set the procedure to be passed to ServicePostScript as the	*/
  /* returnControl procedure.					*/
  {
  gReturnControl = returnControl;
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_NotifyContext                                                     */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure DPSSys_NotifyContext ARGDEF2(DPSSysCID, dps_ctxid, int, notifyType)


  /* Send a notification to the specified context.		*/
  {
  PSContext context;
  ContextID ctxid;
  SchedExt *schedExt;

  ctxid = dps_ctxid;
  context = IDToPSContext(ctxid);

  if (context == NIL) return;
  schedExt = (SchedExt *)GetSchedExt(context);
  switch (notifyType) {
    case PSINTERRUPT:
      schedExt->paused_flag = 0;
      NotifyPSContext(context, notify_interrupt);
      PSMakeRunnable(context);
      break;
    case PSKILL:
      schedExt->paused_flag = 0;
      NotifyPSContext(context, notify_terminate);
      PSMakeRunnable(context);
      break;
    case PSUNFREEZE:
      {
      schedExt->paused_flag = 0;
      PSMakeRunnable(context);
      ReportStatusChange(context, schedExt, PSRUNNING);
      schedExt->xStatus = PSRUNNING;
      break;
      }
    case PSFREEZE:
      {
      /* L2-DPS/PROTO 10 addition */
      if (schedExt->paused_flag > 0)
      {
         ReportStatusChange(context, schedExt, PSFROZEN);
         schedExt->xStatus = PSFROZEN;
      }
      else
      {
         schedExt->paused_flag = 1;
         PSMakeRunnable(context);
      }
      break;
      }
    default:;
    }
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_NotifyWhenReady                                                   */
/*                                                                           */
/*****************************************************************************/

PUBLIC void DPSSys_NotifyWhenReady ARGDEF3 (XdpsOutputProc, outProc, DPSSysCID, dps_ctxid, integer *,val)
                  /* DPSSysCID dps_ctxid; unsigned char *buf; int count */  


{
  int stat;
  ContextID ctxid;
  register integer ct = 0;
  char *newBuf;
  PSContext context;
  SchedExt *schedExt;
  char buf[1]; /* dummy */

  ctxid = dps_ctxid;
  if ((context = IDToPSContext(ctxid)) != NIL)
    schedExt = (SchedExt *)GetSchedExt(context);
  if (buf != NIL)
    {
    /* give the DPS kernel a copy of the buffer */
    if ( (newBuf = GetBuf(buf, ct, val)) == NIL)
      return; /* error */
    }
  else newBuf = NIL;

  stat = ServicePostScript(outProc, ctxid, newBuf, ct, gReturnControl);
}

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_ResetContext                                                      */
/*                                                                           */
/*****************************************************************************/

PUBLIC integer DPSSys_ResetContext ARGDEF2(DPSSysCID, dps_ctxid, XdpsOutputProc, outProc)

                  /* DPSSysCID dps_ctxid; unsigned char *buf; int count */  
  /* Reset the specified context.			*/
  {
  PSContext context;
  ContextID ctxid;

  ctxid = dps_ctxid;
  context = IDToPSContext(ctxid);
  if (context == NIL) return(0);

  ServicePostScript(outProc, ctxid, (char *)NIL, 0, gReturnControl); /* EOF */
  return(1);
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_ContextIDtoSpaceID                                                */
/*                                                                           */
/*****************************************************************************/

PUBLIC DPSSysSID DPSSys_ContextIDtoSpaceID ARGDEF1(DPSSysCID, dps_ctxid)
{
  ContextID ctxid;

  ctxid = dps_ctxid;

  return(PSSpaceToID(IDToPSContext(ctxid)->space));
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_SetStatusMask                                                     */
/*                                                                           */
/*****************************************************************************/

PUBLIC void DPSSys_SetStatusMask ARGDEF5(DPSSysCID, dps_ctxid, char *,client, int, set, int, unset, int, oneShot)
{
  StatusInfo *entry;
  SchedExt *schedExt;
  PSContext context;
  ContextID ctxid;

  ctxid = dps_ctxid;
  context = IDToPSContext(ctxid);

  schedExt = (SchedExt *)GetSchedExt(context);

  /* Find the StatusInfo record for this client (if exists)	*/
  for (entry = schedExt->clients; entry->next != NIL; entry = entry->next)
    if (entry->client == client) break;

  /* At this point entry either points at the entry for this	*/
  /* client, or it is the last entry in the list.		*/

  if (entry->client == client)
    /* There is a record for this client, set the mode.		*/
    {
    entry->xMode |= set; entry->xMode &= ~(unset | oneShot);
    entry->oneShots |= oneShot;
    if ((entry->xMode | entry->oneShots) == 0) entry->markedForDelete = true;
      /* If their is no interest, mark for deletion		*/
    }
  else	/* No entry for this client, add one			*/
    {
    if (set | oneShot) /* Don't add a record for no interests!	*/
      {
      StatusInfo *new;
      new = (StatusInfo *)sys_malloc(sizeof(StatusInfo));
      new->markedForDelete = false;
      new->xMode = set;
      new->oneShots = oneShot;
      new->next = (StatusInfo *)NIL;
      entry->next = new;
        /* entry points at the last entry, remember?		*/
      }
    }

  CleanupSchedExt(schedExt);
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_GetStatus                                                         */
/*                                                                           */
/*****************************************************************************/

PUBLIC integer DPSSys_GetStatus ARGDEF1(DPSSysCID, dps_ctxid)
{
  ContextID ctxid;
  PSContext context;

  ctxid = dps_ctxid;
  context = IDToPSContext(ctxid);
  if (context == NULL) return(PSZOMBIE);
  return(((SchedExt *)GetSchedExt(context))->xStatus);   
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_Restart                                                           */
/*                                                                           */
/*****************************************************************************/

PUBLIC void DPSSys_Restart ARGDEF0()
{
  XDPS_InitSupport();
  }

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_Reset                                                             */
/*                                                                           */
/*****************************************************************************/

PUBLIC void DPSSys_Reset ARGDEF0()
{
/*PSFlushFontCache(); +++ */
  DoPostScriptCleanup();
  XDPS_Reset();
  XDPS_genxsupport_Reset();
  ResetXdpsMarkerImpl();
  }

/*****************************************************************************/
/*                                                                           */
/*  priv_RegisterPackageInits                                                */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure priv_RegisterPackageInits ARGDEF0()
{
   RegisterFiltersInit();
   DeviceInit();
}

/*****************************************************************************/
/*                                                                           */
/*  DPSSys_Init                                                              */
/*                                                                           */
/*****************************************************************************/

PUBLIC integer DPSSys_Init (XdpsStatusProc  statusProc,
                            XdpsDeathProc   deathProc,
                            XdpsReadyProc   readyProc,
                            int            *xDPSReady_ptr)
{
  PostScriptParameters psParameters;
  SchedulerProcs schedulerProcs;
  Stm vmStm;
  boolean status;

  extern Stm XDPS_ProductInit ARGDECL0(); /* in products/adobe/xdps/sources/xdps_product.c */

  DebugAssert(sizeof(longinteger) == MINALIGN); /* ensure proper _t_BufHeader alignment */
  DebugAssert(os_cacheLineSize <= 1);           /* ensure memalign is not called */

  vmStm = XDPS_ProductInit();

  if (vmStm == NIL)
     return (0); /* error */

  freeBufs         = NIL;
  totalSizeOnHand  = 0;
  bufsInFreeList   = 0;
  headOfClientList = NIL;

  priv_readyProc        = readyProc;
  priv_statusChangeProc = statusProc;
  priv_ctxtDeathProc    = deathProc;

  CreateScheduler (AddXSchedInfo,
                   (EnterSchedulerProc) EnteringSched,
                   (SchedulerProc) AboutToSchedule,
                   (SchedulerProc) ContextYielded,
                   (YieldLocType *)NIL,
                   (YieldLocType *)NIL,
                   (unsignedlonginteger *)NIL,
                   xDPSReady_ptr);

  os_bzero((char *)&psParameters, (long int)sizeof(PostScriptParameters));

  psParameters.errStm                 = os_stderr;	/* Use standard stream */
  psParameters.vmStm                  = vmStm;		/* VM file */
  psParameters.pType1BCRenderProcs    = ATMOnlyInit();
#ifdef TRUETYPE_FONTS
  psParameters.pTrueTypeBCRenderProcs = TrueTypeInit();
#else
  psParameters.pTrueTypeBCRenderProcs = NULL;
#endif
  psParameters.customProc             = XDPS_RegisterXcustomOps;
  psParameters.realClock              = NIL;		/* "execution unit" clock */
  psParameters.registerPackageInit    = priv_RegisterPackageInits;
  psParameters.pVMBuildArgs           = NIL;

  schedulerProcs.yieldProc                  = PSYield;
  schedulerProcs.makeRunnableProc           = PSMakeRunnable;
  schedulerProcs.createSchedulerContextProc = CreateSchedulerContext;
  schedulerProcs.newContextStms             = PSNewContextStms;
  psParameters.schedulerProcs = &schedulerProcs;

  InitPostScript(&psParameters);

  status = DPSSys_IniDevImpl();
  if (!status)
     return (0); /* error */

  InitXdpsMarkerImpl();

  RegisterSchedExtCallback      ( (SchedulerCallbackProc) DPSSys_SetupSchedExtVal);
  RegisterContextPausedCallback ( (PausedCallbackProc) ContextIsPaused);
  SetCtxStmInputReleaseProc     (FlowControlReleaseInput);

  ExecuteStartupFile();
  null_ctxid = 0;
  return(1); /* ok */
}
