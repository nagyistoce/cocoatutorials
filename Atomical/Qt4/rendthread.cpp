//
//  readthread.cpp
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

#include "Qt4.h"
#include "rendthread.h"

rendthread::rendthread(GLWidget *wglWidget)
: QThread()
{
    moveToThread(this);

    glWidget = wglWidget;
}

rendthread::~rendthread()
{
    delete glWidget;

    free(xx);
    free(yy);
    free(zz);
}

void rendthread::setup(double imb,double sep,double rrsp, int NNp,int NNp2,int mmode)
{
    Np=NNp;
    Np2=NNp2;
    imbalance=imb;
    separation=sep;
    radsp=rrsp;
    mode=mmode;

    printf("Setup called\n");
    printf("%d %d %e %e %e %d\n",Np,Np2,imbalance,separation,radsp,mode);
    xx=(double *)malloc(Np*sizeof(double));
    yy=(double *)malloc(Np*sizeof(double));
    zz=(double *)malloc(Np*sizeof(double));
}

void rendthread::doRend()
{
    start(NormalPriority);
}

void rendthread::run()
{
    forever{
        emit frameNeeded();
//  	Communication is maybe primitive but it works as one can see uncommenting the line below
//       printf("Surprisingly, here %e %e %e\n",xx[Np-1],yy[Np-1],zz[Np-1]);fflush(stdout);
        glWidget->drawGL(xx,yy,zz,imbalance,separation,radsp,Np,Np2,mode);
        mSleep(20); // Roughly 50 FPS
    }
}

void rendthread::loadPositions(double *xxx,double *yyy,double *zzz)
{
    memcpy(xx,xxx,Np*sizeof(double));
    memcpy(yy,yyy,Np*sizeof(double));
    memcpy(zz,zzz,Np*sizeof(double));
}

// That's all Folks!
////
