//
//  platform.h
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
//         and Robin Mills, San Jose, CA, USA.
//  http://clanmills.com
//

#pragma once
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <QtCore>

#ifdef  __LINUX__
#ifdef  __APPLE__
#undef  __LINUX__
#endif
#endif

#ifdef  __LINUX__
#define __XWINDOWS__
#endif

#ifdef  __WINDOWS__
#include <windows.h>
#define random   rand
#define srandom srand
#endif

#define UNUSED(x)x=x
#define lengthof(x)    ((int)sizeof(x)/(int)sizeof(x[0]))

#ifdef  __WINDOWS__
#define mSleep(milli)  Sleep(milli)
#else
#define mSleep(milli)  usleep(milli*1000)
#endif

// Printf - reroute printf output via OutputDebugString on Windows
#ifdef __WINDOWS__
extern "C" int Printf(const char * format, ... );
#else
#define Printf printf
#endif

extern "C" const char* SS(QString s);
extern "C" int  System(const char * format, ... );
extern "C" void LinkActivated(const char* data);
int             platformInit(int argc,char** argv);

#endif

// That's all Folks!
////
