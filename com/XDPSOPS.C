/* @(#)CM_VerSion xdpsops.c 3010 1.6 19928.eco sum= 45700 3010.102 */
/* @(#)CM_VerSion xdpsops.c 2017 1.4 17841.eco sum= 53930 2017.013 */
/* @(#)CM_VerSion xdpsops.c 2015 1.8 12669.eco sum= 29652 */
/* @(#)CM_VerSion xdpsops.c 2014 1.4 08466.eco sum= 02723 */
/* @(#)CM_VerSion xdpsops.c 2013 1.5 07859.eco sum= 57437 */
/*
  xdpsops.c

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

/*
*******************************************************************************
*                                                                             *
*  File: xdpsops.c                                                            *
*                                                                             *
*******************************************************************************
*                                                                             *
*  The PUBLIC procedures in this file are declared extern in xdpsops.h        *
*                                                                             *
*  This file implements the X/DPS custom operators.                           *
*                                                                             *
*  - This module implements the DPS/X custom operators. It contains the       *
*    proc that registers the operators with DPS, and the procs that are       *
*    called when one of the operators is invoked.                             *
*    Almost all of the procedures in this module simply do some checking      *
*    and then call one or more XDPSMumble routines to do the real work.       *
*    The clientsync operator is implemented here and uses the yield_other     *
*    yield_reason.                                                            *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
*  PRIVATE PROCEDURES:                                                        *
*                                                                             *
*    PSSetXGCDrawable                                                         *
*    PSSetXGCDrawableColor                                                    *
*    PSCurrentXGCDrawable                                                     *
*    PSCurrentXGCDrawableColor                                                *
*    PSCurrentXOffset                                                         *
*    PSSetXOffset                                                             *
*    PSSetXRGBActual                                                          *
*    PSClientSync                                                             *
*    PSSetXDrawingFunction                                                    *
*    PSCurrentXDrawingFunction                                                *
*    PSClientXready                                                           *
*                                                                             *
*  PUBLIC PROCEDURES:                                                         *
*                                                                             *
*    XDPS_RegisterXOps                                                        *
*                                                                             *
*  PUBLIC VARIABLES:                                                          *
*                                                                             *
*    <list>                                                                   *
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
#include DEVPUBLIC
#include SERVER

/*---------------------*/
/* Local include files */
/*---------------------*/

#include "xdpsops.h"
#include "dpssys.h"
#include "actual.h"
#include "genxsupport.h"

/*--------------------*/
/* PRIVATE procedures */
/*--------------------*/

/*****************************************************************************/
/*                                                                           */
/*  PSSetXGCDrawable                                                         */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure PSSetXGCDrawable()
  /*	gc drawable x y    setXgcdrawable    -		*/
{
  long int gc, drawable, x, y;
  PDevice dev;
  PDevice oldDev;

  y = PSPopInteger();
  x = PSPopInteger();
  drawable = PSPopInteger();
  gc = PSPopInteger();
  oldDev = PSGetDevice((PPSGState) NULL); /* get current device */
  dev = XDPS_SetXGCDrawable(oldDev, gc, drawable, x, y);
  if (dev != (PDevice) NULL)
    PSSetDevice(dev, 0);
}

/*****************************************************************************/
/*                                                                           */
/*  PSSetXGCDrawableColor                                                    */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure PSSetXGCDrawableColor()
  /*	gc drawable x y colorinfo   setXgcdrawablecolor    -	*/
{
  integer i;
  long int gc, drawable, x, y;
  PDevice dev, oldDev;
  PSObject color;
  long unsigned int ci[12];

  PSPopTempObject(dpsArrayObj, &color);
  /* Validate the color info array. It must either be 12 entries long
     (for a full colormap and grayramp) or 3 entries long (for just a 
     grayramp). */
  if (color.length != 12 && color.length != 3)
    PSRangeCheck();

  for (i = 0; i < (integer)color.length; i++)
    ci[i] = color.val.arrayval[i].val.ival;
  if (color.length == 3)
    for (i = 3; i < 12; i++)
      ci[i] = 0;

  y = PSPopInteger();
  x = PSPopInteger();
  drawable = PSPopInteger();
  gc = PSPopInteger();

  oldDev = PSGetDevice((PPSGState) NULL); /* get current device */
  dev = XDPS_SetXGCDrawableColor(oldDev, gc, drawable, x, y, ci);
  if (dev != (PDevice) NULL)
    PSSetDevice(dev, 0);
}

