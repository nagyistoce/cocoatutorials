//
//  ddcli.mm
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

#include "ix4.h"

#if BUILD_WITH == DDCLI
// http://www.dribin.org/dave/blog/archives/2008/04/29/ddcli/
#import "DDCommandLineInterface.h"

@interface IX4App : NSObject <DDCliApplicationDelegate>
{
    BOOL            _verbose;
    BOOL            _version;
    BOOL            _help;
    BOOL            _keys;
    BOOL            _open;
    BOOL            _desc;
    NSString*       _pdf;
    NSInteger       _minsize;
    NSString*       _sort;
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
    {@"verbose"    ,'v',    DDGetoptNoArgument},
    {@"version"    ,'V' ,   DDGetoptNoArgument},
    {@"help"       ,'h',    DDGetoptNoArgument},
    {@"keys"       ,'k',    DDGetoptNoArgument},
    {@"open"       ,'o',    DDGetoptNoArgument},
    {@"asc"        ,'a',    DDGetoptNoArgument},
    {@"desc"       ,'d',    DDGetoptNoArgument},
    {@"pdf"        ,'p',    DDGetoptRequiredArgument},
    {@"minsize"    ,'m',    DDGetoptRequiredArgument},
    {@"sort"       ,'s',    DDGetoptRequiredArgument},
    {@"resize"     ,'r',    DDGetoptRequiredArgument},
    {@"label"      ,'l',    DDGetoptRequiredArgument},
    {nil,            0 ,   (DDGetoptArgumentOptions)0},
    };
    [optionsParser addOptionsFromTable: optionTable];
}

- (id) init;
{
    self = [super init];
    if (self == nil)
        return nil;
    _label = @"Description";
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
    display_usage(ix4,false);
}

- (void) printHelp;
{
    [self printUsage: stdout];
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
@end

int main_ddcli(int,char**)
{
    return DDCliAppRunWithClass([IX4App class]);
}
#endif

