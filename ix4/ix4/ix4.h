//
//  ix4.h
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

#ifndef _ix4_h
#define _ix4_h

#include "PDFImages.h"
#include "IO.h"

#define    GETOPT_LONG   1
#define    DDCLI         2
#define    BUILD_WITH    2

#define    VERSION_MAJOR 0
#define    VERSION_MINOR 2

typedef enum error_e
{   OK_ALMOST=-1
,   OK=0 
,	e_SYNTAX=10 
,   e_IMAGE_BAD
,	e_NO_IMAGES
,   e_ALLOC_FAILED
,	e_PDF_CANT_WRITE
,   e_ARG_UNRECOGNISED
,   e_TASK_FAILED
} error_e;

// horrible global variables
extern error_e     error;
extern const char* ix4;

#ifndef lengthof
#define lengthof(x) sizeof(x)/sizeof(x[0])
#endif

void display_usage(bool bAll);
void display_version( void );
void unrecognizedArgument(NSString* arg);
void unrecognizedArgument(const char* arg);

int  main_longopt(int argc,char** argv);
int  main_ddcli  (int,char**);

#endif

// That's all Folks!
////
