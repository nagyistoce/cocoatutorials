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

#include "Qt4.h"
#include "GLThread.h"

GLThread::GLThread(GLWidget *gl,int maxNp)
: QThread()
, openGLWidget(gl)
, nMaxNp(maxNp)
{
    doRendering = true;
    doResize    = false;
    doFog       = false;
    background.bSet = false;
    Np          = 0;
    Np2         = 0;

    xx      = new double[maxNp];
    yy      = new double[maxNp];
    zz      = new double[maxNp];
    rrad    = new double[maxNp];

}

GLThread::~GLThread()
{
    delete [] xx ;
    delete [] yy ;
    delete [] zz ;
    delete [] rrad ;
}

double GLThread::CPU_rad(const double* xx, const double* yy,const double* zz, int N)
{
	int i;
	double x0,y0,z0;

//  Printf("CPU_rad(): N=%d\n",N);

	double res=0.0;

	for(i=0;i<N;i++){
		x0=xx[i];y0=yy[i];z0=zz[i];
		rrad[i]=sqrt(x0*x0+y0*y0+z0*z0);
		if(rrad[i]>res) res=rrad[i];
	}

	return res;
}

void GLThread::stop()
{
	doRendering = false;
}

void GLThread::resizeViewport(const QSize &size)
{
    width    = size.width();
    height   = size.height();
	doResize = true;
}

void GLThread::fog(float density /* = 0.0 */)
{
    doFog = true ;
    fogDensity = density ;
}

void GLThread::run()
{
	/********** The initialization goes here *************/
    Printf("In GLThread::run() - Initialization\n");

    openGLWidget->makeCurrent();
    GLfloat oldCamRadius=1e15;
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

    float zoomMultiplier = 100.0;

	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_POLYGON_SMOOTH);
    glClearAccum(0.0f, 0.0f, 0.0f, 0.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

#ifndef __WINDOWS__
    glEnable(GL_MULTISAMPLE);
#endif

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double PI2 = 8.0 * atan2(1.0,1.0);

    Printf("In GLThread::run() - Initialization done, start loop Np=%d\n",Np);
    int count = 10 ;
    int sleep = 10 ;
    while (count > 0 && Np == 0) {
        mSleep(sleep);
        sleep += 10 ;
        count--;
    }
    renderingHasStopped=false;
    //
    // resize and fog have to be implemented on the OpenGLThread
    while (doRendering) {
        if ( background.bSet ) {
            glClearColor(background.r,background.g,background.b,background.a);
            background.bSet = false ;
        }
        if(!bPaused) {
			if (doResize) {
				doResize = false;
                int side = qMax(width, height);
                glViewport((width - side) / 2, (height - side) / 2, side, side);

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective( 60, 1.0, 1.0, 1000.0);
                glMatrixMode(GL_MODELVIEW);
			}

            if ( doFog ) {
                doFog = false;
                glFogi (GL_FOG_MODE, GL_EXP2); //set the fog mode to GL_EXP2
                float  fb  = 1.0 ; // fogBlackener 1.0 = None < 1.0 = darker
                QColor fog = this->openGLWidget->getBackgroundColor();
                GLfloat fogColor[4] = {fog.redF()*fb, fog.greenF()*fb, fog.blueF()*fb, 1.0};

                if ( fogDensity > 0.0 ) {
                    glFogfv (GL_FOG_COLOR, fogColor); //set the fog color to our color chosen above
                    glFogf (GL_FOG_DENSITY, fogDensity); //set the density to thevalue above
                    glHint (GL_FOG_HINT, GL_NICEST); // set the fog to look the nicest, may slow down on older cards
                    glFogf(GL_FOG_START, 1.5f);
                    glEnable (GL_FOG); //enable the fog
                } else {
                    glDisable (GL_FOG);
                }
            }

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();
			gluPerspective( 60, 1, 1.0, 1000.0);

			rradius=CPU_rad(xx, yy, zz, Np);

			if ( bAutoZoom ) {
				// when we set a new problem, we set bAutoZoom
				// to request the UI to display the problem space
				camRadius=2.3*rradius; // So the zoom is tighter
	
				// When a new problem is started, the molecule radius changes wildly,
				// we then "auto-zoom" until the radius does not change up to 1%. The small
				// 10^-6 is added to denominator since rradius=0 when the rendering is
				// started but the class has not started gathering data yet.
				if(fabs(1-camRadius/(oldCamRadius+1e-6))>1e-2) {
					bAutoZoom = true ;
					oldCamRadius=camRadius;
				} else {
					bAutoZoom = false ; // ...otherwise let the user have control :)
					oldCamRadius=1e15;
				}
				int z = (int) (double) (camRadius*zoomMultiplier);
                openGLWidget->setZoom(z);
				zoom = z ;
			} else {
				camRadius=zoom; // Original code
				camRadius/=zoomMultiplier;
			}
            double y = yRot ;
            double x = xRot ;
            camYaw   =-PI2*y/360.0;
            camPitch = PI2*x/360.0;
	
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
					if(xx[i]>0.0)
						glColor4f(.5+.5*e,0,0,alpha);
					else
						glColor4f(0,0,.5+.5*e,alpha);
				} else {
					if(i%2 && imbalance>1.0)
						glColor4f(e,0.2+0.8*(imbalance-1)/9,(1-e),alpha);
					else
						glColor4f(e,0,(1-e),alpha);
				}

				glTranslatef(xx[i], yy[i], zz[i]);
				gluSphere(glq, (i%2) ? 0.2+0.2*(imbalance-1)/4 : 0.2 , 40, 40);
				if(rrad[i]<=radsp) NNpin++;
				glPopMatrix();
			}
	
			glPushMatrix();
			glColor4f((GLfloat)1,(GLfloat)1,(GLfloat)0,(GLfloat)0.7);
			glTranslatef(0, 0, 0);
			glq=gluNewQuadric();
			gluSphere(glq, radsp, 40, 40);
			glPopMatrix();

            openGLWidget->swapBuffers();
			gluDeleteQuadric(glq);
		} // if(!bPaused);
	
		emit frameNeeded();
		mSleep(13);
    }
    renderingHasStopped=true;
}

void GLThread::loadData
( double* xxx,double* yyy,double* zzz
, double  zzoom
, double  xxRot,double yyRot,double rradsp
, double  ssep,double iimb
, int NNp,int NNp2,int mmode
) {
//  Printf("GLThread::loadData\n");
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

bool GLThread::setPaused(bool aPaused)
{
    bool result = bPaused;
    bPaused = aPaused;
    // openGLWidget->openGThread->isPaused = aPaused;
    return result;
}



// That's all Folks!
////
