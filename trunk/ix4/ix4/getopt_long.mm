//
//  getopt_long.mm
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "ix4.h"

#if BUILD_WITH == GETOPT_LONG

// prototypes
struct Args ;
static void      display_args (Args& args);
static error_e   parseArgs   (Args& args,int argc,char** argv);
static NSArray*  argsToImages(Args& args);

struct Args {
	const char*	 pdf;                    /* -p: option       */
    const char*  label;                  /* -l: option       */
    const char*  sort;                   /* -s: option       */
	NSInteger 	 minsize;				 /* -m: option       */
    NSInteger    resize;                 /* -r: option       */
    int          help;                   /* -h  option       */
    int          keys;                   /* -k  option       */
    bool         desc;                   /* -a|-d option     */
    int          open;                   /* -o  option       */
	int 		 verbose;                /* -v  global option*/ 
    int          version;                /* -V  option       */
	char**		 inputFiles;			 /* input files      */
	int 		 nInputFiles;			 /* # of input files */
};
static const char*  optString = "p:l:s:m:r:h?adovV";
static const struct option longOpts[] =
{	{ "pdf"         , required_argument	, NULL, 'p' }
,	{ "label"		, required_argument , NULL, 'l' }
,	{ "sort"		, required_argument	, NULL, 's' }
,	{ "minsize"     , required_argument	, NULL, 'm' }
,	{ "resize"      , required_argument	, NULL, 'r' }
,	{ "help"		, no_argument		, NULL, 'h' }
,	{ "keys"		, no_argument       , NULL, 'k' }
,	{ "asc"         , no_argument       , NULL, 'a' }
,	{ "desc"		, no_argument       , NULL, 'd' }
,	{ "open"		, no_argument		, NULL, 'o' }
,	{ "verbose"		, no_argument       , NULL, 'v' }
,	{ "version"		, no_argument       , NULL, 'V' }
,	{ NULL			, no_argument		, NULL,  0  }
};

static void display_args( Args& args )
{
    if ( getVerbose() ) {
        reportns([NSString stringWithFormat:@ "pdf:       %s" , args.pdf]);
        reportns([NSString stringWithFormat:@ "label:     %s" , args.label]);
        reportns([NSString stringWithFormat:@ "sort:      %s" , args.sort]);
        reportns([NSString stringWithFormat:@ "minsize:   %ld", args.minsize]);
        reportns([NSString stringWithFormat:@ "resize:    %ld", args.resize]);
        reportns([NSString stringWithFormat:@ "help: %d"      , args.help]);
        reportns([NSString stringWithFormat:@ "keys: %d"      , args.keys]);
        reportns([NSString stringWithFormat:@ "direction: %s" , args.desc  ? "desc" : "asc"]);
        reportns([NSString stringWithFormat:@ "open:      %d" , args.open]);
        reportns([NSString stringWithFormat:@ "verbose:   %d" , getVerbose()]);
        reportns([NSString stringWithFormat:@ "version:   %d" , args.version]);
        
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
    args.label   = "Description";
	
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

int main_longopt(int argc,char** argv)
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
