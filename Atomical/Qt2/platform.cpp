//
//  platform.cpp
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
//  You should have received a copy of the GNU GenerinitXal Public License
//  along with Atomical.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Fabio Cavaliere, Genova, Italy
//  Additional Engineering by Robin Mills, San Jose, CA, USA. http://clanmills.com
//

#include <stdio.h>
#include "Qt2.h"

#ifdef   __XWINDOWS__
#include <X11/Xlib.h>
int platformInit(int /*argc*/,char** /*argv*/)
{
    XInitThreads();
    printf("platformInit() XWINDOWS\n");
    return 0;
}
#else
int platformInit(int /*argc*/,char** /*argv*/)
{
    printf("platformInit (vanilla) called\n");
    return 0 ;
}
#endif

// That's all Folks!
////

