//
//  ix4.mm
//  This file is part of ix4
// 
//  ix is free software: you can redistribute it and/or modify
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#define FILENAME @"FileName"
#define IMAGE    @"Image"

// Modified from http://www.ibm.com/developerworks/aix/library/au-unix-getopt.html
void display_usage(void);
void display_args (void);

static const char *optString = "o:Os:vm:h?";
struct globalArgs_t {
	const char*	outFileName;			/* -o option        */
	const char*	sort;                   /* -s option        */
    int         open;                   /* -O option        */
	int 		verbose;				/* -v option        */
	char**		inputFiles;				/* input files      */
	int 		numInputFiles;			/* # of input files */
	NSInteger 	minsize;				/* -m option        */
} globalArgs;

static const struct option longOpts[] = {
	{ "output"		, required_argument	, NULL, 'o' },
	{ "open"		, no_argument		, NULL, 'O' },
	{ "sort"		, required_argument	, NULL, 's' },
	{ "verbose"		, optional_argument	, NULL, 'v' },
	{ "minsize"     , required_argument	, NULL, 'm' },
	{ "help"		, no_argument		, NULL, 'h' },
	{ NULL			, no_argument		, NULL, 0   }
};

/* Display program usage, and exit.
 */
void display_usage( void )
{
	puts("ix4 - convert images to PDF" );
	puts("usage: [ --output filename | --verbose | --open | --sort s | --minsize n | --help | file ]+");
	exit( EXIT_FAILURE );
}

/* Convert the input files to PDF, governed by globalArgs.
 */
void display_args( void )
{
    if ( globalArgs.verbose ) {
        printf( "output:  %s\n" , globalArgs.outFileName);
        printf( "verbose: %d\n" , globalArgs.verbose);
        printf( "minsize: %ld\n", globalArgs.minsize);
        printf( "sort:    %s\n" , globalArgs.sort ? globalArgs.sort : "none");
        
//        for (int i = 0 ; i < globalArgs.numInputFiles ; i++ ) {
//            printf("file %2d = %s\n",i,globalArgs.inputFiles[i]);
//        }
    }
}
// end of code copied from ibm

static enum
{	OK = 0 
,	SYNTAX 
,   NOT_IMAGE
,	NO_IMAGES
,	NO_PDF_FILENAME
}	error = OK ;

void warn (const char* msg);
void warns(NSString*   msg);
void warn (const char* msg)
{
    if ( globalArgs.verbose ) {
        puts(msg);
    }
}

void warns(NSString* msg)
{
    warn([msg UTF8String]);
}

static void notImage(NSString* filename)
{
    warns([NSString stringWithFormat:@"%@ is not an image file",filename]);
    error = NOT_IMAGE ; // only warn, don't terminate
}

static void noImages(bool bCreate /*= true*/)
{
	warn(bCreate?"Unable to create images array":"no suitable images") ;
	error = NO_IMAGES ;
}

static void noPDFFileName(const char* filename)
{
	if ( filename ) {
        NSLog(@"Unable to create pdfFileName %s",filename) ;
    } else {
        NSLog(@"--output pdffile not specified") ;
        display_usage();
    }
	error = NO_PDF_FILENAME ;
}

static void syntax(void)
{
    void display_usage(void);
	error = SYNTAX ;
}

static NSComparisonResult comp(NSString* a,NSString* b) { return [a localizedCaseInsensitiveCompare : b]; }

static NSComparisonResult sortFunction (id a, id b, void* )
{
	a = [a valueForKey:FILENAME];
	b = [b valueForKey:FILENAME];
	NSComparisonResult result = comp([a lastPathComponent],[b lastPathComponent]);
	if ( !result )     result = comp(a,b) ;
	return result ;
}

