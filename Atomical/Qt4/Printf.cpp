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

// the purpose of Printf is to reroute printf output via OutputDebugString on Windows
extern "C" int Printf(const char * format, ... )
{
    static char buf[2*4096];
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

// That's all Folks!
////
