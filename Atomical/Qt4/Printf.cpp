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

#ifdef __WINDOWS__
#include "stdarg.h"
#include <windows.h>
#endif

#ifdef __WINDOWS__
extern "C" int Printf(const char * format, ... )
{
    char buf[2*4096];
    char*       p = buf;
    va_list     args;
    va_start(args, format);
    int result = _vsnprintf(p, sizeof buf - 3, format, args); // -3 for safety
    va_end(args);
    p[result]= 0;
    OutputDebugStringA(buf);

    return result;
}
#endif

extern "C" int System(const char * format, ... )
{
    char buf[2*4096];
    char*       p = buf;
    va_list     args;
    va_start(args, format);
    int result = _vsnprintf(p, sizeof buf - 3, format, args); // -3 for safety
    va_end(args);
    p[result]= 0;

#if __WINDOWS__
    WinExec(p,SW_NORMAL);
#else
    result = system(buf);
#endif

    return result;
}

// That's all Folks!
////
