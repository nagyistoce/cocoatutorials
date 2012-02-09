//
//  PDFImages.h
//  ix4
//
//  Created by Robin Mills on 2012-02-05.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ix4_PDFImages_h
#define ix4_PDFImages_h

#include <Cocoa/Cocoa.h>
#include <AppKit/AppKit.h>

#include "IO.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif