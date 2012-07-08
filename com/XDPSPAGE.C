/* @(#)CM_VerSion xdpspage.c 3011 1.2 21473.eco sum= 01484 3011.002 */
/* @(#)CM_VerSion xdpspage.c 3010 1.4 19928.eco sum= 31174 3010.102 */
/* @(#)CM_VerSion xdpspage.c 2017 1.2 17334.eco sum= 40924 2017.011 */
/* @(#)CM_VerSion xdpspage.c 2016 1.2 14237.eco sum= 27795 */
/* @(#)CM_VerSion xdpspagedevice.c 2015 1.6 12669.eco sum= 11808 */
/* @(#)CM_VerSion xdpspagedevice.c 2014 1.3 08466.eco sum= 62378 */
/* @(#)CM_VerSion xdpspagedevice.c 2013 1.2 06671.eco sum= 30484 */
/*
  xdpspage.c

(c) Copyright 1993, 1995 Adobe Systems Incorporated.
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

/*****************************************************************************/
/*                                                                           */
/*  xdpspage.c - implements support for a page device created when the       */
/*               current device is a window device or a null device.         */
/*                                                                           */
/*  NOTE: most of the ODM code was stolen from products/adobe/sps/sources    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  This module contains one public function (RegisterXDPSPageDevice) which  */
/*  is exported through the public interface XDPS_SUPPORT                    */
/*                                                                           */
/*****************************************************************************/

/*-----------------------------------------*/
/* Public interfaces from PostScript world */
/*-----------------------------------------*/

#include PACKAGE_SPECS
#include ENVIRONMENT
#include PUBLICTYPES
#include XDPS_SUPPORT
#include POSTSCRIPT
#include OUTPUTDEVICE
#include DEVPUBLIC
#include EXCEPT
#include ODMDICT
#include ODMKEYS
#include UTIL

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "xdevice.h"

/*-----------*/
/* constants */
/*-----------*/

#define GetAtom( name ) \
   ODMAtomCreate( name, os_strlen( name ), true )

#define GetPageSize( odp, reqX, reqY, actX, actY ) \
   GetODMIntArray4( odp, PageSize_Atom(), reqX, reqY, actX, actY )

#define GetHWResolution( odp, reqX, reqY, actX, actY ) \
   GetODMIntArray4( odp, HWResolution_Atom(), reqX, reqY, actX, actY )

#define PutPageSize( odp, reqX, reqY ) \
   PutODMRealArray4( odp, PageSize_Atom(), 5, reqX, reqY, reqX, reqY )

#define PutHWResolution( odp, reqX, reqY ) \
   PutODMIntArray4( odp, HWResolution_Atom(), 4, reqX, reqY, reqX, reqY )

#define DO_NOT_SET (-9999.0) /* fix for 2ps_xdps.431 */

/*------------------------*/
/* ODM retrieve functions */
/*------------------------*/

PRIVATE boolean GetODMIntArray ( ODMDictionary  odp,
                                 ODMAtom        atom,
                                 integer        num,
                                 integer       *retVal )
{
   ODMValueRec  value;
   ODMArray     array;
   integer      i, arrLen;

   if( !ODMDictGet( odp, atom, &value )  ||
        value.kind != ODMVALUE_ARRAY_KIND  ||
        (array = value.val.arrayVal) == NIL )
      return( false );

   arrLen = ODMArrayLength( array );
   if( num > arrLen )  num = arrLen;
   for( i=0; i<num; ++i ) {
      ODMArrayGet( array, i, &value );
      retVal[i] = ( value.kind == ODMVALUE_INT_KIND ) ?
                  value.val.intVal : (int)(value.val.realVal+0.5);
   }
   return( true );
}

PRIVATE boolean GetODMIntArray4 ( ODMDictionary  odp,
                                  ODMAtom        atom,
                                  integer       *reqX,
                                  integer       *reqY,
                                  integer       *actX,
                                  integer       *actY )
{
   integer  iVal[4];

   if( !GetODMIntArray( odp, atom, 4, iVal ) )
      return( false );
   *reqX = iVal[REQUESTX];
   *reqY = iVal[REQUESTY];
   *actX = iVal[ ACTUALX];
   *actY = iVal[ ACTUALY];
   return( true );
}

/*-------------------------*/
/* ODM insertion functions */
/*-------------------------*/

PRIVATE procedure PutODMValue ( ODMDictionary   dict,
                                ODMAtom         key,
                                ODMValue        value )
{
   DURING
      ODMDictPut( dict, key, value );
   HANDLER
      ODMValueDestroy( value );
      RERAISE;
   END_HANDLER
}

PRIVATE procedure PutODMNull ( ODMDictionary  odp,
                               ODMAtom        atom )
{
   ODMValueRec  value;
   value.kind = ODMVALUE_NULL_KIND;
   ODMDictPut( odp, atom, &value );
}

