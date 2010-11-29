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

#include "Qt4.h"
#include "calcthread.h"
#include <stdlib.h>

calcThread::calcThread(int maxNp,int aEigenModeMaxNp)
: QThread()
{
    Np=0;
    nMaxNp=maxNp;
    eigenModeMaxNp=aEigenModeMaxNp;
    calculateEigenmode=-1;
    lastNp = -1;
    lastMode = -1;
    lastDim = -1;

    int i,dim;

    xx     = new double[maxNp];
    yy     = new double[maxNp];
    zz     = new double[maxNp];
    xx_old = new double[maxNp];
    yy_old = new double[maxNp];
    zz_old = new double[maxNp];
    ax     = new double[maxNp];
    ay     = new double[maxNp];
    az     = new double[maxNp];

    dim=303; // Hard wired: eigenmodes for at most 100 particles

    Evls=new double[dim];

    Eivs=new double *[dim];
    H=new double *[dim];
    m2=new double *[dim];

    if(H==NULL) Printf("WOE!!!\n");

    for(i=0;i<dim;i++) {
        Eivs[i]=new double [dim];
        H[i]=new double [dim];
        m2[i]=new double [dim];

        if(H[i]==NULL) Printf("WOE!!!\n");
    }

    n_eigmode=-1; // When set to -1, the loop calculates relaxation.

    moveToThread(this); // Do I need this?
}