/*****************************************************************************/
/*                                                                           */
/*  PSCurrentXGCDrawable                                                     */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure PSCurrentXGCDrawable()
  /*	-    currentXgcdrawable    gc drawable x y	*/
{
  DevLong  x, y;
  long int gc, drawable;
  PDevice dev;

  dev = PSGetDevice((PPSGState) NULL); /* get current device */
  XDPS_CurrentXGCDrawable(dev, &gc, &drawable, &x, &y);
  PSPushInteger(gc);
  PSPushInteger(drawable);
  PSPushInteger(x);
  PSPushInteger(y);
}

/*****************************************************************************/
/*                                                                           */
/*  PSCurrentXGCDrawableColor                                                */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure PSCurrentXGCDrawableColor()
  /*	-    currentXgcdrawablecolor    gc drawable x y colorinfo	*/
{
  integer i;
  DevLong x, y;
  long int gc, drawable;
  PDevice dev;
  long unsigned int ci[12];

  dev = PSGetDevice((PPSGState) NULL); /* get current device */
  XDPS_CurrentXGCDrawableColor(dev, &gc, &drawable, &x, &y, ci);
  PSPushInteger(gc);
  PSPushInteger(drawable);
  PSPushInteger(x);
  PSPushInteger(y);
  for (i = 0; i < 12; i++)
    PSPushInteger(ci[i]);
  if (PSExecuteString("12 array astore"))
    PSHandleExecError();
}

/*****************************************************************************/
/*                                                                           */
/*  PSCurrentXOffset                                                         */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure PSCurrentXOffset()
  /*	-    currentXoffset    x y	*/
{
  DevLong  x, y;
  PDevice dev;
  dev = PSGetDevice((PPSGState) NULL); /* get current device */
  XDPS_CurrentXOffset(dev, &x, &y);
  PSPushInteger(x);
  PSPushInteger(y);
}

/*****************************************************************************/
/*                                                                           */
/*  PSSetXOffset                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure PSSetXOffset()
  /*	x y    setXoffset    -		*/
{
  long int x, y;
  PDevice dev, oldDev;
  y = PSPopInteger();
  x = PSPopInteger();
  oldDev = PSGetDevice((PPSGState) NULL); /* get current device */
  dev = XDPS_SetXOffset(oldDev, x, y);
  if ((dev != (PDevice)NULL) && (dev != oldDev))
  {
     Mtx m1;
     m1 = *(PSGetMatrix(NULL));
     PSSetDevice(dev, 0);
     *(PSGetMatrix(NULL)) = m1;
  }
}

/*****************************************************************************/
/*                                                                           */
/*  PSSetXRGBActual                                                          */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure PSSetXRGBActual()
  /*	r g b    setXrgbactual    boolean	*/
{
  boolean       result;
  PDevice       device;
  DevColorSpace dcs;
  DevInputColor devInputColor;
  DevColor      devColor;
  real          real_read;
  real          real_green;
  real          real_blue;

  device = PSGetDevice((PPSGState) NULL); /* get current device */

  real_blue  = PSPopReal();
  real_green = PSPopReal();
  real_read  = PSPopReal();

  if (real_read  < 0.0 || real_read  > 1.0
  ||  real_green < 0.0 || real_green > 1.0
  ||  real_blue  < 0.0 || real_blue  > 1.0)
     PSRangeCheck();

  os_bzero (&dcs, sizeof(dcs));

  dcs.priv              = NIL;
  dcs.colorSpace        = DEVRGB_COLOR_SPACE;
/*dcs.cie               = not applicable here */
/*dcs.devNInfo          = not applicable here */
/*dcs.createDevNColor   = not applicable here */
/*dcs.addDevNColorRef   = not applicable here */
/*dcs.remDevNColorRef   = not applicable here */

  devInputColor.value.rgb.red   = real_read;
  devInputColor.value.rgb.green = real_green;
  devInputColor.value.rgb.blue  = real_blue;

  /*----------------------------------------------------------------*/
  /* It is important that we don't do our own color conversion here */
  /* but rather call the device's ConvertColor procedure. Otherwise */
  /* float-to-integer-to-char roundoff errors could prevent us from */
  /* getting a cache hit next time setrgbcolor is called.           */
  /*----------------------------------------------------------------*/

  PSConvertColor (&dcs, &devInputColor, &devColor);

  result = XDPS_SetXRGBActual(device, &devColor, &devInputColor);

  PSPushBoolean(result);
}

