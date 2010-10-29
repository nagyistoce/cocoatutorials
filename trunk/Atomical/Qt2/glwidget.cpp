/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/


//
//  glwidget.cpp
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

#include <QtGui>
#include <QtOpenGL>
#include <math.h>
#include "glwidget.h"
#include "calcthread.h"
#include "../Fabio/OGLView.h"
#include "../Fabio/globals.h"
#include "../Fabio/ranmar.h"
#include "../Fabio/jacobi.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define random rand
#define srandom srand

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

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

void CPU_update(const double* oldx, const double* oldy,const double* oldz, double* x, double *y,double *z, int N){
        int i,j;
        double f_x=0;
        double f_y,f_z,x0,y0,z0,x1,y1,z1,ud,ud3,pref;

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

void GLWidget::oneShot()
{
    CPU_update(xx_old, yy_old,zz_old, xx, yy, zz, Np);
    CPU_poscpy(xx_old, yy_old,zz_old, xx, yy, zz, Np);
    CPU_energy(xx, yy, zz, E, Np);
//    printf("%e %e %e\n",xx[0],yy[0],zz[0]);
}

void GLWidget::oneShot2(){
    paintGL();
    updateGL();
}

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    view = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;
    zoom = 0;

    qtGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    qtPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}

GLWidget::~GLWidget()
{
//    printf("Goodnight GLWidget\n");
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(750, 750);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        //updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        //updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        //updateGL();
    }
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
    if ( event->modifiers() & Qt::ShiftModifier ) {
        float z = zRot - event->delta();
        setZRotation(z);
    } else {
        int z = zoom + (event->delta() < 0 ? 25 : -25) ;
        //zoom=zoom + (event->delta() < 0 ? 100 : -100) ;
        //if(zoom<=5) zoom=5;
        setZoom(z);
    }

//    updateGL();
}

void GLWidget::setZoom(int z)
{
    if ( z < 5 ) z = 5 ;
    if (z != zoom) {
        emit zoomChanged(z);
        zoom=z;
//        printf("zoom=%d\n",zoom);
       // updateGL();
    }
}

void GLWidget::updateThis()
{
    updateGL();
}

void GLWidget::fog_on(GLfloat density)
{
        GLfloat fogColor[] = {0.0, 0.0, 0.0, 1.0};

        glFogi (GL_FOG_MODE, GL_EXP2); //set the fog mode to GL_EXP2

        glFogfv (GL_FOG_COLOR, fogColor); //set the fog color to our color chosen above
        glFogf (GL_FOG_DENSITY, density); //set the density to thevalue above
        glHint (GL_FOG_HINT, GL_NICEST); // set the fog to look the nicest, may slow down on older cards
        glFogf(GL_FOG_START, 1.0f);
//	glFogf(GL_FOG_END, 5.0f);
        glEnable (GL_FOG); //enable the fog
}

void GLWidget::initializeGL()
{
//    qglClearColor(qtBlack());
//  static GLfloat clearcolor[4] = { 0., 0., 0., 1. };

    view = new OGLView(this) ;

//  GLfloat redDiffuseMaterial[] = {1.0, 0.0, 0.0}; //set the material to red
    GLfloat whiteSpecularMaterial[] = {1.0, 1.0, 1.0}; //set the material to white
//  GLfloat greenEmissiveMaterial[] = {0.0, 1.0, 0.0}; //set the material to green
    GLfloat mShininess[] = {16}; //set the shininess of the material

    GLfloat whiteSpecularLight[] = {1.0, 1.0, 1.0}; //set the light specular to white
    GLfloat blackAmbientLight[] = {0.0, 0.0, 0.0}; //set the light ambient to black
    GLfloat whiteDiffuseLight[] = {1.0, 1.0, 1.0}; //set the diffuse light to white

    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LEQUAL);
    glClearDepth(1.0f);

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

    static GLfloat lightPosition[4] = {-5, 0, 0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glEnable(GL_MULTISAMPLE);

     glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

     glEnable (GL_BLEND);
     glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     gluPerspective( 60, 1.0, 1.0, 1000.0);

     fog_on(0.1);

//     printf("At start: zoom=%d\n",zoom);

     MaxNp=5001;
     mem_alloc();

     mode=3;
     Np=196;
     Np2=0;

     separation=0.0;
     imbalance=1.0;

     PREC=1e-11;
     radsp=0.0;

     srand(time(NULL));
     rmarin(rand()%31329,rand()%30081);

     Initialize();

     cThread= new calcThread();
// Connect the emitted signal to the thing here
     cThread->setup(xx_old,yy_old,zz_old,imbalance,Np,Np2,mode);
     connect(cThread, SIGNAL(stepDone(double *,double *,double *,double *)), this, SLOT(updatePositions(double *,double *,double *,double *)));
     cThread->doCalc();
 /*
     timer = new QTimer(this);
     connect(timer, SIGNAL(timeout()), this, SLOT(oneShot()));
     timer->start(16);
*/

     timer = new QTimer(this);
     connect(timer, SIGNAL(timeout()), this, SLOT(oneShot2()));
     timer->start(20);

/*
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateThis()));
    timer->start(16);
    */
//    delete(cThread);
}

void GLWidget::paintGL()
{
    GLfloat camYaw,camPitch,camRadius;
    GLfloat camX,camY,camZ;

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    camRadius=zoom;
    camRadius/=100.0;
    camYaw=-6.28*yRot/(360*16);
    camPitch=6.28*xRot/(360*16);

    camX = cos(camYaw) * cos(camPitch) * camRadius;
    camY = sin(camYaw) * cos(camPitch) * camRadius;
    camZ = sin(camPitch) * camRadius;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt( camX, camY, camZ,  0.0, 0.0, 0.0,  0.0, 0.0, 1.0);

//       printf("PaintGL() %e %e %e\n",xx[0],yy[0],zz[0]);
    view->draw(xx, yy, zz, separation, imbalance, radsp, Np, Np2, mode);
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMax(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 60, 1.0, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 4 * dy);
        setYRotation(yRot + 4 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot - 4 * dy);
        setZRotation(zRot - 4 * dx);
    }
    lastPos = event->pos();
}

void GLWidget::updatePositions(double *xxx,double *yyy,double *zzz,double *EE)
{
    memcpy(xx,xxx,Np*sizeof(double));
    memcpy(yy,yyy,Np*sizeof(double));
    memcpy(zz,zzz,Np*sizeof(double));
    /*
    for(int i=0;i<Np;i++){
        xx[i]=xxx[i];
        yy[i]=yyy[i];
        zz[i]=zzz[i];
    }
*/
//    printf("[OUT] %e %e %e\n",xx[0],yy[0],zz[0]);

    UNUSED(EE); // Then, we set up a convergence but maybe in the calc stuff.
}


// That's all Folks!
////
