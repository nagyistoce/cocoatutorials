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

#define KEYVALUE "keyvalue"
#define FILENAME "filename"
#define PATH     "path"
#define DATE     "date"
#define IMAGEREP "imageRep"

#ifndef lengthof
#define lengthof(x) sizeof(x)/sizeof(x[0])
#endif

static enum
{	OK = 0 
,	SYNTAX 
,   NOT_IMAGE
,	NO_IMAGES
,	NO_PDF_FILENAME
,   INVALIDSORT
}	error = OK ;

enum sort_e
{   se_none=0
,   se_path
,   se_filename
,   se_date
}   sort=se_none;

struct
{   const char* key;
    sort_e      value;
}  sortKeys[] = 
{   FILENAME    , se_filename
,   PATH        , se_path
,   DATE        , se_date
};

static sort_e findSort(const char* key);
static void warn    (const char* msg);
static void errors  (const char* msg);
static void report  (const char* msg);

static void warns   (NSString* msg);
static void errorns (NSString* msg);
static void reportns(NSString* msg);

// Modified from http://www.ibm.com/developerworks/aix/library/au-unix-getopt.html
static void display_usage(void);
static void display_args (void);

static const char *optString = "o:s:adOvm:h?";
struct {
	const char*	outFileName;			/* -o: option       */
	sort_e      sort;                   /* -s: option       */
    bool        asc;                    /* -a|-d option     */
    int         open;                   /* -O  option       */
	int 		verbose;				/* -v  option       */
	NSInteger 	minsize;				/* -m: option       */
	char**		inputFiles;				/* input files      */
	int 		numInputFiles;			/* # of input files */
    NSString*   sortKey;                /*                  */
} globalArgs;

static const struct option longOpts[] = {
	{ "output"		, required_argument	, NULL, 'o' },
	{ "sort"		, required_argument	, NULL, 's' },
	{ "asc"         , optional_argument	, NULL, 'a' },
	{ "desc"		, optional_argument	, NULL, 'd' },
	{ "open"		, no_argument		, NULL, 'O' },
	{ "verbose"		, optional_argument	, NULL, 'v' },
	{ "minsize"     , required_argument	, NULL, 'm' },
	{ "help"		, no_argument		, NULL, 'h' },
	{ NULL			, no_argument		, NULL, 0   }
};

/* Display program usage, and exit.
 */
static void display_usage( void )
{
	puts("ix4 - convert images to PDF" );
	puts("usage: [ --[output filename | sort key | asc | desc | open | verbose | minsize n | help] | file ]+");
	exit( EXIT_FAILURE );
}

/* Convert the input files to PDF, governed by globalArgs.
 */
static void display_args( void )
{
    if ( globalArgs.verbose ) {
        warns([NSString stringWithFormat:@"output:    %s\n" , globalArgs.outFileName]);
        warns([NSString stringWithFormat:@ "sort:      %s\n" , sortKeys[globalArgs.sort].key]);
        warns([NSString stringWithFormat:@ "direction: %s\n" , globalArgs.asc  ? "asc" : "desc"]);
        warns([NSString stringWithFormat:@ "verbose:   %d\n" , globalArgs.verbose]);
        warns([NSString stringWithFormat:@ "minsize:   %ld\n", globalArgs.minsize]);
        warns([NSString stringWithFormat:@ "open:      %d\n" , globalArgs.open]);
        
//        for (int i = 0 ; i < globalArgs.numInputFiles ; i++ ) {
//            warnsns([NSString stringWithFormat:@"file %2d = %s",i,globalArgs.inputFiles[i]]);
//        }
    }
}
// end of code copied from ibm

static sort_e findSort(const char* key)
{
    sort_e result = se_none;
    for ( int i = 0 ; !result && i < lengthof(sortKeys) ; i++ )
        if ( strcmp(key,sortKeys[i].key)==0 ) {
            result = sortKeys[i].value;
            [globalArgs.sortKey release];
             globalArgs.sortKey = [NSString stringWithUTF8String:key];
        }
    return result;
}

static void warn(const char* msg)
{
    if ( globalArgs.verbose ) {
        puts(msg);
    }
}

static void errors(const char* msg)
{
    puts(msg);
}

