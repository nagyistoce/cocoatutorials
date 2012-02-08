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
#define VERSION_MINOR 2

#define    GETOPT_LONG 1
#define    DDCLI       2
#define    BUILD_WITH  2

#import  <Foundation/Foundation.h>
#import  <AppKit/AppKit.h>
#import  <Quartz/Quartz.h>
#import  <ApplicationServices/ApplicationServices.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "PDFImages.h"

#ifndef lengthof
#define lengthof(x) sizeof(x)/sizeof(x[0])
#endif

static void display_usage(const char* program,bool bAll)
{
	if ( bAll ) reportns([NSString stringWithFormat:@"%s - convert images to PDF",program]);
	report("usage: [ --[pdf path | label key | sort key | asc | desc | open | verbose | version | minsize n | help] | file ]+");
	exit( EXIT_FAILURE );
}

static void display_version( void )
{
	reportns( [NSString stringWithFormat:@"ix4: %d.%d built:%s %s",VERSION_MAJOR,VERSION_MINOR,__TIME__,__DATE__] );
	exit( EXIT_FAILURE );
}

//////////////////////////////////////////////////////////////////
//==============================================================//
//////////////////////////////////////////////////////////////////
#if (BUILD_WITH==GETOPT_LONG)

// prototypes
struct Args ;
static void display_args (Args& args);

// static data
static enum error_e
{	OK = 0 
,	e_SYNTAX 
,   e_NOT_IMAGE
,	e_NO_IMAGES
,	e_NO_PDF_FILENAME
,   e_INVALIDSORT
,   e_UNRECOGNIZED_ARG
}	error = OK ;

static error_e   parseArgs   (Args& args,int argc,char** argv);
static NSArray*  argsToImages(Args& args);

struct Args {
	const char*	 pdf;                    /* -p: option       */
    const char*  label;                  /* -l: option       */
    const char*  sort;                   /* -s: option       */
    NSInteger    resize;                 /* -r: option       */
	NSInteger 	 minsize;				 /* -m: option       */
    bool         desc;                   /* -a|-d option     */
    int          keys;                   /* -k  option       */
    int          open;                   /* -o  option       */
	int 		 verbose;                /* -v  global option*/ 
    int          version;                /* -V  option       */
    int          help;                   /* -h  option       */
	char**		 inputFiles;			 /* input files      */
	int 		 nInputFiles;			 /* # of input files */
};

// Modified from http://www.ibm.com/developerworks/aix/library/au-unix-getopt.html
static const char*  optString = "p:s:r:m:l:adovVh?";
static const struct option longOpts[] =
{	{ "pdf"         , required_argument	, NULL, 'p' }
,	{ "sort"		, required_argument	, NULL, 's' }
,	{ "resize"      , required_argument	, NULL, 'r' }
,	{ "minsize"     , required_argument	, NULL, 'm' }
,	{ "label"		, required_argument , NULL, 'l' }
,	{ "asc"         , no_argument       , NULL, 'a' }
,	{ "desc"		, no_argument       , NULL, 'd' }
,	{ "keys"		, no_argument       , NULL, 'k' }
,	{ "open"		, no_argument		, NULL, 'O' }
,	{ "verbose"		, no_argument       , NULL, 'v' }
,	{ "version"		, no_argument       , NULL, 'V' }
,	{ "help"		, no_argument		, NULL, 'h' }
,	{ NULL			, no_argument		, NULL, 0   }
};

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

static void noPDFFileName(NSString* filename)
{
	if ( filename ) {
        errorns([NSString stringWithFormat:@"Unable to create pdfFileName %@",filename]) ;
    } else {
        errorns(@"--pdf pdffile not specified") ;
    }
	error = e_NO_PDF_FILENAME ;
}

static void syntaxError()
{
	error = e_SYNTAX ;
}

/* Convert the input files to PDF, governed by args.
 */
