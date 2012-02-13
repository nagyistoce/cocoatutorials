//
//  IO.cpp
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

#include <iostream>
#include "ix4.h"

static boolean_t gbVerbose = NO ;

void setVerbose(boolean_t verbose)
{
    gbVerbose = verbose;
}

boolean_t getVerbose() { return gbVerbose ; }

void warns(const char* msg)
{
    if ( gbVerbose ) {
        errors(msg);
    }
}

void warns(NSString* msg)
{
    warns([msg UTF8String]);
}

void errors(const char* msg)
{
    if ( !error ) {
        fputs(msg,stderr);
        fputc('\n',stderr);
    }
}

void errors(NSString* msg)
{
    errors([msg UTF8String]);
}

void reports(const char* msg)
{
    puts(msg);
}

void reports(NSString* msg)
{
    reports([msg UTF8String]);
}

void whispers(const char* msg)
{
    if ( gbVerbose ) {
        puts(msg);
    }
}

void whispers(NSString* msg)
{
    whispers([msg UTF8String]);
}

// That's all Folks!
////