calcThread::~calcThread()
{
    int i,dim;

    delete [] xx     ;
    delete [] yy     ;
    delete [] zz     ;
    delete [] xx_old ;
    delete [] yy_old ;
    delete [] zz_old ;
    delete [] ax     ;
    delete [] ay     ;
    delete [] az     ;

    dim=303;

    for(i=0;i<dim;i++) {
        delete [] Eivs[i];
        delete [] H[i];
        delete [] m2[i];
    }
    delete [] Eivs;
    delete [] H;
    delete [] m2;

    delete [] Evls;
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

// Eigenvalue problem, needed for the vibrational modes
void calcThread::eigsrt(double *d, double **v, int n)
{
    int k,j,i;
    double p;

    for (i=0;i<n;i++) {
        p=d[k=i];
        for (j=i;j<n;j++)
            if (d[j] >= p) p=d[k=j];
        if (k != i) {
            d[k]=d[i];
            d[i]=p;
            for (j=0;j<n;j++) {
                p=v[i][j];
                v[i][j]=v[k][j];
                v[k][j]=p;
            }
        }
    }
}

void calcThread::twst(double **m, int i,int j,double cc,double ss,int n)
{
    int k=n;
    double t;

    while(k--){
        t = cc*m[i][k]+ss*m[j][k];
        m[j][k] = -ss*m[i][k]+cc*m[j][k];
        m[i][k] = t;
    }
}

void calcThread::eigen(int n)
{
//void calcThread::eigen(double **m, double *l, double **vc, double **m2, int n){
//                              H       Evls          Eivs          m2     dim
    double q,mod,t,th,cc,ss;
    int i,j,k,mm;

    i=n;
    while(i--) {
        j=n;
        while(j--) m2[i][j] = H[i][j];
    }

    i=n;
    while(i--) {
        j=n;
        while(j--) Eivs[i][j] = i==j; // Was [i][j] but think it's transposed that way
    }

    while(1){
        mod = 0;
        i=0,
        j=0;

        k=n;
        while(k--){
            mm=n;
            while((--mm)>k){
                q = fabs(m2[k][mm]);
                if(q > mod) {
                    mod=q;
                    i=k;
                    j=mm;
                }
            }
        }

        if(mod < 0.00000000001) break;

        th = 0.5*atan(2*m2[i][j]/(m2[i][i] - m2[j][j]));

        cc = cos(th);
        ss = sin(th);

        k=n;
        while(k--){
            t = cc*m2[k][i] + ss*m2[k][j];
            m2[k][j] = -ss*m2[k][i] + cc*m2[k][j];
            m2[k][i]=t;
        }
        twst(m2,i,j,cc,ss,n);
        twst(Eivs,i,j,cc,ss,n);
    }

    j=n;
    while(j--){
        Evls[j] = m2[j][j];
    };
}

void calcThread::eigen_driver(int dim){
    int i,j;

    eigen(dim);
#if 0
    Printf("\n\n--- eigen_driver() ---\n");
    for(int i=0;i<dim;i++) {
        if(fabs(Evls[i])<1e-5) Evls[i]=0;
        Evls[i]=sqrt(fabs(Evls[i]));
    }
#endif
    eigsrt(Evls,Eivs,dim);

    double tmp;
    for(i=0;i<dim;i++){
        for(j=i+1;j<dim;j++){
            tmp=Eivs[i][j];
            Eivs[i][j]=Eivs[j][i];
            Eivs[j][i]=tmp;
        }
    }
}

// Code for the vibrational modes calculation
void calcThread::cshift(int ci,int i,double dd)
{
    switch(ci){
        case 0:
            xx[i]+=dd;
            break;
        case 1:
            yy[i]+=dd;
            break;
        case 2:
            zz[i]+=dd;
            break;
        default:
            break;
    }

}

double calcThread::HH(int ci,int i,int cj,int j)
{
    double Epp,Emm,Epm,Emp;
    double Delta=0.00005;

    // Ah! The good old five-points stencil :)

    if(ci==cj && i==j){
        cshift(ci,i,+Delta);
        Epp=__CPU_energy();

        cshift(ci,i,-2*Delta);
        Emm=__CPU_energy();

        cshift(ci,i,+Delta);
        Epm=__CPU_energy();

        return (Epp+Emm-2*Epm)/(Delta*Delta);
    } else {
        cshift(ci,i,+Delta);
        cshift(cj,j,+Delta);
        Epp=__CPU_energy();

        cshift(ci,i,-2*Delta);
        cshift(cj,j,-2*Delta);
        Emm=__CPU_energy();

        cshift(ci,i,+2*Delta);
        Epm=__CPU_energy();

        cshift(ci,i,-2*Delta);
        cshift(cj,j,+2*Delta);
        Emp=__CPU_energy();

        cshift(ci,i,+Delta);
        cshift(cj,j,-Delta);

        return 0.25*(Epp+Emm-Epm-Emp)/(Delta*Delta);
    }
}

int calcThread::encode(int ci,int i)
{
    return i+(ci-1+mode-2)*Np;;
}

void calcThread::decode(int ii,int *ci,int *i)
{
    ldiv_t n;

    n=ldiv(ii,Np);

    *ci=n.quot+3-mode;
    *i=n.rem;
}

void calcThread::calc_Hessian(void)
{
    int dim,ii,jj;
    int ci,i,cj,j;
    double tmp;

    dim=encode(2,Np);

    for(ii=0;ii<dim;ii++){
        decode(ii,&ci,&i);
        H[ii][ii]=HH(ci, i, ci, i);
        for(jj=ii+1;jj<dim;jj++){
            decode(jj,&cj,&j);
            tmp=HH(ci, i, cj, j);
            H[ii][jj]=tmp;
            H[jj][ii]=tmp;
        }
    }
}

void calcThread::alloc_Evals(void)
{
    int i,dim;

    if(allocated_evals) return;

    dim=encode(2,Np);
    olddim=dim;

    Evls=new double[dim];

    Eivs=new double *[dim];
    H=new double *[dim];
    m2=new double *[dim];

    if(H==NULL) Printf("WOE!!!\n");

    for(i=0;i<dim;i++) {
        Eivs[i]=new double [dim];
        H[i]=new double [dim];
        m2[i]=new double [dim];

        if(H[i]==NULL) Printf("WOE!!!\n");
    }
/*
    Evls=(double *)malloc(dim*sizeof(double));
    Eivs=(double **)malloc(dim*sizeof(double *));
    H=(double **)malloc(dim*sizeof(double *));
    m2=(double **)malloc(dim*sizeof(double *));

    for(i=0;i<dim;i++) {
        Eivs[i]=(double *)malloc(dim*sizeof(double));
        H[i]=(double *)malloc(dim*sizeof(double));
        m2[i]=(double *)malloc(dim*sizeof(double));
    }
*/
    allocated_evals=true;
}

void calcThread::free_Evals(void){
    int i;

    if(!allocated_evals) return;

    for(i=0;i<olddim;i++) {
        delete [] Eivs[i];
        delete [] H[i];
        delete [] m2[i];
    }
    delete [] Eivs;
    delete [] H;
    delete [] m2;

    delete [] Evls;

/*
    dim=olddim;

    free(Evls);
    for(i=0;i<dim;i++) {
        free(Eivs[i]);
        free(H[i]);
        free(m2[i]);
    }
    free(Eivs);
    free(H);
    free(m2);
*/
    allocated_evals=false;
    n_eigmode=-1;
}

int calcThread::maxEigenmode(){
    return encode(2,Np)-1;
}

void calcThread::chooseEigenmode(int n)
{
    int nn=n;
    if(n<0 || n>encode(2,Np)-1) nn=0; // Default to the ground state
    n_eigmode=encode(2,Np)-1-nn; // Internal sort of eigenmodes is reversed
    faket=0.0;
    Printf("n_eigmode=%d\n",n_eigmode);
#if 0
    Printf("//------ %d: %e ------//\n",n_eigmode,Evls[n_eigmode]);
    for(int j=0;j<encode(2,Np);j++){
        Printf("%e\n",Eivs[j][n_eigmode]);
    }
    Printf("\n");
#endif
}

void calcThread::stopEigenmodes(void)
{
    pause();
    msleep(1);
//  free_Evals();
    n_eigmode=-1;
    resume(); // Resumes in relaxation mode
}

void calcThread::startEigenmodes(int nn)
{
    calculateEigenmode = nn;
}

void calcThread::calculateEigenmodes(int nn)
{
    if(Np>eigenModeMaxNp) return;

    int dim;
    pause();

    Printf("Start Eigenmodes()\n");
    dim=encode(2,Np);
    Printf("With %d particles in %d dimensions, %d eigenmodes\n",Np,mode,dim);
    if ( lastNp != Np || lastMode != mode || lastDim != dim ) {

        memcpy(ax,xx_old,Np*sizeof(double));
        memcpy(ay,yy_old,Np*sizeof(double));
        memcpy(az,zz_old,Np*sizeof(double));
/*
        H=(double **)malloc(dim*sizeof(double *));
        for(i=0;i<dim;i++) H[i]=(double *)malloc(dim*sizeof(double));
        m2=(double **)malloc(dim*sizeof(double *));
        for(i=0;i<dim;i++) m2[i]=(double *)malloc(dim*sizeof(double));
*/
        if(H==NULL) Printf("Oh my gosh!\n");
        calc_Hessian();
#if 0
        for(int i=0;i<dim;i++){
            for(int j=0;j<dim;j++){
                Printf("%e ",H[i][j]);
            }
            Printf("\n");
        }
        Printf("Done calculating Hessian\n");
#endif

        eigen_driver(dim);
#if 0
        Printf("Done calculating Eigenvalues & Eigenvectors\n");
        for(int i=0;i<dim;i++){
            Printf("//------ %d: %e ------//\n",i,Evls[i]);
            for(int j=0;j<dim;j++){
                Printf("%e\n",Eivs[j][i]);
            }
            Printf("\n");
        }
#endif
        lastNp   = Np;
        lastMode = mode;
        lastDim  = dim ;
    }
    chooseEigenmode(nn);

    emit calculateEigenmodesDone();
    resume();
}

// This stuff is based on the Mandelbrot example

void calcThread::setup(double *xxx,double *yyy,double *zzz,double imb,double tprec,int NNp,int NNp2,int mmode)
{
    int i;

    Printf("IN SETUP\n");

    Np=NNp;
    Np2=NNp2;
    imbalance=imb;
    targetPrecision=tprec;
    mode=mmode;
    do_calc=1;

    Printf("In calcThread::setup(), mode=%d\n",mode);

    for(i=0;i<Np;i++){
        xx[i]=xxx[i];
        yy[i]=yyy[i];
        zz[i]=zzz[i];
    }

    __CPU_poscpy();
}

void calcThread::pause()
{
    isPaused=true;
}

void calcThread::resume()
{
    isPaused=false;
}

void calcThread::run()
{
    double amp=0.3;
    double coord;
    int dim,j,ci,ii;

    forever {
        if (                    calculateEigenmode >= 0 ) {
            calculateEigenmodes(calculateEigenmode);
            Printf("calcThread::calculateEigenmodes done\n");
                                calculateEigenmode = -1 ;
        }
        // Here, insert logic to deal with eigenmodes...
        if(n_eigmode==-1){
            if(do_calc && !isPaused){
                __CPU_update();
                E=__CPU_energy();
                __CPU_poscpy();
            } else {
                mSleep(20);
            }

            if(fabs(E-oldE)<targetPrecision) {
                Printf("In calcThread::run(), converged!\n");
                do_calc=0;
                oldE=1e99;
                emit isConverged();
            }
            if(do_calc) oldE=E;
        } else {
            dim=encode(2, Np);
            for(j=0;j<dim;j++){
                coord=Eivs[j][n_eigmode];
                decode(j, &ci, &ii);
                switch (ci) {
                    case 0:
                        xx[ii]=ax[ii]+amp*coord*sin(0.01*faket);
                        break;
                    case 1:
                        yy[ii]=ay[ii]+amp*coord*sin(0.01*faket);
                        break;
                    case 2:
                        zz[ii]=az[ii]+amp*coord*sin(0.01*faket);
                        break;
                    default:
                        break;
                }

            }
            faket+=1.0;
        }
        emit stepDone(xx,yy,zz,&E);
        mSleep(2);
    }
}

void calcThread::doCalc()
{
    start(HighPriority);
}

// That's all Folks!
////
