//
//  PDFImages.cpp
//  ix4
//
//  Created by Robin Mills on 2012-02-05.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import  <Foundation/Foundation.h>
#import  <AppKit/AppKit.h>
#import  <Quartz/Quartz.h>
#import  <ApplicationServices/ApplicationServices.h>

#include "PDFImages.h"

#define KEYVALUE "keyvalue"
#define FILENAME "filename"
#define PATH     "path"
#define DATE     "date"
#define LABEL    "label"
#define IMAGEREP "imageRep"

static NSInteger bigger(NSInteger a,NSInteger b) { return a > b ? a : b ; }

static NSComparisonResult sortFunction (id a, id b, void* p)
{
    boolean_t* pDesc = (boolean_t*) p;
	if ( *pDesc ) {
        id t = a;
        a = b ;
        b = t ;
    }
    a = [a valueForKey:@KEYVALUE];
    b = [b valueForKey:@KEYVALUE];
    
	return ( a && b ) ? [a compare:b] : NSOrderedSame ;
}


NSMutableArray* pathsToImages(NSArray* paths,sort_e sort,boolean_t desc,NSInteger minsize)
{
    NSMutableArray* images = [[NSMutableArray alloc]init] ;

    ////
    // run over inputFiles to be sure everything is an image
    if ( images ) for ( NSString* path in paths ) {
        NSString* fileName = [path lastPathComponent];
        NSImage*  image    = [[NSImage alloc] initWithContentsOfFile:path];
        id        keyValue = NULL;
        
        if ( image )
        {
            NSArray* reps = [image representations] ;
            ////
            // find the biggest representation
            NSInteger jBig	= -1 ;
            NSInteger biggest = 0 ;
            for ( int j = 0 ; j < [reps count] ; j++ ) {
                NSImageRep* imageRep= [ reps objectAtIndex:j] ;
                NSInteger   width   = [imageRep pixelsWide];
                NSInteger   height	= [imageRep pixelsHigh];
                NSInteger   big		= bigger(width,height) ;
                if ( big > biggest ) {
                    jBig = j ;
                    biggest = big ;
                }
            }
            
            ////
            // save the imageRep and other metadata in a dictionary in the images array
            if ( biggest >= minsize ) {
                NSImageRep*	  imageRep = [reps objectAtIndex:jBig];
                switch ( sort ) {
                    case se_filename:
                        keyValue = fileName;
                        break;
                        
                    case se_path:
                        keyValue = path;
                        break;
                        
                    case se_date     :
                    {   NSDictionary* exifDict = [((id)imageRep) valueForProperty: NSImageEXIFData];
                        if ( exifDict ) for ( NSString* exifKey in exifDict ) {
                            id value = [exifDict objectForKey: exifKey];
                            if (  [exifKey compare:@"DateTimeOriginal"] == 0 ) {
                                keyValue = value;
                                reportns([NSString stringWithFormat:@"exifKey %@ value: %@",exifKey,value]);
                                break;
                            }
                        }
                        if ( exifDict ) for ( NSString* exifKey in exifDict ) 
                            reportns([NSString stringWithFormat:@"exifKey %@ value: %@"
                                      ,exifKey,[exifDict objectForKey: exifKey]]);
                    }break;
                        
                    case se_none:
                    default:
                        break;
                }
                
                NSDictionary*	dict	= [ NSDictionary dictionaryWithObjectsAndKeys
                                           :imageRep,@IMAGEREP
                                           ,fileName,@FILENAME
                                           ,path,@PATH
                                           ,keyValue,@KEYVALUE
                                           ,nil
                                           ] ;
                
                [images addObject:dict] ;
            } else {
                warns([NSString stringWithFormat:@"image less than minsize %@",path]);
            }
        }
        if ( sort ) [images sortUsingFunction:sortFunction context:&desc];
    }
    
    return images;
}

PDFDocument* imagesToPDF(NSArray* images,NSString* pdfFileName,boolean_t bOpen)
{
	PDFDocument* document = [[PDFDocument alloc]init];
	if ( document ) {
        
		////
		// store an outline (with the name of the file on the label for every page)
		PDFOutline* outlines = [[PDFOutline alloc]init] ;
		[document setOutlineRoot:outlines] ;
		
		////
		// cycle over the images
		for ( int i = 0 ; i < [images count] ; i++ ) {
			NSDictionary*   dict	= [images objectAtIndex:i];
			NSImageRep*     imageRep= [dict  valueForKey:@IMAGEREP];
            NSString*       fileName= [dict  valueForKey:@FILENAME];
            id              keyValue= [dict  valueForKey:@KEYVALUE];
            NSString*       label   = [NSString stringWithFormat:@"%@",keyValue?keyValue:fileName];
			NSInteger       width	= [imageRep pixelsWide];
			NSInteger       height	= [imageRep pixelsHigh];
            
            // label is having trouble with leading /'s (for example in paths)
            NSMutableString* mString = [NSMutableString stringWithCapacity:[label length]+1];
            [mString appendString:label];
            [mString replaceOccurrencesOfString:@"/" withString:@" /" options:NSLiteralSearch range:NSMakeRange(0,1)];
            // [mString length])];
            label = [NSString stringWithString:mString];
            
            reportns([NSString stringWithFormat:@"label = %@",label]);
            
            // warns(path);
			// NSLog(@"%@ width,height = %ldx%ld",fileName,width,height) ;
            
			////
			// create the PDF page with the image and insert it at the end of the document
			NSImage* image = [[NSImage alloc]init] ;
			[image addRepresentation:imageRep] ;
            
            // older compiler demands (CIImage*) cast for (NSImage*)
            // cast him to (id) to keep compiler happy
			PDFPage* page = [[ PDFPage alloc]initWithImage:(id)image];
            
			if ( page ) {
				[document insertPage:page atIndex:i] ;
				
				////
				// store the filename in the Outline
				PDFOutline* outline = [[PDFOutline alloc]init] ;
				
				[outline setLabel:label] ;
				NSPoint point = { width/2, height/2 } ;
                
				PDFDestination* dest = [[PDFDestination alloc]initWithPage:page atPoint:point];
				[outline setAction:[[PDFActionGoTo alloc]initWithDestination:dest]];
				[outlines insertChild:outline atIndex:i];
                
				[page release];
			}
		}
        
		////
		// write the PDF file and display it
		if ( [document writeToFile:pdfFileName] ) {
            if ( bOpen/*args.open*/ ) {
                [[NSWorkspace sharedWorkspace] openFile:pdfFileName];
            }
		} else {
			errorns([NSString stringWithFormat:@"unable to write pdffile %@",pdfFileName]);
		}
	}
    return document ;
}
