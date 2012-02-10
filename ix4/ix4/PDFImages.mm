//
//  PDFImages.cpp
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

#import  <Foundation/Foundation.h>
#import  <AppKit/AppKit.h>
#import  <Quartz/Quartz.h>
#import  <ApplicationServices/ApplicationServices.h>

#include "DDCommandLineInterface.h"
#include "PDFImages.h"
#include "JSON.h"

NSString* FILENAME = @"filename";
NSString* METADICT = @"metadict";
NSString* IMAGEREP = @"imageRep";
NSString* LABEL    = @"label";
NSString* SORT     = @"sort";

static NSInteger bigger(NSInteger a,NSInteger b) { return a > b ? a : b ; }

static NSComparisonResult sortFunction(id a, id b, void* p)
{
    boolean_t* pDesc = (boolean_t*) p;
	if ( *pDesc ) {
        id t = a;
        a = b ;
        b = t ;
    }
    a = [a valueForKey:SORT];
    b = [b valueForKey:SORT];
    
	return ( a && b ) ? [a compare:b] : NSOrderedSame ;
}

static NSDictionary* dictDump(id object)
{
    NSDictionary* dictionary = [object isKindOfClass:[NSDictionary class]] ? object : nil;
    if ( dictionary ) {
        ddprintf(@"dictDump\n");
        for (id key in dictionary) {
            ddprintf(@"key: %@, value: %@\n", key, [dictionary objectForKey:key]);
        }
    }
	return dictionary ;
}

static NSArray* arrayDump(id object)
{
    NSArray* array = [object isKindOfClass:[NSArray class]] ? object : nil;
    if ( array ) {
        int index = 0 ;
        ddprintf(@"arrayDump\n");
        for (id value in array) {
            if ( [value isKindOfClass:[NSDictionary class]] )
                  value = [NSString stringWithFormat:@"<dict %d>",[value count]];
            ddprintf(@"index: %d, value: %@\n", index, value);
            index++ ;
        }
    }
	return array ;
}

NSArray*    pathsToImages
( NSArray*  paths
, NSString* sortKey
, NSString* labelKey
, boolean_t desc
, boolean_t keys
, NSInteger minsize
) {
    NSMutableArray* images = [[NSMutableArray alloc]init] ;
    NSMutableSet*   keySet = keys ? [NSMutableSet setWithCapacity:[paths count]] : nil;
    NSInteger       index = 0 ;
    
    if ( !images ) return images ;

    ////
    // get the metadata in JSON format with exiftool
    // http://www.sno.phy.queensu.ca/~phil/exiftool/
    NSTask*   task = [[NSTask alloc] init];
    [task     setLaunchPath:@"/usr/local/bin/exiftool"];
    NSMutableArray* args = [NSMutableArray arrayWithObjects:@"-j",nil];
    [args     addObjectsFromArray:paths];
    [task     setArguments:args];

    NSPipe*   outPipe = [[NSPipe alloc] init];
    [task     setStandardOutput:outPipe];
    [task     setStandardError:[NSFileHandle fileHandleForWritingAtPath:@"/dev/null"]];
    [outPipe  release];
    [task     launch];
    
    // Read the output
    NSData*   data = [[outPipe fileHandleForReading] readDataToEndOfFile];
    [task     waitUntilExit];
    int       status = [task terminationStatus];
    [task     release];

    // convert to JSON
    NSString* jsonString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    JSON*     json       = [[JSON alloc]init];
    NSObject* metaData   = status==0?[json jsonToObj:jsonString]:nil;

    for ( NSString* path in paths ) {
        NSString*     fileName = [path lastPathComponent];
        NSImage*      image    = [[NSImage alloc] initWithContentsOfFile:path];

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
            // find metadict in the metadata
            NSDictionary* metaDict = nil;
            if ( [metaData isKindOfClass:[NSArray class]] ) {
                NSArray* metaArray = (NSArray*) metaData;
                if ( [metaArray count] > index ) {
                    if ( [ [metaArray objectAtIndex:index] isKindOfClass:[NSDictionary class]] ) {
                        metaDict = [metaArray objectAtIndex:index];
                        if ( keySet ) {
                            for ( id k in metaDict )
                                [keySet addObject:k];
                        }
                    }
                } 
            }
            
            ////
            // save the imageRep and other metadata in a dictionary in the images array
            if ( biggest >= minsize ) {
                NSImageRep*             imageRep = [reps objectAtIndex:jBig];
                NSMutableDictionary*	dict	 = [ NSMutableDictionary dictionaryWithObjectsAndKeys
                                                   : imageRep,IMAGEREP
                                                   , fileName,FILENAME
                                                   ,nil
                                                   ] ;
                [images addObject:dict ] ;
                // keep metadict for future features based on meta-data
                [dict setObject:metaDict forKey:METADICT];

                // find the label in the meta data
                NSString* label = nil;
                if ( metaDict && labelKey ) {
                    if ( [metaDict objectForKey:labelKey] ) {
                        label = [metaDict objectForKey:labelKey];
                    } 
                }
                if ( !label ) label = fileName ;
                [dict setObject:label forKey:LABEL];

                // find the sort in the meta data
                NSString* sort = nil;
                if ( metaDict && sortKey ) {
                    if ( [metaDict objectForKey:sortKey] ) {
                        sort = [metaDict objectForKey:sortKey];
                    } 
                }
                if ( !sort  ) sort = [NSString stringWithFormat:@"06d",index] ;
                [dict setObject:sort forKey:SORT];
                
            } else {
                warns([NSString stringWithFormat:@"image less than minsize %@",path]);
            }
        }
        [images sortUsingFunction:sortFunction context:&desc];
        index++;
    }
    
    if ( keySet ) {
        NSSortDescriptor* sort = [NSSortDescriptor sortDescriptorWithKey:@"description"
                                                               ascending:desc?NO:YES];
        for ( id key in [keySet sortedArrayUsingDescriptors:[NSArray arrayWithObject:sort]] ) 
            ddprintf(@"key: %@\n",key);
    }
    
    return keys ? nil : images ;
}

