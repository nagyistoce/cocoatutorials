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

// #define GETOPT_LONG 1
#define DDCLI       2

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

sort_e sort=se_none;

static void display_usage(const char* program,bool bAll)
{
	if ( bAll ) reportns([NSString stringWithFormat:@"%s - convert images to PDF",program]);
	report("usage: [ --[pdf filename | sort key | asc | desc | open | verbose | version | minsize n | help] | file ]+");
	exit( EXIT_FAILURE );
}

static void display_version( void )
{
	reportns( [NSString stringWithFormat:@"ix4: %d.%d built:%s %s",VERSION_MAJOR,VERSION_MINOR,__TIME__,__DATE__] );
	exit( EXIT_FAILURE );
}

#ifdef GETOPT_LONG

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

static sort_e    findSort    (Args& args,const char* key);
static error_e   parseArgs   (Args& args,int argc,char** argv);
static NSArray*  argsToImages(Args& args);

struct
{   const char*  key;
    sort_e       value;
}   sortKeys[] = 
{   "filename" , se_filename
,   "path"     , se_path
,   "date"     , se_date
};

struct Args {
	const char*	 pdf;                    /* -p: option       */
	sort_e       sort;                   /* -s: option       */
    bool         desc;                   /* -a|-d option     */
    int          open;                   /* -o  option       */
	int 		 verbose;                /* -v  global option*/ 
    int          version;                /* -V  option       */
	NSInteger 	 minsize;				 /* -m: option       */
    int          help;                   /* -h  option       */
	char**		 inputFiles;			 /* input files      */
	int 		 nInputFiles;			 /* # of input files */
    NSString*    sortKey;                /*                  */
};

// Modified from http://www.ibm.com/developerworks/aix/library/au-unix-getopt.html
static const char *optString = "p:s:adovVm:h?";
static const struct option longOpts[] =
{	{ "pdf"         , required_argument	, NULL, 'p' }
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
        reportns([NSString stringWithFormat:@ "sort:      %s" , sortKeys[args.sort].key]);
        reportns([NSString stringWithFormat:@ "direction: %s" , args.desc  ? "desc" : "asc"]);
        reportns([NSString stringWithFormat:@ "verbose:   %d" , getVerbose()]);
        reportns([NSString stringWithFormat:@ "minsize:   %ld", args.minsize]);
        reportns([NSString stringWithFormat:@ "open:      %d" , args.open]);
        
        for (int i = 0 ; i < args.nInputFiles ; i++ ) {
            reportns([NSString stringWithFormat:@"file %2d = %s",i,args.inputFiles[i]]);
        }
    }
}

static NSArray* argsToImages(Args& args)
{
    NSArray*        result = NULL;
    NSMutableArray* paths  = [[NSMutableArray alloc]init] ;
    if ( !paths ) noImages(true);
    
    ////
    // run over inputFiles to be sure everything is an image
    else {
        for ( int i = 0 ; !error && i < args.nInputFiles ; i++ ) {
            NSString* path = [NSString stringWithUTF8String:args.inputFiles[i]];
            [paths addObject:path];
        }
        result = pathsToImages(paths,args.sort,args.desc,args.minsize);
    }
    return result; 
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
			case 'p':
				args.pdf = optarg;
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
				setVerbose(YES);
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
    
    if ( !args.nInputFiles ) args.help = 1 ;
    
    return error ;
}

int main (int argc,char** argv)
{
    if ( argc <= 1 ) display_usage(argv[0],true);
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    // parse the command line
    Args args ;
    error = parseArgs(args,argc,argv);
    
    // tell the user what's going on
    if ( args.version ) display_version();
    if ( args.help    ) display_usage(argv[0],true);
	if ( !error       ) display_args(args);
    
	// convert the args into cocoa structures
    NSArray*  images  = argsToImages(args) ;
    NSString* pdf     = [NSString stringWithUTF8String:args.pdf];
    
	// generate output
    if ( images ) imagesToPDF(images,pdf,args.open) ;
    
    [pool drain];
    return error;
}

#endif

#ifdef DDCLI
// http://www.dribin.org/dave/blog/archives/2008/04/29/ddcli/
#import "DDCommandLineInterface.h"

@interface IX4App : NSObject <DDCliApplicationDelegate>
{
    int             _verbose;
    BOOL            _version;
    BOOL            _help;
    NSString*       _pdf;
    NSInteger       _minsize;
    BOOL            _open;
    NSString*       _sort;
    BOOL            _desc;
}
@end

@implementation IX4App
- (void) application: (DDCliApplication *) app
    willParseOptions: (DDGetoptLongParser *) optionsParser;
{
    DDGetoptOption optionTable[] = 
    {
        // Long         Short   Argument options
        {@"open"       ,'o',    DDGetoptNoArgument},
        {@"desc"       ,'d',    DDGetoptNoArgument},
        {@"asc"        ,'a',    DDGetoptNoArgument},
        {@"verbose"    ,'v',    DDGetoptNoArgument},
        {@"version"    ,'V' ,   DDGetoptNoArgument},
        {@"help"       ,'h',    DDGetoptNoArgument},
        {@"sort"       ,'s',    DDGetoptRequiredArgument},
        {@"pdf"        ,'p',    DDGetoptRequiredArgument},
        {@"minsize"    ,'m',    DDGetoptRequiredArgument},
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
    _desc = NO;
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
        ddprintf(@"open: %d\n"      ,_open);
        ddprintf(@"verbose: %d\n"   ,_verbose);
        ddprintf(@"sort: %@\n"      ,_sort);
        ddprintf(@"minsize: %d\n"   ,_minsize);
        ddprintf(@"pdf: %@\n"       ,_pdf);
        ddprintf(@"Arguments: %@\n" , arguments);
    }
    NSArray*    images  = pathsToImages(arguments,se_none,!_desc,_minsize);
    const char* message = !images         ? "no images allocated" 
                        : ![images count] ? "no suitable images found!"
                        : NULL
                        ;
    if (message) printf("%s\n",message);
    else imagesToPDF(images,_pdf,_open);    
    
    return !message ? EXIT_SUCCESS : EXIT_FAILURE ;
}

int main (int argc, char * const * argv)
{
    return DDCliAppRunWithClass([IX4App class]);
}
@end

#endif


