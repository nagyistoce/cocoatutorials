//
//  Qt2.h
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

#ifndef __QT2_H__
#define __QT2_H__
#include <stdio.h>

#define UNUSED(x)x=x
#define ASSERT(t) myAssert(t,__FILE__,__LINE__)

void myAssert(int t,const char* filename,int linenumber);

#ifdef DEFINE_ASSERT
void myAssert(int t,const char* filename,int linenumber)
{
    if ( !t ) {
        printf("****** ASSERT filename = %s linenumber = %d",filename,linenumber);
        exit(1);
    }
}
#endif


#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#endif

// That's all Folks!
////