PRIVATE procedure PutODMAtom ( ODMDictionary  odp,
                               ODMAtom        atom,
                               ODMAtom        atomVal )
{
   ODMValueRec  value;
   value.kind = ODMVALUE_ATOM_KIND;
   value.val.atomVal = atomVal;
   PutODMValue( odp, atom, &value );
}

PRIVATE procedure PutODMDict ( ODMDictionary  odp,
                               ODMAtom        atom,
                               ODMDictionary  dictVal )
{
   ODMValueRec  value;
   value.kind = ODMVALUE_DICT_KIND;
   value.val.dictVal = dictVal;
   PutODMValue( odp, atom, &value );
}

PRIVATE procedure PutODMRealArray ( ODMDictionary  odp,
                                    ODMAtom        atom,
                                    integer        num,
                                    integer        arrLen,
                                    real          *rVals )
{
   ODMValueRec  value;
   ODMArray     array;
   integer      i;

   if( num > arrLen )  arrLen = num;
   array = ODMArrayCreate( arrLen );
   value.kind = ODMVALUE_REAL_KIND;
   for( i=0; i<num; ++i ) {
      if( rVals[i] == DO_NOT_SET ) continue;
      value.val.realVal = rVals[i];
      ODMArrayPut( array, i, &value );
   }
   value.kind = ODMVALUE_ARRAY_KIND;
   value.val.arrayVal = array;
   PutODMValue( odp, atom, &value );
}

PRIVATE procedure PutODMRealArray4 ( ODMDictionary  odp,
                                     ODMAtom        atom,
                                     integer        arrSize,
                                     real           reqX,
                                     real           reqY,
                                     real           actX,
                                     real           actY )
{
   real  rVals[5];

   rVals[REQUESTX] = reqX;
   rVals[REQUESTY] = reqY;
   rVals[ ACTUALX] = actX;
   rVals[ ACTUALY] = actY;
   PutODMRealArray( odp, atom, 4, arrSize, rVals );
}

PRIVATE procedure PutODMIntArray ( ODMDictionary  odp,
                                   ODMAtom        atom,
                                   integer        num,
                                   integer        arrLen,
                                   integer       *iVals )
{
   ODMValueRec  value;
   ODMArray     array;
   integer      i;

   if( num > arrLen )  arrLen = num;
   array = ODMArrayCreate( arrLen );
   value.kind = ODMVALUE_INT_KIND;
   for( i=0; i<num; ++i ) {
      if( iVals[i] == DO_NOT_SET ) continue;
      value.val.intVal = iVals[i];
      ODMArrayPut( array, i, &value );
   }
   value.kind = ODMVALUE_ARRAY_KIND;
   value.val.arrayVal = array;
   PutODMValue( odp, atom, &value );
}

PRIVATE procedure PutODMIntArray4 ( ODMDictionary  odp,
                                    ODMAtom        atom,
                                    integer        arrSize,
                                    integer        reqX,
                                    integer        reqY,
                                    integer        actX,
                                    integer        actY )
{
   integer  iVals[5];

   iVals[REQUESTX] = reqX;
   iVals[REQUESTY] = reqY;
   iVals[ ACTUALX] = actX;
   iVals[ ACTUALY] = actY;
   PutODMIntArray( odp, atom, 4, arrSize, iVals );
}

#define	DPS_PPI	72.0

PRIVATE ODMDictionary DPS_ODMDict;

/*****************************************************************************/
/*                                                                           */
/*  DPS_CreateODMDict                                                        */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure DPS_CreateODMDict (OutputDevice   outputDevice,
                                     ODMDictionary *odp)
{
	ODMDictionary fullDict;
	ODMDictionary devDict;
	ODMAtom devAtom;
	char *name;

	DURING
		name = outputDevice->name;
		if (!(fullDict = *odp)) {
			*odp = fullDict = ODMDictCreate();
			devAtom = GetAtom(name);
			PutODMAtom(fullDict, DefaultDevice_Atom(), devAtom);
		}
		else
			devAtom = GetAtom(name);
		devDict = ODMDictCreate();
		PutODMAtom(devDict, OutputDevice_Atom(), devAtom);
		PutODMNull(devDict, InputAttributes_Atom());
		PutODMNull(devDict, ImagingBBox_Atom());
		PutODMNull(devDict, MediaType_Atom());
		PutODMNull(devDict, MediaColor_Atom());
		PutODMNull(devDict, MediaWeight_Atom());
		PutODMNull(devDict, MediaPosition_Atom());
		PutPageSize(devDict, 0, 0);
		PutHWResolution(devDict, 75, 75);
		PutODMDict(fullDict, devAtom, devDict);
	HANDLER
		CantHappen();
	END_HANDLER
}

/*****************************************************************************/
/*                                                                           */
/*  DPS_CreatePSDevice                                                       */
/*                                                                           */
/*****************************************************************************/

PRIVATE PDevice DPS_CreatePSDevice (PrintInstructionsDict pi)
{
	return DPSSys_CloneDevice();
}