static void display_args( Args& args )
{
    if ( getVerbose() ) {
        reportns([NSString stringWithFormat:@ "pdf:       %s" , args.pdf]);
        reportns([NSString stringWithFormat:@ "sort:      %s" , args.sort]);
        reportns([NSString stringWithFormat:@ "direction: %s" , args.desc  ? "desc" : "asc"]);
        reportns([NSString stringWithFormat:@ "keys: %d"      , args.keys]);
        reportns([NSString stringWithFormat:@ "verbose:   %d" , getVerbose()]);
        reportns([NSString stringWithFormat:@ "resize:    %d" , args.resize]);
        reportns([NSString stringWithFormat:@ "minsize:   %ld", args.minsize]);
        reportns([NSString stringWithFormat:@ "label:     %s" , args.label]);
        reportns([NSString stringWithFormat:@ "open:      %d" , args.open]);
        
        for (int i = 0 ; i < args.nInputFiles ; i++ ) {
            reportns([NSString stringWithFormat:@"file %2d = %s",i,args.inputFiles[i]]);
        }
    }
}

static error_e parseArgs(Args& args,int argc,char** argv,NSArray** pArray)
{
    int opt = 0;
	int longIndex = 0;
    NSMutableArray* array = [NSMutableArray arrayWithCapacity:argc];
	
	/* Initialize args before we get to work. */
    memset(&args,0,sizeof(args));
	args.minsize = 512   ;
	
	/* Process the arguments with getopt_long(), then 
	 * populate args. 
	 */
	while( (opt = ::getopt_long( argc, argv, optString, longOpts, &longIndex)) !=-1 ) {
		switch( opt ) {
			case 'p':
				args.pdf = optarg;
				break;
                
			case 'a':
				args.desc = 0;
				break;
                
			case 'd':
				args.desc = 1;
				break;
                
			case 'k':
				args.keys = 1;
				break;
                
			case 'o':
				args.open = 1;
				break;
				
			case 'v':
				setVerbose(YES);
				break;
				
			case 'V':
				args.version = 1;
				break;
				
			case 'r':
				args.resize = optarg?atoi(optarg):0;
				break;
				
			case 's':
				args.sort = optarg;
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
        else
            [array addObject:[NSString stringWithUTF8String:args.inputFiles[i]]];
    
    if ( !args.nInputFiles ) args.help = 1 ;
    *pArray = array;
    
    return error ;
}

int main (int argc,char** argv)
{
    if ( argc <= 1 ) display_usage(argv[0],true);
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    // parse the command line
    Args args ;
    NSArray* paths;
    error = parseArgs(args,argc,argv,&paths);
    
    // tell the user what's going on
    if ( args.version ) display_version();
    if ( args.help    ) display_usage(argv[0],true);
	if ( !error       ) {
        display_args(args);   
        // convert the args into cocoa structures
        NSString* label  = args.label?[NSString stringWithUTF8String:args.label]:nil;
        NSString* sort   = args.sort ?[NSString stringWithUTF8String:args.sort]:nil;
        NSString* pdf    = [NSString stringWithUTF8String:args.pdf];
        NSArray*  images = pathsToImages(paths,sort,label,args.desc,args.keys,args.minsize);
        
        // generate output
        if ( images ) imagesToPDF(images,pdf,args.open,args.resize) ;
    }
    
    [pool drain];
    return error;
}
#endif

//////////////////////////////////////////////////////////////////
//==============================================================//
//////////////////////////////////////////////////////////////////

#if (BUILD_WITH==DDCLI)
// http://www.dribin.org/dave/blog/archives/2008/04/29/ddcli/
#import "DDCommandLineInterface.h"

@interface IX4App : NSObject <DDCliApplicationDelegate>
{
    int             _verbose;
    BOOL            _version;
    BOOL            _help;
    BOOL            _keys;
    NSString*       _pdf;
    NSInteger       _minsize;
    BOOL            _open;
    NSString*       _sort;
    BOOL            _desc;
    NSInteger       _resize;
    NSString*       _label;
}
@end

@implementation IX4App
- (void) application: (DDCliApplication *) app
    willParseOptions: (DDGetoptLongParser *) optionsParser;
{
    DDGetoptOption optionTable[] = 
    {
        // Long         Short   Argument options
        {@"sort"       ,'s',    DDGetoptRequiredArgument},
        {@"resize"     ,'r',    DDGetoptRequiredArgument},
        {@"pdf"        ,'p',    DDGetoptRequiredArgument},
        {@"minsize"    ,'m',    DDGetoptRequiredArgument},
        {@"label"      ,'l',    DDGetoptRequiredArgument},
        {@"open"       ,'o',    DDGetoptNoArgument},
        {@"desc"       ,'d',    DDGetoptNoArgument},
        {@"asc"        ,'a',    DDGetoptNoArgument},
        {@"keys"       ,'k',    DDGetoptNoArgument},
        {@"verbose"    ,'v',    DDGetoptNoArgument},
        {@"version"    ,'V' ,   DDGetoptNoArgument},
        {@"help"       ,'h',    DDGetoptNoArgument},
        {nil,            0 ,   (DDGetoptArgumentOptions)0},
    };
    [optionsParser addOptionsFromTable: optionTable];
}
- (id) init;
{
    self = [super init];
    if (self == nil)
        return nil;
    return self;
}

- (void) setVerbose: (BOOL) verbose;
{
    _verbose = verbose;
    ::setVerbose(verbose);
}

- (void) setAsc: (BOOL) asc;
{
    _desc = asc?NO:YES;
}

- (void) printUsage: (FILE *) stream;
{
    ddfprintf(stream, @"%@: Usage [OPTIONS] <argument> [...]\n", DDCliApp);
    display_usage("ix4",false);
}

- (void) printHelp;
{
    [self printUsage: stdout];
    const char* ix4 = "ix4";
    display_usage(ix4,false);
}

- (void) printVersion;
{
    ddprintf(@"%@ version: %d.%d built: %s %s\n", DDCliApp, VERSION_MINOR,VERSION_MAJOR,__TIME__,__DATE__);
    display_version();
}

- (int) application: (DDCliApplication *) app
   runWithArguments: (NSArray *) arguments;
{
    if (_help)
    {
        [self printHelp];
        return EXIT_SUCCESS;
    }
    
    if (_version)
    {
        [self printVersion];
        return EXIT_SUCCESS;
    }
    
    if ([arguments count] < 1)
    {
        ddfprintf(stderr, @"%@: At least one argument is required\n", DDCliApp);
        [self printUsage: stderr];
        ddfprintf(stderr, @"Try `%@ --help' for more information.\n", DDCliApp);
        return EX_USAGE;
    }
    if ( _verbose ) {
        ddprintf(@"pdf: %@\n"       ,_pdf);
        ddprintf(@"asc: %d\n"       ,_desc?0:1);
        ddprintf(@"keys: %d\n"      ,_keys);
        ddprintf(@"open: %d\n"      ,_open);
        ddprintf(@"verbose: %d\n"   ,_verbose);
        ddprintf(@"sort: %@\n"      ,_sort);
        ddprintf(@"resize: %d\n"    ,_resize);
        ddprintf(@"minsize: %d\n"   ,_minsize);
        ddprintf(@"pdf: %@\n"       ,_pdf);
        ddprintf(@"label: %@\n"     ,_label);
        ddprintf(@"Arguments: %@\n" , arguments);
    }
    NSArray*    images  = pathsToImages(arguments,_sort,_label,_desc,_keys,_minsize);
    const char* message = !images         ? "no images allocated" 
                        : ![images count] ? "no suitable images found!"
                        : NULL
                        ;
    if (message) printf("%s\n",message);
    imagesToPDF(images,_pdf,_open,_resize);    
    
    return !message ? EXIT_SUCCESS : EXIT_FAILURE ;
}

int main (int argc, char * const * argv)
{
    return DDCliAppRunWithClass([IX4App class]);
}
@end
//////////////////////////////////////////////////////////////////
//==============================================================//
//////////////////////////////////////////////////////////////////

#endif


