//
//  platform.cpp
//  This file is part of QtSee
//
//  QtSee is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  QtSee is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with QtSee.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Alan Mills, Camberley, England
//         and Robin Mills, San Jose, CA, USA
//  http://clanmills.com
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "platform.h"

#ifdef __WINDOWS__
#include <windows.h>
#endif

static const char* platformName =
#if __WINDOWS__
#define vsnprintf _vsnprintf
"Microsoft Windows";
#endif
#if __APPLE__
"MacOSX";
#endif
#ifdef   __XWINDOWS__
"XWindows";
#endif

#ifdef   __XWINDOWS__
#include <X11/Xlib.h>
#else
static void XInitThreads() { return ; }
#endif


int platformInit(int argc,char** argv)
{
    XInitThreads();
    setbuf(stdout,NULL);  // don't buffer stdout so we don't need fflush(stdout) calls!
    Printf("platformInit() %s\n",platformName);
    return 0;

    UNUSED(argc);
    UNUSED(argv);
}

#ifdef __WINDOWS__
extern "C" int Printf(const char * format, ... )
{
    char buff[128*1024];
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

extern "C" int Println(const char* s) { return Printf("%s\n",s); }

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

static const char* OPEN =
#if __WINDOWS__
"explorer";
#elif __APPLE__
"open";
#else
"xdg-open";
#endif

extern "C" void LinkActivated(const char* data)
{
    Printf("linkActivated: %s\n",data);
    System("%s %s\n",OPEN,data);
}

extern "C" const char* SS(QString s)
{
    return (const char*) s.toAscii() ;
}


// That's all Folks!
////
