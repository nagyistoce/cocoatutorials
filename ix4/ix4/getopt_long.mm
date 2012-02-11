//
//  getopt_long.mm
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "ix4.h"

#if BUILD_WITH == GETOPT_LONG

// http://www.ibm.com/developerworks/aix/library/au-unix-getopt.html
// prototypes
struct Args ;
static void      display_args(Args& args);
static error_e   parseArgs   (Args& args,int argc,char** argv);
static NSArray*  argsToImages(Args& args);

struct Args {
	const char*	 pdf;                    /* -p: option       */
    const char*  label;                  /* -l: option       */
    const char*  sort;                   /* -s: option       */
	NSInteger 	 minsize;				 /* -m: option       */
    NSInteger    resize;                 /* -r: option       */
    int          keys;                   /* -k  option       */
    bool         desc;                   /* -a|-d option     */
    int          open;                   /* -o  option       */
	int 		 verbose;                /* -v  option       */ 
    int          version;                /* -V  option       */
    int          help;                   /* -h  option       */
	char**		 paths;	        		 /* input files      */
	int 		 nPaths;		    	 /* # of input files */
};
static const char*  optString = "p:l:s:m:r:adovVh?";
static const struct option longOpts[] =
{	{ "pdf"         , required_argument	, NULL, 'p' }
,	{ "label"		, required_argument , NULL, 'l' }
,	{ "sort"		, required_argument	, NULL, 's' }
,	{ "minsize"     , required_argument	, NULL, 'm' }
,	{ "resize"      , required_argument	, NULL, 'r' }
,	{ "keys"		, no_argument       , NULL, 'k' }
,	{ "asc"         , no_argument       , NULL, 'a' }
,	{ "desc"		, no_argument       , NULL, 'd' }
,	{ "open"		, no_argument		, NULL, 'o' }
,	{ "verbose"		, no_argument       , NULL, 'v' }
,	{ "version"		, no_argument       , NULL, 'V' }
,	{ "help"		, no_argument		, NULL, 'h' }
,	{ NULL			, no_argument		, NULL,  0  }
};

static void display_args( Args& args )
{
    if ( getVerbose() ) {
        reports([NSString stringWithFormat:@ "pdf:      %s" ,args.pdf]);
        reports([NSString stringWithFormat:@ "label:    %s" ,args.label]);
        reports([NSString stringWithFormat:@ "sort:     %s" ,args.sort]);
        reports([NSString stringWithFormat:@ "minsize:  %ld",args.minsize]);
        reports([NSString stringWithFormat:@ "resize:   %ld",args.resize]);
        reports([NSString stringWithFormat:@ "keys:     %d" ,args.keys]);
        reports([NSString stringWithFormat:@ "asc:      %d" ,args.desc?0:1]);
        reports([NSString stringWithFormat:@ "open:     %d" ,args.open]);
        reports([NSString stringWithFormat:@ "verbose:  %d" ,getVerbose()]);
        reports([NSString stringWithFormat:@ "version:  %d" ,args.version]);
        reports([NSString stringWithFormat:@ "help:     %d" ,args.help]);
        
        for (int i = 0 ; i < args.nPaths ; i++ ) {
            reports([NSString stringWithFormat:@"file %2d = %s",i,args.paths[i]]);
        }
    }
}

static error_e parseArgs(Args& args,int argc,char** argv)
{
    int opt = 0;
	int longIndex = 0;
	
	// Initialize args
    memset(&args,0,sizeof(args));
	args.minsize = 512   ;
    args.label   = "Description";
	
	// Process the arguments with getopt_long()
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
                
			case 'l':
				args.label = optarg;
				break;
                
			case 'o':
				args.open = 1;
				break;
				
			case 'v':
				setVerbose(YES);
                args.verbose = 1;
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
	
	args.paths  = argv + optind;
	args.nPaths = argc - optind;
    
    return error ;
}

int main_longopt(int argc,char** argv)
{
    error = OK;
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    
    // parse the command line
    Args args ;
    parseArgs(args,argc,argv);

    // output to user
    if ( args.verbose ) {
        display_args(args);
    }
    
    if (args.help || args.version )
    {
        if ( args.help    ) display_usage(true);
        if ( args.version ) display_version();
        if ( !error       ) error = OK_ALMOST ; // pretend error
    }
    
    if ( !error && args.nPaths < 1)
    {
        display_usage(false);
        error = e_SYNTAX;
    }
        
    for ( int i = 0 ; !error && i < args.nPaths ; i++ )
        if ( args.paths[i][0] == '-' )
            unrecognizedArgument(args.paths[i]);

    // do the job
    if (!error ) {
        NSArray* images = pathsToImages(args.nPaths,args.paths,args.sort,args.label,args.desc,args.keys,args.minsize);
        imagesToPDF(images,args.pdf,args.open,args.resize);    
    }
    
    [pool release];
    if ( error == OK_ALMOST ) error = OK;
    return error;
}

#endif