static NSImage* scaleImage(NSImage* image,double width,double height);
static NSImage* scaleImage(NSImage* image,double width,double height)
{
    // http://lists.apple.com/archives/cocoa-dev/2004/Sep/msg01309.html
    double widthFactor;
    double heightFactor;
    double scale;

    [image setScalesWhenResized:YES];
    heightFactor = height/[image size].height;
    widthFactor =  width /[image size].width;
    if(heightFactor > widthFactor){
        scale = widthFactor;
    } else {
        scale = heightFactor;
    }
    width  = (int) ((double)([image size].width)  * scale);
    height = (int) ((double)([image size].height) * scale);
    
    NSImage* result = [[NSImage alloc] initWithSize:NSMakeSize(width, height)];

    NSAffineTransform*  at = [NSAffineTransform transform];
    [at scaleBy:scale];
    
    [result lockFocus];
    [[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationLow];
    [image setSize:[at transformSize:[image size]]];
    [image compositeToPoint:NSMakePoint((width-[image size].width)/2 , (height-[image size].height)/2) operation:NSCompositeCopy];
    [result unlockFocus];
    
    return result;
}

PDFDocument* imagesToPDF
( NSArray*  images
, NSString* pdf
, boolean_t bOpen
, NSInteger resize
) {
    if ( !images ) return nil;
    
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
			NSImageRep*     imageRep= [dict     valueForKey:IMAGEREP];
			NSInteger       width	= [imageRep pixelsWide];
			NSInteger       height	= [imageRep pixelsHigh];
            NSString*       label   = [dict     valueForKey:LABEL];
            
            // label is having trouble with leading /'s (for example in paths)
            // and we don't want to alter label (it's an NSString* not an NSMutableString* into the image's dictionary)
            NSMutableString* mString = [NSMutableString stringWithCapacity:[label length]+1];
            [mString appendString:label];
            [mString replaceOccurrencesOfString:@"/" withString:@" /" options:NSLiteralSearch range:NSMakeRange(0,1)];
            label =  [NSString stringWithString:mString];
            // reportns([NSString stringWithFormat:@"label = %@",label]);
            
			////
			// create the PDF page with the image and insert it at the end of the document
			NSImage* image = [[NSImage alloc]init] ;
			[image addRepresentation:imageRep] ;
            
            NSImage* thumb = resize ? scaleImage(image,resize,resize) : [image retain];
            
            // older compiler demands (CIImage*) cast for (NSImage*)
            // cast him to (id) to keep compiler happy
			PDFPage* page = [[ PDFPage alloc]initWithImage:(id)thumb];
            
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
		if ( [document writeToFile:pdf] ) {
            if ( bOpen/*args.open*/ ) {
                [[NSWorkspace sharedWorkspace] openFile:pdf];
            }
		} else {
			errorns([NSString stringWithFormat:@"unable to write pdffile %@",pdf]);
		}
	}
    return document ;
}
