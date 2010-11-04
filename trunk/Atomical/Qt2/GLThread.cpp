//
//  GLThread.cpp
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
#include "GLThread.h"

double GLThread::CPU_rad(const double* xx, const double* yy,const double* zz, int N)
{
	int i;
	double x0,y0,z0;

	double res=0.0;

	for(i=0;i<N;i++){
		x0=xx[i];y0=yy[i];z0=zz[i];
		rrad[i]=sqrt(x0*x0+y0*y0+z0*z0);
		if(rrad[i]>res) res=rrad[i];
	}

	return res;
}

GLThread::GLThread(GLWidget *gl)
: QThread(), glw(gl)
{
	doRendering = true;
	doResize = false;
}

void GLThread::stop()
{
	doRendering = false;
}

void GLThread::resizeViewport(const QSize &size)
{
	w = size.width();
	h = size.height();
	doResize = true;
}

void GLThread::run()
{
	/********** The initialization goes here *************/

    printf("In GLThread::run() - Initialization\n");

	glw->makeCurrent();

	GLfloat whiteSpecularMaterial[] = {1.0, 1.0, 1.0}; //set the material to white

	GLfloat mShininess[] = {16}; //set the shininess of the material

	GLfloat whiteSpecularLight[] = {1.0, 1.0, 1.0}; //set the light specular to white
	GLfloat blackAmbientLight[] = {0.0, 0.0, 0.0}; //set the light ambient to black
    GLfloat whiteDiffuseLight[] = {1.0, 1.0, 1.0}; //set the diffuse light to white

	GLfloat camYaw,camPitch,camRadius;
	GLfloat camX,camY,camZ;

    GLUquadric* glq;

	int i,NNpin;
	float alpha;
	double rradius;

	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_POLYGON_SMOOTH);

	glClearAccum(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LEQUAL);
	glClearDepth(1.0f);

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 60, 1.0, 1.0, 1000.0);

#ifndef __CYGWIN__
    glEnable(GL_MULTISAMPLE);
#endif

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    printf("In GLThread::run() - Initialization done, start loop\n");

	while (doRendering) {
		/*** This is crappy and needs to be fixed ***/

		if (doResize) {
			glViewport(0, 0, w, h);
			doResize = false;
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		gluPerspective( 60, 1, 1.0, 1000.0);

		rradius=CPU_rad(xx, yy, zz, Np);

        // the camRadius is quite a small float
        // zoom is quite a large integer
        float zoomMultiplier = 100.0;
        if ( bAutoZoom ) {
            // when we set a new problem, we set bAutoZoom
            // to request the UI to display the problem space
            camRadius=5.0* rradius;
            if ( camRadius < 10 ) camRadius = 10;
            int z = (int) (double) (camRadius*zoomMultiplier);
            glw->setZoom(z);
            zoom = z ;
            bAutoZoom = false ;
        } else {
            camRadius=zoom; // Original code
            camRadius/=zoomMultiplier;
        }

		camYaw=-6.28*yRot/(360*16);
		camPitch=6.28*xRot/(360*16);

		camX = cos(camYaw) * cos(camPitch) * camRadius;
		camY = sin(camYaw) * cos(camPitch) * camRadius;
		camZ = sin(camPitch) * camRadius;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		gluLookAt( camX, camY, camZ,  0.0, 0.0, 0.0,  0.0, 0.0, 1.0);

		alpha=0.9;

		NNpin=0;
		glq=gluNewQuadric();

		for(i=0;i<Np;i++){
			glPushMatrix();
			GLfloat e=(GLfloat)(rrad[i]/rradius);
			if(mode==2 && Np2>0 && separation>0){
				if(xx[i]>0.0){
					glColor4f(.5+.5*e,0,0,alpha);
				} else {
					glColor4f(0,0,.5+.5*e,alpha);
				}
			} else {
				if(i%2 && imbalance>1.0) glColor4f(e,0.2+0.8*(imbalance-1)/9,(1-e),alpha); else glColor4f(e,0,(1-e),alpha);
			}

			glTranslatef(xx[i], yy[i], zz[i]);
			if(i%2) gluSphere(glq, 0.2+0.2*(imbalance-1)/4, 40, 40); else gluSphere(glq, 0.2, 40, 40);
			if(rrad[i]<=radsp) NNpin++;
			glPopMatrix();
		}

		glPushMatrix();
		glColor4f((GLfloat)1,(GLfloat)1,(GLfloat)0,(GLfloat)0.7);
		glTranslatef(0, 0, 0);
		glq=gluNewQuadric();
		gluSphere(glq, radsp, 40, 40);
		glPopMatrix();

		glw->swapBuffers();
		gluDeleteQuadric(glq);

		emit frameNeeded();

		mSleep(13);
	 }
}

void GLThread::loadData
( double* xxx,double* yyy,double* zzz
, double  zzoom
, double  xxRot,double yyRot,double rradsp
, double  ssep,double iimb
, int NNp,int NNp2,int mmode
) {
//  printf("GLThread::loadData\n");
	zoom=zzoom;
	xRot=xxRot;
	yRot=yyRot;
	radsp=rradsp;
	separation=ssep;
	imbalance=iimb;
	Np=NNp;
	Np2=NNp2;
	mode=mmode;

	memcpy(xx,xxx,Np*sizeof(double));
	memcpy(yy,yyy,Np*sizeof(double));
	memcpy(zz,zzz,Np*sizeof(double));
}

// That's all Folks!
////
