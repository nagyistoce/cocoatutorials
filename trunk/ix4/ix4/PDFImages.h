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
    
enum sort_e
{   se_none=0
,   se_path
,   se_filename
,   se_date
};
    

NSMutableArray* pathsToImages(NSArray* paths,sort_e sort,boolean_t asc,NSInteger minsize);
PDFDocument*    imagesToPDF  (NSArray* images,NSString* pdfFileName,boolean_t bOpen);

#ifdef __cplusplus
}
#endif

#endif
