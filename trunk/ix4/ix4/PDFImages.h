//
//  PDFImages.h
//  This file is part of ix4
// 
//  ix4 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  ix is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with Favorites.  If not, see <http://www.gnu.org/licenses/>.
//
//  This file is original work by Robin Mills, San Jose, CA 95112, USA
//  Created 2012 http://clanmills.com robin@clanmills.com
//

#ifndef ix4_PDFImages_h
#define ix4_PDFImages_h

#include <Cocoa/Cocoa.h>
#include <Quartz/Quartz.h>
#include <AppKit/AppKit.h>
#include <Foundation/Foundation.h>

#include "ix4.h"

// Obj-C interface
NSArray*
pathsToImages
( NSArray*  paths
, NSString* sortKey
, NSString* labelKey
, boolean_t desc
, boolean_t keys
, NSInteger minsize
);
    
PDFDocument*
imagesToPDF
( NSArray*  images
, NSString* pdfFileName
, boolean_t bOpen
, NSInteger resize
);

// C++ interface
PDFDocument*
imagesToPDF
( NSArray*  images
, const char* pdf
, boolean_t bOpen
, NSInteger resize
);

NSArray*
pathsToImages
( NSArray*  paths
, const char* sortKey
, const char* labelKey
, boolean_t desc
, boolean_t keys
, NSInteger minsize
);



#endif
