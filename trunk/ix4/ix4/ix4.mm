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

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

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
#define LABEL    "label"
#define IMAGEREP "imageRep"

#ifndef lengthof
#define lengthof(x) sizeof(x)/sizeof(x[0])
#endif

// http://www.dribin.org/dave/blog/archives/2008/04/29/ddcli/

// static data
static int gbVerbose = 1 ;
static enum error_e
{	OK = 0 
,	e_SYNTAX 
,   e_NOT_IMAGE
,	e_NO_IMAGES
,	e_NO_PDF_FILENAME
,   e_INVALIDSORT
,   e_UNRECOGNIZED_ARG
}	error = OK ;

// local data structures
enum sort_e
{   se_none=0
,   se_path
,   se_filename
,   se_date
}   sort=se_none;

struct
{   const char* key;
    sort_e      value;
}   sortKeys[] = 
{   FILENAME    , se_filename
,   PATH        , se_path
,   DATE        , se_date
};

// forward declaration
struct Args ;

static sort_e           findSort    (Args& args,const char* key);
static error_e          parseArgs   (Args& args,int argc,char** argv);
static NSMutableArray*  inspectInput(Args& args);
static error_e          doIt        (Args& args,NSArray* images);

// report => stdout error => stderr warn => stderr IF verbose
static void warn    (const char* msg);
static void errors  (const char* msg);
static void report  (const char* msg);

static void warns   (NSString* msg);
static void errorns (NSString* msg);
static void reportns(NSString* msg);

// Modified from http://www.ibm.com/developerworks/aix/library/au-unix-getopt.html

static const char *optString = "o:s:adOvVm:h?";
struct Args {
	const char*	outFileName;			/* -o: option       */
	sort_e      sort;                   /* -s: option       */
    bool        desc;                   /* -a|-d option     */
    int         open;                   /* -O  option       */
//	int 		verbose;				/* -v  Global option*/ 
    int         version;                /* -V  option       */
	NSInteger 	minsize;				/* -m: option       */
    int         help;                   /* -h  option       */
	char**		inputFiles;				/* input files      */
	int 		nInputFiles;			/* # of input files */
    NSString*   sortKey;                /*                  */
};

static const struct option longOpts[] =
{	{ "output"		, required_argument	, NULL, 'o' }
,	{ "sort"		, required_argument	, NULL, 's' }
,	{ "asc"         , no_argument       , NULL, 'a' }
,	{ "desc"		, no_argument       , NULL, 'd' }
,	{ "open"		, no_argument		, NULL, 'O' }
,	{ "verbose"		, no_argument       , NULL, 'v' }
,	{ "version"		, no_argument       , NULL, 'V' }
,	{ "minsize"     , required_argument	, NULL, 'm' }
,	{ "help"		, no_argument		, NULL, 'h' }
,	{ NULL			, no_argument		, NULL, 0   }
};

static void display_usage(char** argv,bool bAll=false);
static void display_args (Args& args);
static void display_version(void);

/* Display program usage, and exit.
 */
static void display_usage(char** argv,bool bAll /* = false*/)
{
	if ( bAll ) reportns([NSString stringWithFormat:@"%s - convert images to PDF",argv[0]]);
	report("usage: [ --[output filename | sort key | asc | desc | open | verbose | version | minsize n | help] | file ]+");
	exit( EXIT_FAILURE );
}

/* Display program version, and exit.
 */
static void display_version( void )
{
	reportns( [NSString stringWithFormat:@"ix4: %d.%d built:%s %s",VERSION_MAJOR,VERSION_MINOR,__TIME__,__DATE__] );
	exit( EXIT_FAILURE );
}

/* Convert the input files to PDF, governed by args.
 */
static void display_args( Args& args )
{
    if ( gbVerbose ) {
        reportns([NSString stringWithFormat:@ "output:    %s" , args.outFileName]);
        reportns([NSString stringWithFormat:@ "sort:      %s" , sortKeys[args.sort].key]);
        reportns([NSString stringWithFormat:@ "direction: %s" , args.desc  ? "desc" : "asc"]);
        reportns([NSString stringWithFormat:@ "verbose:   %d" , gbVerbose]);
        reportns([NSString stringWithFormat:@ "minsize:   %ld", args.minsize]);
        reportns([NSString stringWithFormat:@ "open:      %d" , args.open]);
        
        for (int i = 0 ; i < args.nInputFiles ; i++ ) {
            reportns([NSString stringWithFormat:@"file %2d = %s",i,args.inputFiles[i]]);
        }
    }
}
// end of code copied from ibm

static NSInteger bigger(NSInteger a,NSInteger b) { return a > b ? a : b ; }

static sort_e findSort(Args& args,const char* key)
{
    sort_e result = se_none;
    for ( int i = 0 ; !result && i < lengthof(sortKeys) ; i++ )
        if ( strcmp(key,sortKeys[i].key)==0 ) {
            result = sortKeys[i].value;
            [args.sortKey release];
             args.sortKey = [NSString stringWithUTF8String:key];
        }
    return result;
}

static void warn(const char* msg)
{
    if ( gbVerbose ) {
        errors(msg);
    }
}

static void errors(const char* msg)
{
    fputs(msg,stderr);
    fputc('\n',stderr);
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
    report([msg UTF8String]);
}

static void warns(NSString* msg)
{
    warn([msg UTF8String]);
}