/*****************************************************************************/
/*                                                                           */
/*  DPS_getOutDevParams                                                      */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure DPS_getOutDevParams (OutputDevice outputDevice,
                                       ODMDictionary odp,
                                       GetODPReason reason)
{
	integer xRes, yRes, pixWd, pixHt;

	/* Get dimensions and resolution of current window device */
	DPSSys_GetDeviceSize(&pixWd, &pixHt);
	DPSSys_GetDeviceResolution(&xRes, &yRes);
	PutPageSize(odp, pixWd * DPS_PPI / xRes, pixHt * DPS_PPI / yRes);
	PutHWResolution(odp, xRes, yRes);
}

/*****************************************************************************/
/*                                                                           */
/*  DPS_outDevParamsOk                                                       */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure DPS_outDevParamsOk (OutputDevice    outputDevice,
                                      ODMDictionary   odp,
                                      boolean         mediaMatching,
                                      RejectValProc   rejectVal,
                                      void           *rejectValHook)
{
	ODMAtom atom;
	ODMValueRec value;
	integer xRes, yRes, pixWd, pixHt;
	integer ptWd, ptHt;
	integer reqWd, reqHt, actWd, actHt;
	integer reqXRes, reqYRes, actXRes, actYRes;

	/* Get dimensions and resolution of current window device */

	DPSSys_GetDeviceSize(&pixWd, &pixHt);
	DPSSys_GetDeviceResolution(&xRes, &yRes);

	/*
	 * The PageSize values must match the actual window size; see Red
	 * book section 4.11.3, page 233 for tolerance matching details (5
	 * default user space units).
	 */
	ptWd = pixWd * DPS_PPI / xRes;
	ptHt = pixHt * DPS_PPI / yRes;

	atom = PageSize_Atom();
	GetPageSize(odp, &reqWd, &reqHt, &actWd, &actHt);
	if (reqWd + 5 < ptWd ||
		reqWd - 5 > ptWd ||
		reqHt + 5 < ptHt ||
		reqHt - 5 > ptHt)
		(*rejectVal)(atom, rejectValHook);

	/*
	 * The HWResolution values must match the HW device resolution
	 */
	atom = HWResolution_Atom();
	GetHWResolution(odp, &reqXRes, &reqYRes, &actXRes, &actYRes);
	if (xRes != reqXRes ||
		yRes != reqYRes)
		(*rejectVal)(atom, rejectValHook);

	/*
	 * Values for the rest are fixed to the defaults from
	 * OutputDevicesDict
	 */
	if (ODMDictGet(odp, atom = MediaType_Atom(), &value) &&
		value.kind != ODMVALUE_NULL_KIND)
		(*rejectVal)(atom, rejectValHook);

	if (ODMDictGet(odp, atom = MediaColor_Atom(), &value) &&
		value.kind != ODMVALUE_NULL_KIND)
		(*rejectVal)(atom, rejectValHook);

	if (ODMDictGet(odp, atom = MediaWeight_Atom(), &value) &&
		value.kind != ODMVALUE_NULL_KIND)
		(*rejectVal)(atom, rejectValHook);
}

/*****************************************************************************/
/*                                                                           */
/*  DPS_createPageSink -                                                     */
/*                                                                           */
/*****************************************************************************/

PRIVATE PPageSink DPS_createPageSink (PrintInstructionsDict pi)
{
	return 0;
}

/*****************************************************************************/
/*                                                                           */
/*  DPS_OutputDeviceProcsRec - output device callbacks                       */
/*                                                                           */
/*****************************************************************************/

PRIVATE OutputDeviceProcsRec DPS_OutputDeviceProcsRec = {
	/* setOutDevParams: not used because we do not
	 * have non-volatile parameter storage */
	0,

	/* getOutDevParams */
	DPS_getOutDevParams,

	/* getSlotHWAttributes: not used because it "is never called
	 * in products with deferred binding of input attributes" */
	0,

	/* outDevParamsOk */
	DPS_outDevParamsOk,

	/* getOdpConfirmation: not used "because window devices don't
	 * support Policy 2 (user confirmation)" */
	0,

	/* createPageSink */
	DPS_createPageSink,
};

/*****************************************************************************/
/*                                                                           */
/*  DPS_OutputDeviceRec - output device descriptor                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE OutputDeviceRec DPS_OutputDeviceRec = {
	"Window",			/* name */
	&DPS_OutputDeviceProcsRec,	/* procs */
};

/*****************************************************************************/
/*                                                                           */
/*  RegisterXDPSPageDevice - output device registration procedure            */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure RegisterXDPSPageDevice (void)
{
	PageDeviceInit(DPS_CreatePSDevice);

	DPS_CreateODMDict(&DPS_OutputDeviceRec, &DPS_ODMDict);
	ODM_Init(DPS_ODMDict, 0);
	ODM_RgstOutputDevice(&DPS_OutputDeviceRec);
}
