//
//  OGLView.cpp
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

#include "../Qt2/Qt2.h"
#include "OGLView.h"
/*
#include "globals.h"
#include "ranmar.h"
#include "jacobi.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define random rand
#define srandom srand
*/
#include "window.h"

/**************** Physics *******************/
/*
#define Delta 0.00001

void CPU_energy(const double* xx, const double* yy,const double* zz, double *EE, int N){
        int i,k;
        double x0,y0,x1,y1,z0,z1,pref;

        Emax=0.0;

        for (i=0;i < N;i++){
                EE[i]=0.0;
                x0=xx[i];y0=yy[i];z0=zz[i];
                EE[i]+=x0*x0+y0*y0+z0*z0;
                EE[i]+=0.5*(vxt[i]*vxt[i]+vyt[i]*vyt[i]+vzt[i]*vzt[i]);
                for(k=i+1;k<N;k++){
                        x1=xx[k];y1=yy[k];z1=zz[k];
                        pref=1.0;
                        if(i%2) pref*=imbalance;
                        if(k%2) pref*=imbalance;
                        EE[i]+=pref/sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)+(z1-z0)*(z1-z0));
                }
                if(EE[i]>Emax) Emax=EE[i];
        }
}

double Energy(const double* xx, const double* yy,const double* zz, int N){
        int i;
        double locE;

        locE=0.0;

        CPU_energy(xx, yy, zz, E, Np);

        for(i=0;i<Np;i++) locE+=E[i];

        return locE;

        UNUSED(N);
}

void cshift(int ci,int i,double dd){
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

double HH(int ci,int i,int cj,int j){
        double Epp,Emm,Epm,Emp;

        if(ci==cj && i==j){
                cshift(ci,i,+Delta);
                Epp=Energy(xx,yy,zz,Np);

                cshift(ci,i,-2*Delta);
                Emm=Energy(xx,yy,zz,Np);

                cshift(ci,i,+Delta);
                Epm=Energy(xx,yy,zz,Np);

                return (Epp+Emm-2*Epm)/(Delta*Delta);
        } else {
                cshift(ci,i,+Delta);
                cshift(cj,j,+Delta);
                Epp=Energy(xx,yy,zz,Np);

                cshift(ci,i,-2*Delta);
                cshift(cj,j,-2*Delta);
                Emm=Energy(xx,yy,zz,Np);

                cshift(ci,i,+2*Delta);
                Epm=Energy(xx,yy,zz,Np);

                cshift(ci,i,-2*Delta);
                cshift(cj,j,+2*Delta);
                Emp=Energy(xx,yy,zz,Np);

                cshift(ci,i,+Delta);
                cshift(cj,j,-Delta);

                return 0.25*(Epp+Emm-Epm-Emp)/(Delta*Delta);
        }
}

int encode(int ci,int i){
        return i+(ci-1+mode-2)*Np;;
}

void decode(int ii,int *ci,int *i){
        ldiv_t n;

        n=ldiv(ii,Np);

        *ci=n.quot+3-mode;
        *i=n.rem;
}

void calc_Hessian(double **H){
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

void alloc_Evals(void){
        int i,dim;

        if(allocated_evals) return;

        dim=encode(2,Np);
        olddim=dim;

        Evls=(double *)malloc(dim*sizeof(double));
        Eivs=(double **)malloc(dim*sizeof(double *));
        for(i=0;i<dim;i++) Eivs[i]=(double *)malloc(dim*sizeof(double));

        allocated_evals=1;
}

void free_Evals(void){
        int i,dim;

        if(!allocated_evals) return;

        dim=olddim;
        printf("HERE: dim=%d\n",dim);
        free(Evls);
        for(i=0;i<dim;i++) free(Eivs[i]);
        free(Eivs);

        allocated_evals=0;
}

void Eigenmodes(void){
        int i,j,dim;
        double **H;

        memcpy(ax,xx_old,Np*sizeof(double));
        memcpy(ay,yy_old,Np*sizeof(double));
        memcpy(az,zz_old,Np*sizeof(double));

        printf("Start Eigenmodes()\n");
        dim=encode(2,Np);
        printf("With %d particles in %d dimensions, %d eigenmodes\n",Np,mode,dim);

        alloc_Evals();
        H=(double **)malloc(dim*sizeof(double *));
        for(i=0;i<dim;i++) H[i]=(double *)malloc(dim*sizeof(double));

        calc_Hessian(H);

        for(i=0;i<dim;i++){
                for(j=0;j<dim;j++){
                        printf("%e ",H[i][j]);
                }
                printf("\n");
        }

        printf("Done calculating Hessian\n");

        eigen_driver(H,Eivs,Evls,idx,dim);

        printf("Done calculating Eigenvalues & Eigenvectors\n");
}

void CPU_force(const double* xxx,const double* yyy,const double* zzz,double *ff_x,double *ff_y,double *ff_z,int N){
        int i,j;
        double dd,udd3,pref,xi,xj,yi,yj,zi,zj;

        for(i=0;i<N;i++){
                xi=xxx[i];
                yi=yyy[i];
                zi=zzz[i];

                if(mode==3) ff_x[i]=2*xi;
                ff_y[i]=2*yi; // if sep>0 && Np2>0 fabs!
                ff_z[i]=2*zi;

                for(j=0;j<N;j++){
                        if(j!=i){
                                xj=xxx[j];
                                yj=yyy[j];
                                zj=zzz[j];

                                pref=1.0;
                                if(i%2) pref*=imbalance;
                                if(j%2) pref*=imbalance;
                                dd=sqrt((xi-xj)*(xi-xj)+(yi-yj)*(yi-yj)+(zi-zj)*(zi-zj));

                                udd3=pref/(dd*dd*dd);

                                if(mode==3) ff_x[i]-=(xi-xj)*udd3;
                                ff_y[i]-=(yi-yj)*udd3;
                                ff_z[i]-=(zi-zj)*udd3;
                        }
                }
        }
}

void init_Eigenmode(int n)
{
        int i,ci,ii,dim;
        double coord;

        dim=encode(2,Np);

        printf("%d-->idx[%d]=%d\n",n,n,idx[n]);

        for(i=0;i<dim;i++){
                coord=Eivs[i][2];
                decode(i, &ci, &ii);
                switch (ci) {
                        case 0:
                                xx_old[ii]=ax[ii]+0.1*coord;
                                break;
                        case 1:
                                yy_old[ii]=ay[ii]+0.1*coord;
                                break;
                        case 2:
                                zz_old[ii]=az[ii]+0.1*coord;
                                break;
                        default:
                                break;
                }
        }

        CPU_force(xx_old, yy_old, zz_old, f_x, f_y, f_z, Np);

        for(i=0;i<Np;i++){
                vxt[i]=-0.5*f_x[i]*dtt;
                vyt[i]=-0.5*f_y[i]*dtt;
                vzt[i]=-0.5*f_z[i]*dtt;
        }
}
*/
double CPU_rad(const double* xx, const double* yy,const double* zz, double *rad, int N){
        int i;
        double x0,y0,z0;

        double res=0.0;

        for(i=0;i<N;i++){
                x0=xx[i];y0=yy[i];z0=zz[i];
                rad[i]=sqrt(x0*x0+y0*y0+z0*z0);
                if(rad[i]>res) res=rad[i];
        }

        return res;
}
/*
void CPU_update(const double* oldx, const double* oldy,const double* oldz, double* x, double *y,double *z, int N){
        int i,j;
        double f_x,f_y,f_z,x0,y0,z0,x1,y1,z1,ud,ud3,pref;

        for (i=impurity;i < N;i++) {
                x0=oldx[i];
                y0=oldy[i];
                z0=oldz[i];

                if(mode==3) f_x=-2*x0;
                f_y=-2*y0; // if sep>0 && Np2>0 fabs!
                f_z=-2*z0;

                for(j=0;j<N;j++) {
                        if(j!=i){
                                x1=oldx[j];
                                y1=oldy[j];
                                z1=oldz[j];
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

                if(mode==3) x[i]=oldx[i]+f_x*DSTEP;
                y[i]=oldy[i]+f_y*DSTEP;
                z[i]=oldz[i]+f_z*DSTEP;
        }
}

void CPU_poscpy(double* oldx, double* oldy, double *oldz,const double* newx, const double *newy,const double *newz, int N){
        memcpy(oldx,newx,N*sizeof(double));
        memcpy(oldy,newy,N*sizeof(double));
        memcpy(oldz,newz,N*sizeof(double));
}



double my_rand(double ampl){
        double tmp;
        float ttmp[2];
        ranmar(ttmp,1);
        tmp = 2.0*ampl*(ttmp[1]-0.5);
        return tmp*((double)Np/12.0);;
}

void RandomInit(double* data, int n){
        int i;
        double tmp;

        for (i = 0; i < n; ++i) {
                tmp = my_rand(1);
                data[i] = tmp;
        }
}

void mem_alloc(void){
        idx = (int *)malloc(MaxNp*sizeof(int));
        xx = (double *)malloc(MaxNp*sizeof(double));
        yy = (double *)malloc(MaxNp*sizeof(double));
        zz = (double *)malloc(MaxNp*sizeof(double));
        xx_old = (double *)malloc(MaxNp*sizeof(double));
        yy_old = (double *)malloc(MaxNp*sizeof(double));
        zz_old = (double *)malloc(MaxNp*sizeof(double));
        E = (double *)malloc(MaxNp*sizeof(double));
        rad = (double *)malloc(MaxNp*sizeof(double));
        f_x = (double *)malloc(MaxNp*sizeof(double));
        f_y = (double *)malloc(MaxNp*sizeof(double));
        f_z = (double *)malloc(MaxNp*sizeof(double));
        ax = (double *)malloc(MaxNp*sizeof(double));
        ay = (double *)malloc(MaxNp*sizeof(double));
        az = (double *)malloc(MaxNp*sizeof(double));
        vx = (double *)malloc(MaxNp*sizeof(double));
        vy = (double *)malloc(MaxNp*sizeof(double));
        vz = (double *)malloc(MaxNp*sizeof(double));
        vxt = (double *)malloc(MaxNp*sizeof(double));
        vyt = (double *)malloc(MaxNp*sizeof(double));
        vzt = (double *)malloc(MaxNp*sizeof(double));
        vxn = (double *)malloc(MaxNp*sizeof(double));
        vyn = (double *)malloc(MaxNp*sizeof(double));
        vzn = (double *)malloc(MaxNp*sizeof(double));
}

void mem_free(void){
        free(idx);
        free(xx);
        free(yy);
        free(zz);
        free(xx_old);
        free(yy_old);
        free(zz_old);
        free(E);
        free(rad);
        free(f_x);
        free(f_y);
        free(f_z);
        free(ax);
        free(ay);
        free(az);
        free(vx);
        free(vy);
        free(vz);
        free(vxn);
        free(vyn);
        free(vzn);
        free(vxt);
        free(vyt);
        free(vzt);
}

void init_double_layer(int Ndl){
        int i;
        if(Np2>0){
                if(Np2>Np) return;
                for(i=0;i<Ndl;i++){
                        xx_old[i]=-separation;
                        xx[i]=-separation;

                }
                for(i=Ndl;i<Np;i++){
                        xx_old[i]=+separation;
                        xx[i]=separation;
                }

        }
}

void Initialize(void){
        int i;
        float tmp[4];
        double rr,th,ph;

        if(Np>MaxNp) return;

        free_Evals();

        for(i=0;i<MaxNp;i++){
                xx[i]=0.0;
                yy[i]=0.0;
                zz[i]=0.0;
                xx_old[i]=0.0;
                yy_old[i]=0.0;
                zz_old[i]=0.0;
                E[i]=0.0;
                rad[i]=0.0;
                vxt[i]=0.0;
                vyt[i]=0.0;
                vzt[i]=0.0;
        }

        converged=0;
        myE=1e98;
        myEold=1e99;
        do_update=1;
        dperc=1.0;
        DSTEP=0.01;

        srandom(time(NULL));
        if(mode==2) resc=sqrt((double)Np);
        if(mode==3) resc=pow((double)Np,1./3.);

        for(i=0;i<Np;i++){
            int I = i ; // store i - ranmar is clobbering him on Windows!
                ranmar(tmp, 4);
            i = I;
                rr=tmp[1]*resc;
                th=tmp[2]*2*M_PI;
                ph=tmp[3]*M_PI;
                if(mode==2){
                        xx_old[i]=0.0;
                        yy_old[i]=rr*cos(th);
                        zz_old[i]=rr*sin(th);
                } else {
                        xx_old[i]=rr*cos(ph);
                        yy_old[i]=rr*cos(th)*sin(ph);
                        zz_old[i]=rr*sin(th)*sin(ph);
                }
        }

        if(mode==2) init_double_layer(Np2);

        verlet=0;
        converged=0;
        is_frozen=0;

#if 0
        if(mode==2) {
                fog_off();
        } else {
                fog_on(0.05);
        }
#endif
}

*/

