//
//  OGLView.m
//  Cocoa_GL
//
//  Created by Fabio Cavaliere on 05/10/10.
//  Copyright 2010 Dipartimento di Fisica. All rights reserved.
//
#include "../Qt2/Qt2.h"
#include "OGLView.h"
#include "globals.h"
#include "ranmar.h"
#include "jacobi.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define random rand
#define srandom srand

#include "window.h"

/**************** Physics *******************/

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
                        EE[i]+=pref/sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)+(z1-z0)*(z1-z0)+delta2);
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

void CPU_rad(const double* xx, const double* yy,const double* zz, int N){
        int i;
        double x0,y0,z0;

        radius=0.0;

        //	#pragma omp strict parallel for
        for(i=0;i<N;i++){
                x0=xx[i];y0=yy[i];z0=zz[i];
                rad[i]=sqrt(x0*x0+y0*y0+z0*z0);
                if(rad[i]>radius) radius=rad[i];
        }
}

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
                                ud=1.0/sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)+(z1-z0)*(z1-z0)+delta2);
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
                ranmar(tmp, 4);
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

/*
void motion(int x, int y)
{
	camYaw += 0.02*(GLfloat)(mx-x);
	
	if(camYaw < 0.0)
		camYaw = 6.28+camYaw;
	else if(camYaw > 6.28)
		camYaw = camYaw-6.28;
	
	camPitch += 0.02*(GLfloat)(y-my);
	
	if(camPitch > 1.57)
		camPitch = 1.57;
	else if(camPitch < -1.57)
		camPitch = -1.57;
	
	mx = x;
	my = y;
}
*/

void OGLView::drawRect(/*NSRect* dirtyRect*/) const
{
	int i;
        float alpha;
//      float x0,y0,z0,x1,y1,z1,nrm;
//	GLfloat e,camX,camY,camZ,camRadius=5.0;
//	[[self openGLContext] makeCurrentContext];

        CPU_rad(xx, yy, zz, Np);
	GLUquadric *glq;
	alpha=ALPHAVAL;
	glq=gluNewQuadric();
/*
	if(verlet && !allocated_evals && POV) {
		nrm=sqrt(vxt[Np-1]*vxt[Np-1]+vyt[Np-1]*vyt[Np-1]+vzt[Np-1]*vzt[Np-1]);
		if(mode==2) x0=xx_old[Np-1]+1; else x0=xx_old[Np-1]-1.5*vyt[Np-1]/nrm;
		y0=yy_old[Np-1]-1.5*vyt[Np-1]/nrm;
		z0=zz_old[Np-1]-1.5*vzt[Np-1]/nrm;
		x1=xx_old[Np-1]+vxt[Np-1]/nrm;		
		y1=yy_old[Np-1]+vyt[Np-1]/nrm;
		z1=zz_old[Np-1]+vzt[Np-1]/nrm;
		gluLookAt( x0, y0, z0,  x1, y1, z1,  1.0, 0.0, 0.0);
	} else {
		gluLookAt( camX, camY, camZ,  0.0, 0.0, 0.0,  0.0, 0.0, 1.0);		
	}
*/
	Npin=0;
		
        for(i=0;i<Np;i++){
			glPushMatrix();
                        GLfloat e=(GLfloat)(rad[i]/radius);
			if(mode==2 && Np2>0 && separation>0){
				if(xx_old[i]>0.0){
					glColor4f(.5+.5*e,0,0,alpha);
				} else {
					glColor4f(0,0,.5+.5*e,alpha);
				}

				
			} else {
				if(i%2 && imbalance>1.0) glColor4f(e,0.2+0.8*(imbalance-1)/9,(1-e),alpha); else glColor4f(e,0,(1-e),alpha);								
			}

			if(!allocated_evals && verlet && i==Np-1) glColor4f(0,1,0,alpha);
			
			glTranslatef(xx_old[i], yy_old[i], zz_old[i]);
                        if(i%2) gluSphere(glq, 0.2+0.2*(imbalance-1)/4, 40, 40); else gluSphere(glq, 0.2, 40, 40);
			if(rad[i]<=radsp){
				Npin++;
			}		
			glPopMatrix();
	}

	glPushMatrix();
	glColor4f((GLfloat)1,(GLfloat)1,(GLfloat)0,(GLfloat)0.7);
	glTranslatef(0, 0, 0);
	gluSphere(glq, radsp, 40, 40);		
	glPopMatrix();
	
	/*
	if(Np2>0&&separation>0){
		glPushMatrix();
		glColor4f(0, 0, 1, 0.4);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, -separation);
		gluDisk(glq,0.0,1.1*radius,40,1);
		glPopMatrix();
		
		glPushMatrix();
		glColor4f(1, 0, 0, 0.4);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, separation);
		gluDisk(glq,0.0,1.1*radius,40,1);
		glPopMatrix();		
	}
	*/
	gluDeleteQuadric(glq);
	
        glFlush();	// Single buffering...
}

void OGLView::oneShot()
{
    CPU_update(xx_old, yy_old,zz_old, xx, yy, zz, Np);
    CPU_poscpy(xx_old, yy_old,zz_old, xx, yy, zz, Np);
    CPU_energy(xx, yy, zz, E, Np);
    drawRect();
}

void OGLView::oneShot2(){
//    CPU_update(xx_old, yy_old,zz_old, xx, yy, zz, Np);
//    CPU_poscpy(xx_old, yy_old,zz_old, xx, yy, zz, Np);
//    CPU_energy(xx, yy, zz, E, Np);
    drawRect();
}

OGLView::OGLView(QObject *parent)
    : QObject(parent)
{
    MaxNp=5001;
    mem_alloc();

    delta2=0.0;
//    angle=60.0;
    mode=3;
    Np=96;

    Np2=0;
    separation=0.0;
    PREC=1e-11;
    //distance=15;

    radsp=0.0;
    imbalance=1.0;

    srand(time(NULL));
    rmarin(rand()%31329,rand()%30081);

    Initialize();
    // buildGeometry(divisions, scale);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(oneShot()));
    timer->start(16);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(oneShot2()));
    timer->start(0);
}

OGLView::~OGLView()
{
}

void OGLView::setColor(QColor c)
{
    UNUSED(c);
}

void OGLView::draw() const
{
    drawRect();
}
