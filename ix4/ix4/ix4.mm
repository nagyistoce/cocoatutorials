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

#include "ix4.h"

// horrible global variables
error_e     error ;
const char* ix4 = "ix4";

#if BUILD_WITH == DDCLI 
const char* parser = "ddcli";
#endif
#if BUILD_WITH == GETOPT_LONG
const char* parser = "getopt_long";
#endif

void display_usage(const char* program,bool bAll)
{
	if ( bAll ) reportns([NSString stringWithFormat:@"%s - convert images to PDF",program]);
	report("usage: [ --[ pdf path | label key | sort key | minsize n | FLAG]   | file ]+");
    report("FLAG: { help | keys | asc | desc | open | verbose | version }");
	exit( EXIT_FAILURE );
}

void display_version( void )
{
	reportns( [NSString stringWithFormat:@"ix4: %d.%d parser: %s built:%s %s"
               ,VERSION_MAJOR,VERSION_MINOR,parser,__TIME__,__DATE__]
            );
	exit( EXIT_FAILURE );
}

void unrecognizedArgument(const char* arg)
{
    errorns([NSString stringWithFormat:@"%unrecognized argument: %s",arg]);
    error = e_UNRECOGNIZED_ARG ;
}

int main (int argc,char** argv)
{
#if BUILD_WITH == GETOPT_LONG
    return main_longopt(argc,argv);
#endif
#if BUILD_WITH == DDCLI
    return main_ddcli(argc,argv);
#endif
    return 1;
}