static void errorns(NSString* msg)
{
    errors([msg UTF8String]);
}

static void report(const char* msg)
{
    puts(msg);
}

static void reportns(NSString* msg)
{
    errors([msg UTF8String]);
}

static void warns(NSString* msg)
{
    warn([msg UTF8String]);
}

static void notImage(NSString* filename)
{
    errorns([NSString stringWithFormat:@"%@ is not an image file",filename]);
    error = NOT_IMAGE ; // only warn, don't terminate
}

static void noImages(bool bCreate /*= true*/)
{
	errors(bCreate?"Unable to create images array":"no suitable images") ;
	error = NO_IMAGES ;
}

static void invalidSortKey(const char* key)
{
	NSString* msg = [NSString stringWithFormat:@"invalid sort key %s",key];
    errorns(msg);
    error = INVALIDSORT;
}

static void noPDFFileName(const char* filename)
{
	if ( filename ) {
        errorns([NSString stringWithFormat:@"Unable to create pdfFileName %s",filename]) ;
    } else {
        errorns(@"--output pdffile not specified") ;
        display_usage();
    }
	error = NO_PDF_FILENAME ;
}

static void syntax(void)
{
    void display_usage(void);
	error = SYNTAX ;
}

static NSComparisonResult sortFunction (id a, id b, void* )
{
	if ( !globalArgs.asc ) {
        id t = a;
        a = b ;
        b = t ;
    }
    a = [a valueForKey:@KEYVALUE];
    b = [b valueForKey:@KEYVALUE];
    
	return ( a && b ) ? [a compare:b] : NSOrderedSame ;
}

static void doIt(NSString* pdfFileName,NSMutableArray* images)
{
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
			NSDictionary*   dict	= [images objectAtIndex:i];
			NSImageRep*     imageRep= [dict  valueForKey:@IMAGEREP];
            id              keyValue= [dict  valueForKey:@KEYVALUE];
            NSString*       label   = [NSString stringWithFormat:@"%@",keyValue?keyValue:@"--null--"];
			NSInteger       width	= [imageRep pixelsWide];
			NSInteger       height	= [imageRep pixelsHigh];
            
            // label is having trouble with leading /'s (for example in paths)
            NSMutableString* mString = [NSMutableString stringWithCapacity:[label length]+1];
            [mString appendString:label];
            [mString replaceOccurrencesOfString:@"/" withString:@" /" options:NSLiteralSearch range:NSMakeRange(0,1)];// [mString length])];
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
    memset(&globalArgs,0,sizeof(globalArgs));
	globalArgs.minsize = 512   ;
	
	/* Process the arguments with getopt_long(), then 
	 * populate globalArgs. 
	 */
	opt = ::getopt_long( argc, argv, optString, longOpts, &longIndex );
	while( opt != -1 ) {
		switch( opt ) {
			case 'o':
				globalArgs.outFileName = optarg;
				break;

			case 'a':
				globalArgs.asc = 1;
				break;
                
			case 'd':
				globalArgs.asc = 0;
				break;
                
			case 'O':
				globalArgs.open = 1;
				break;
				
			case 'v':
				globalArgs.verbose = 1;
				break;
				
			case 's':
				globalArgs.sort = findSort(optarg);
                if ( !error && !globalArgs.sort ) invalidSortKey(optarg);
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
		NSString* path     = [NSString stringWithUTF8String:globalArgs.inputFiles[i]];
        NSString* fileName = [path lastPathComponent];
		NSImage*  image    = [[NSImage alloc] initWithContentsOfFile:path];
        id        keyValue = NULL;
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
			// save the imageRep and other metadata in a dictionary in the images array
			if ( biggest >= globalArgs.minsize ) {
				NSImageRep*	  imageRep = [reps objectAtIndex:jBig];
                switch ( globalArgs.sort ) {
                    case se_filename :
                        keyValue = fileName;
                        break;
                        
                    case se_path     :
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
                warns([NSString stringWithFormat:@"too small %@",path]);
            }
		}
        if ( [images count]==0 ) noImages(false);
	}
    
	if ( !error ) display_args();
	if ( !error ) doIt(pdfFileName,images) ;
	
    [pool drain];
    return error;
}

