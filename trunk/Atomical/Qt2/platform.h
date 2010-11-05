//
//  platform.h
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

#pragma once
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef  __LINUX__
#ifdef  __APPLE__
#undef  __LINUX__
#endif
#endif

#ifdef  __LINUX__
#define __XWINDOWS__
#endif
#ifdef __WINDOWS__
#define random rand
#define srandom srand
#endif
#define UNUSED(x)x=x
#define mSleep(milli)  usleep(milli*1000)
#define lengthof(x)    sizeof(x)/sizeof(x[0])

int platformInit(int argc,char** argv);

#endif

// That's all Folks!
////