static void unrecognizedArgument(const char* arg)
{
    errorns([NSString stringWithFormat:@"%unrecognized argument: %s",arg]);
    error = e_UNRECOGNIZED_ARG ;
}

static void notImage(NSString* filename)
{
    errorns([NSString stringWithFormat:@"%@ is not an image file",filename]);
    error = e_NOT_IMAGE ;
}

static void noImages(bool bCreate /*= true*/)
{
	errors(bCreate?"Unable to create images array":"no suitable images") ;
	error = e_NO_IMAGES ;
}

static void invalidSortKey(const char* key)
{
	NSString* msg = [NSString stringWithFormat:@"invalid sort key %s",key];
    errorns(msg);
    error = e_INVALIDSORT;
}

static void noPDFFileName(const char* filename)
{
	if ( filename ) {
        errorns([NSString stringWithFormat:@"Unable to create pdfFileName %s",filename]) ;
    } else {
        errorns(@"--output pdffile not specified") ;
    }
	error = e_NO_PDF_FILENAME ;
}

static void syntaxError()
{
	error = e_SYNTAX ;
}

static NSComparisonResult sortFunction (id a, id b, void* p)
{
    Args* pArgs = (Args*) p;
	if ( pArgs->desc ) {
        id t = a;
        a = b ;
        b = t ;
    }
    a = [a valueForKey:@KEYVALUE];
    b = [b valueForKey:@KEYVALUE];
    
	return ( a && b ) ? [a compare:b] : NSOrderedSame ;
}

static NSMutableArray* inspectInput(Args& args)
{
    NSMutableArray* images = [[NSMutableArray alloc]init] ;
    if ( !images ) noImages(true);
    
    ////
    // run over inputFiles to be sure everything is an image
    for ( int i = 0 ; !error && i < args.nInputFiles ; i++ ) {
        NSString* path     = [NSString stringWithUTF8String:args.inputFiles[i]];
        NSString* fileName = [path lastPathComponent];
        NSImage*  image    = [[NSImage alloc] initWithContentsOfFile:path];
        id        keyValue = NULL;
        // if ( !image ) notImage(path) ;
        
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
            if ( biggest >= args.minsize ) {
                NSImageRep*	  imageRep = [reps objectAtIndex:jBig];
                switch ( args.sort ) {
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
        if ( !error && ![images count]) noImages(false);
        if ( args.sort ) [images sortUsingFunction:sortFunction context:&args];
    }
    
    return images;
}

static error_e doIt(Args& args,NSArray* images)
{
	if ( !error && !args.outFileName) noPDFFileName(NULL);
    const char* outFileName = (const char*)args.outFileName ;
	NSString*   pdfFileName = error==OK ? [NSString stringWithUTF8String:outFileName]: nil ;
	if ( !error && !pdfFileName   ) noPDFFileName((const char*)args.outFileName);

	PDFDocument* document = error==OK?[[PDFDocument alloc]init]:NULL ;
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
            if ( args.open ) {
                [[NSWorkspace sharedWorkspace] openFile:pdfFileName];
            }
		} else {
			errorns([NSString stringWithFormat:@"unable to write pdffile %@",pdfFileName]);
		}
	}
    return error;
}

static error_e parseArgs(Args& args,int argc,char** argv)
{
    int opt = 0;
	int longIndex = 0;
	
	/* Initialize args before we get to work. */
    memset(&args,0,sizeof(args));
	args.minsize = 512   ;
	
	/* Process the arguments with getopt_long(), then 
	 * populate args. 
	 */
	while( (opt = ::getopt_long( argc, argv, optString, longOpts, &longIndex)) !=-1 ) {
		switch( opt ) {
			case 'o':
				args.outFileName = optarg;
				break;
                
			case 'a':
				args.desc = 0;
				break;
                
			case 'd':
				args.desc = 1;
				break;
                
			case 'O':
				args.open = 1;
				break;
				
			case 'v':
				gbVerbose++;
				break;
				
			case 'V':
				args.version = 1;
				break;
				
			case 's':
				args.sort = findSort(args,optarg);
                if ( !error && !args.sort ) invalidSortKey(optarg);
				break;
				
			case 'm':
				args.minsize = optarg?atoi(optarg):0;
				break;
				
			case 'h':	/* fall-through is intentional */
			case '?':
				args.help = 1;
				break;
                
			default:
                // this should not happen
				break;
		}
	}
	
	args.inputFiles 	= argv + optind;
	args.nInputFiles 	= argc - optind;
    for ( int i = 0 ; !error && i < args.nInputFiles ; i++ ) 
        if ( args.inputFiles[i][0] == '-' )
            unrecognizedArgument(args.inputFiles[i]);
    
    return error ;
}

int main (int argc,char** argv)
{
    if ( argc <= 1 ) display_usage(argv,true);
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    // parse the command line
    Args args ;
    error = parseArgs(args,argc,argv);
    
    // tell the use what's going on
    if ( args.version ) display_version();
    if ( args.help    ) display_usage(argv,true);
    gbVerbose--;
    
	if ( !error       ) display_args(args);

	// convert the args into cocoa structures
    NSMutableArray* images = inspectInput(args) ;
    
	// generate output
    if ( !error ) doIt(args,images) ;
    
    if ( !error && !args.nInputFiles ) args.help = 1 ;
    [pool drain];
    return error;
}

