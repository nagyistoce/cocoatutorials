//
//  Printf.cpp
//  This file is part of Atomical
//
//  Atomical is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Atomical is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Atomical.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Fabio Cavaliere, Genova, Italy
//  Additional Engineering by Robin Mills, San Jose, CA, USA. http://clanmills.com
//

#include "Printf.h"
#include <stdlib.h>
#include <stdarg.h>

#ifdef __WINDOWS__
#include <windows.h>
#endif

#if __WINDOWS__
#define vsnprintf _vsnprintf
#endif

#ifdef __WINDOWS__
extern "C" int Printf(const char * format, ... )
{
    char buff[2*4096];
    char*       p = buff;
    va_list     args;
    va_start(args, format);
    int result = vsnprintf(p, sizeof buff - 3, format, args); // -3 for safety
    va_end(args);
    p[result]= 0;
    OutputDebugStringA(buff);

    return result;
}
#endif

extern "C" int System(const char* format, ... )
{
    char buff[2*4096];
    char*       p = buff;
    va_list     args;
    va_start(args, format);
    int result = vsnprintf(buff, sizeof buff, format,args);
    p[result]= 0;
    va_end(args);

#if __WINDOWS__
    WinExec(p,SW_NORMAL);
#else
    result = system(buff);
#endif

    return result;
}

// That's all Folks!
////