// Assuming that all the calculations are done in glWidget, this method needs as arguments
//
// double *xxx, *yyy, *zzz, *rad <--- DONE
// double imbalance, radsp
// int Np, Np2, mode
//

void OGLView::drawRect(double *xxx,double *yyy,double *zzz,double sep, double imb, double rrsp, int NNp, int NNp2, int mmode) const
{
        int i,NNpin;
        float alpha;
        double rradius;
        double *rrad;

        rrad=(double *)malloc(NNp*sizeof(double));

        rradius=CPU_rad(xxx, yyy, zzz, rrad, NNp);
/*
        printf("drawRect()\n");
        printf("Np=%d mode=%d\n",NNp,mmode);
        printf("[***] %e %e %e\n",xxx[0],yyy[0],zzz[0]);
*/
	GLUquadric *glq;
        alpha=0.9;
	glq=gluNewQuadric();

        NNpin=0;
		
        for(i=0;i<NNp;i++){
                glPushMatrix();
                GLfloat e=(GLfloat)(rrad[i]/rradius);
                if(mmode==2 && NNp2>0 && sep>0){
                        if(xxx[i]>0.0){
                                glColor4f(.5+.5*e,0,0,alpha);
                        } else {
                                glColor4f(0,0,.5+.5*e,alpha);
                        }
				
                } else {
                        if(i%2 && imb>1.0) glColor4f(e,0.2+0.8*(imb-1)/9,(1-e),alpha); else glColor4f(e,0,(1-e),alpha);
                }

//                if(!allocated_evals && verlet && i==NNp-1) glColor4f(0,1,0,alpha); // Bullet color

                glTranslatef(xxx[i], yyy[i], zzz[i]);
                if(i%2) gluSphere(glq, 0.2+0.2*(imb-1)/4, 40, 40); else gluSphere(glq, 0.2, 40, 40);
                if(rrad[i]<=rrsp) NNpin++;
                glPopMatrix();
	}

	glPushMatrix();
	glColor4f((GLfloat)1,(GLfloat)1,(GLfloat)0,(GLfloat)0.7);
	glTranslatef(0, 0, 0);
        gluSphere(glq, rrsp, 40, 40);
	glPopMatrix();
	
	gluDeleteQuadric(glq);
	
        glFlush();	// Single buffering...

        free(rrad);
}