static void doIt(NSString* pdfFileName,NSMutableArray* images)
{
	// sort the images using the filename (case insensitive)
    if ( globalArgs.sort ) [images sortUsingFunction:sortFunction context:nil];
	
	PDFDocument* document = [[PDFDocument alloc]init] ;
	if ( document ) {
        
		////
		// store an outline (with the name of the file on the label for every page)
		PDFOutline* outlines = [[PDFOutline alloc]init] ;
		[document setOutlineRoot:outlines] ;
		
		////
		// cycle over the images
		for ( int i = 0 ; i < [images count] ; i++ ) {
			NSDictionary* dict		= [images objectAtIndex:i];
			NSImageRep* imageRep	= [dict  valueForKey:IMAGE];
			NSString*	fileName	= [dict  valueForKey:FILENAME];
			NSInteger	width		= [imageRep pixelsWide];
			NSInteger	height		= [imageRep pixelsHigh];
            
            if ( globalArgs.verbose ) warns(fileName);
			// NSLog(@"%@ width,height = %ldx%ld",fileName,width,height) ;
            
			////
			// create the PDF page with the image and insert it at the end of the document
			NSImage* image = [[NSImage alloc]init] ;
			[image addRepresentation:imageRep] ;
			PDFPage* page = [[ PDFPage alloc]initWithImage:/*(CIImage*)*/image];
            
			if ( page ) {
				[document insertPage:page atIndex:i] ;
				
				////
				// store the filename in the Outline
				PDFOutline* outline = [[PDFOutline alloc]init] ;
				
				[outline setLabel:[fileName lastPathComponent]] ;
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
            if ( globalArgs.open ) {
                [[NSWorkspace sharedWorkspace] openFile:pdfFileName];
            }
		} else {
			NSLog(@"unable to write pdffile %@",pdfFileName);
		}
	}
}

static NSInteger bigger(NSInteger a,NSInteger b) { return a > b ? a : b ; }

int main (int argc,char** argv)
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
	int opt = 0;
	int longIndex = 0;
	
	/* Initialize globalArgs before we get to work. */
	globalArgs.outFileName 		= NULL;
    globalArgs.open             = 0   ;
    globalArgs.sort             = NULL;
	globalArgs.verbose 			= 0;
	globalArgs.inputFiles 		= NULL;
	globalArgs.numInputFiles 	= 0;
	globalArgs.minsize          = 512;
	
	/* Process the arguments with getopt_long(), then 
	 * populate globalArgs. 
	 */
	opt = ::getopt_long( argc, argv, optString, longOpts, &longIndex );
	while( opt != -1 ) {
		switch( opt ) {
			case 'o':
				globalArgs.outFileName = optarg;
				break;

			case 'O':
				globalArgs.open = 1;
				break;
				
			case 'v':
				globalArgs.verbose = optarg?atoi(optarg):1;
				break;
				
			case 'm':
				globalArgs.minsize = optarg?atoi(optarg):0;
				break;
				
			case 'h':	/* fall-through is intentional */
			case '?':
				display_usage();
				break;

			default:
				/* You won't actually get here. */
				break;
		}
		
		opt = ::getopt_long( argc, argv, optString, longOpts, &longIndex );
	}
	
	globalArgs.inputFiles 		= argv + optind;
	globalArgs.numInputFiles 	= argc - optind;
	
    if ( !error && !globalArgs.outFileName) noPDFFileName(NULL);

	NSString* pdfFileName  = error==OK ? [NSString stringWithUTF8String:(const char*)globalArgs.outFileName]: nil ;
	NSMutableArray* images = error==OK ? [[NSMutableArray alloc]init]			: nil ;
	
	if ( !error && !images      ) noImages(true) ;
	if ( !error && !pdfFileName ) noPDFFileName((const char*)globalArgs.outFileName);
    
    ////
	// run over inputFiles to be sure everything is an image
	if ( !error ) for ( int i = 0 ; !error && i < globalArgs.numInputFiles ; i++ ) {
		NSString* fileName =  [NSString stringWithUTF8String:globalArgs.inputFiles[i]];
		NSImage*  image    =  [[NSImage alloc] initWithContentsOfFile:fileName];
		if ( !image ) notImage(fileName) ;
		
		if ( image)
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
			// save the imageRep and filename in a dictionary in the images array
			if ( biggest >= globalArgs.minsize ) {
				NSImageRep*		imageRep= [reps objectAtIndex:jBig];
				NSDictionary*	dict	= [ NSDictionary dictionaryWithObjectsAndKeys:imageRep,IMAGE
                                           ,fileName,FILENAME
                                           ,nil
                                           ] ;
                
				[images addObject:dict] ;
			} else {
                warns([NSString stringWithFormat:@"too small %@",fileName]);
            }
		}
        if ( [images count]==0 ) noImages(false);
	}
    
	if ( !error ) display_args();
	if ( !error ) doIt(pdfFileName,images) ;
	
    [pool drain];
    return 0;
}