/*****************************************************************************/
/*                                                                           */
/*  PSClientSync                                                             */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure PSClientSync()
  /*	-	clientsync	-		*/
{
  DPSSys_SetupSchedExtVal (NIL);

  PSYield (yield_other, (char *)NIL);
}

/*****************************************************************************/
/*                                                                           */
/*  PSSetXDrawingFunction                                                    */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure PSSetXDrawingFunction()
  /*
  	function    setXdrawingfunction    -
  */
{
  GSDrawingFunc func, oldFunc, *pFunc;

  func = PSPopInteger();
  if ((func < GXclear) || (func > GXset))
    PSRangeCheck();
  pFunc = (GSDrawingFunc *)PSGetGStateExt((PPSGState) NULL);
  oldFunc = GetDrawingFunction(pFunc);
  SetDrawingFunction(pFunc, func);
  XDPS_DrawingFunctionChanged(
    PSGetDevice((PPSGState) NULL), oldFunc, func);
}

/*****************************************************************************/
/*                                                                           */
/*  PSCurrentXDrawingFunction                                                */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure PSCurrentXDrawingFunction()
  /*
  	-    currentXdrawingfunction	function
  */
{
  GSDrawingFunc *pFunc;

  pFunc = (GSDrawingFunc *) PSGetGStateExt((PPSGState) NULL);
  PSPushInteger(GetDrawingFunction(pFunc));
}

/*****************************************************************************/
/*                                                                           */
/*  PSClientXready                                                           */
/*                                                                           */
/*****************************************************************************/

PRIVATE procedure PSClientXready()
  /*
  	[i0 i1 i2 i3]  PSClientXready  -
  	           i0  PSClientXready  -
  */
{
  integer        idx;
  integer        val[4];
  PSObject       obj;
  PSOperandType  obj_type;

  val[0] = val[1] = val[2] = val[3] = 0;

  obj_type = PSGetOperandType ();                 /* stackunderflow */

  if (obj_type == arrayType)
  {
     PSPopTempObject (dpsArrayObj, &obj);            

     if ( (obj.length > 4) || (obj.length == 0) ) /* rangecheck     */
        PSRangeCheck();

     PDCheckArray (&obj, obj.length);             /* invalidaccess  */

     for (idx=0; idx<(integer)obj.length; idx++)
        val[idx] = PDArrayGetInt (&obj,idx);      /* typecheck      */
  }
  else
     val[0] = PSPopInteger();                     /* typecheck      */

  DPSSys_SetupSchedExtVal (val);

  PSYield (yield_other, (char *)NIL);
}

/*****************************************************************************/
/*                                                                           */
/*  XDPS_RegisterXOps                                                        */
/*                                                                           */
/*****************************************************************************/

PUBLIC procedure XDPS_RegisterXcustomOps (void)
{
  PSRegister ("setXgcdrawable",          PSSetXGCDrawable);
  PSRegister ("setXgcdrawablecolor",     PSSetXGCDrawableColor);
  PSRegister ("setXoffset",              PSSetXOffset);
  PSRegister ("currentXgcdrawable",      PSCurrentXGCDrawable);
  PSRegister ("currentXgcdrawablecolor", PSCurrentXGCDrawableColor);
  PSRegister ("currentXoffset",          PSCurrentXOffset);
  PSRegister ("clientsync",              PSClientSync);
  PSRegister ("setXrgbactual",           PSSetXRGBActual);
  PSRegister ("setXdrawingfunction",     PSSetXDrawingFunction);
  PSRegister ("currentXdrawingfunction", PSCurrentXDrawingFunction);
  PSRegister ("clientXready",            PSClientXready);
}
