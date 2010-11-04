//
//  calcthread.cpp
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

#include "Qt2.h"
#include "calcthread.h"

calcThread::calcThread()
: QThread()
{
    moveToThread(this); // Do I need this?
}

void calcThread::__CPU_update()
{
	int i,j;
	double f_x,f_y,f_z,x0,y0,z0,x1,y1,z1,ud,ud3,pref;
	double DSTEP=0.01;

	for (i=0;i < Np;i++) {
		x0=xx_old[i];
		y0=yy_old[i];
		z0=zz_old[i];

		f_x=0.0;
		f_y=0.0;
		f_z=0.0;

		if(mode==3) f_x=-2*x0;
		f_y=-2*y0; // if sep>0 && Np2>0 fabs!
		f_z=-2*z0;

		for(j=0;j<Np;j++) {
			if(j!=i) {
				x1=xx_old[j];
				y1=yy_old[j];
				z1=zz_old[j];
				pref=1.0;
				if(i%2) pref*=imbalance;
				if(j%2) pref*=imbalance;
				ud=1.0/sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)+(z1-z0)*(z1-z0));
				ud3=pref*ud*ud*ud;
				if(mode==3) f_x+=(x0-x1)*ud3;
				f_y+=(y0-y1)*ud3;
				f_z+=(z0-z1)*ud3;
			}
		}

		if(mode==3) xx[i]=xx_old[i]+f_x*DSTEP;
		yy[i]=yy_old[i]+f_y*DSTEP;
		zz[i]=zz_old[i]+f_z*DSTEP;
	}
}

double calcThread::__CPU_energy()
{
	int i,k;
	double EE,x0,y0,x1,y1,z0,z1,pref,res;

	res=0.0;

	for (i=0;i < Np;i++) {
		EE=0.0;
		x0=xx[i];y0=yy[i];z0=zz[i];
		EE+=x0*x0+y0*y0+z0*z0;
		for(k=i+1;k<Np;k++) {
			x1=xx[k];y1=yy[k];z1=zz[k];
			pref=1.0;
			if(i%2) pref*=imbalance;
			if(k%2) pref*=imbalance;
			EE+=pref/sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)+(z1-z0)*(z1-z0));
		}
		res+=EE;
	}

	return res;
}

void calcThread::__CPU_poscpy()
{
	memcpy(xx_old,xx,Np*sizeof(double));
	memcpy(yy_old,yy,Np*sizeof(double));
	memcpy(zz_old,zz,Np*sizeof(double));
}


// This stuff is based on the Mandelbrot example

void calcThread::setup(double *xxx,double *yyy,double *zzz,double imb,double tprec,int NNp,int NNp2,int mmode)
{
    int i;

    printf("IN SETUP\n");

    Np=NNp;
    Np2=NNp2;
    imbalance=imb;
    targetPrecision=tprec;
    mode=mmode;
    do_calc=1;

    printf("In calcThread::setup(), mode=%d\n",mode);

    for(i=0;i<Np;i++){
        xx[i]=xxx[i];
        yy[i]=yyy[i];
        zz[i]=zzz[i];
    }

    __CPU_poscpy();
}

void calcThread::run()
{
    double oldE=1e99;

    forever {
        if(do_calc){
            __CPU_update();
            E=__CPU_energy();
            __CPU_poscpy();
        } else {
            mSleep(20);
        }
        if(fabs(E-oldE)<targetPrecision) {
            printf("In calcThread::run(), converged!\n");
            do_calc=0;
            oldE=1e99;
        }
        if(do_calc) oldE=E;
        emit stepDone(xx,yy,zz/*,&E*/);
        mSleep(2);
    }
}

void calcThread::doCalc(){
    start(HighPriority);
}

// That's all Folks!
////
