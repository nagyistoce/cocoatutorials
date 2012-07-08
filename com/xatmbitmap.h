/* @(#)CM_VerSion xatmbitmap.h 3010 1.3 19246.eco sum= 00103 3010.005 */
/* @(#)CM_VerSion xatmbitmap.h 2017 1.4 17334.eco sum= 49688 2017.011 */
/* @(#)CM_VerSion xatmbitmap.h 2015 1.2 11637.eco sum= 32728 */
/*
  xatmbitmap.h

(c) Copyright 1991-1993 Adobe Systems Incorporated.
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

#ifndef	XATMBITMAP_H
#define XATMBITMAP_H

#include "xatmfont.h"

/*

Interface to the bitmap-manipulation loading functions.

Since the procedures that do the actual work are all called by procedure
pointer, and since I like to hide as much stuff from being public as
possible, I instead declare a procedure-loading function that loads the
procedure pointers into the font.  Nice, clean, simple interface.

*/

extern procedure XatmLoadBitmapFuncs ARGDECL1(FontPtr, font);

#endif	/* XATMBITMAP_H */