/*
void OGLView::oneShot()
{
    CPU_update(xx_old, yy_old,zz_old, xx, yy, zz, Np);
    CPU_poscpy(xx_old, yy_old,zz_old, xx, yy, zz, Np);
    CPU_energy(xx, yy, zz, E, Np);
}

void OGLView::oneShot2(){
    drawRect(xx, yy, zz, imbalance, radsp, Np, Np2, mode); // This call will be in glWidget
}
*/

OGLView::OGLView(QObject *parent)
    : QObject(parent)
{
    /*
    MaxNp=5001;
    mem_alloc();

    mode=3;
    Np=96;
    Np2=0;

    separation=0.0;
    imbalance=1.0;

    PREC=1e-11;
    radsp=0.0;

    srand(time(NULL));
    rmarin(rand()%31329,rand()%30081);

    Initialize();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(oneShot()));
    timer->start(16);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(oneShot2()));
    timer->start(0);
    */
}

OGLView::~OGLView()
{
}

void OGLView::setColor(QColor c)
{
    UNUSED(c);
}

void OGLView::draw(double *xxx, double *yyy, double *zzz, double sep, double imb, double rrsp, int NNp, int NNp2, int mmode) const // (xx, yy, zz,imb, rrsp, NNp, NNp2, mmode)
{
//    printf("draw() %e %e %e\n",xxx[0],yyy[0],zzz[0]);
    drawRect(xxx, yyy, zzz, sep, imb, rrsp, NNp, NNp2, mmode);
}

// That's all Folks!
////
