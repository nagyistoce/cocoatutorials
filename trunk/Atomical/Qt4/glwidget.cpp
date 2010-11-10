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
#include <QObject>
#include <QtOpenGL>
#include <QGLWidget>
#include <math.h>
#include "glwidget.h"
#include "calcthread.h"
#include "Qt4.h"


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget(QWidget *parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
//  view = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;
    zoom = 7000;

    glt = new GLThread(this);
    connect(glt, SIGNAL(frameNeeded()), this, SLOT(updateFrame()));
    updateFrame();

    setAutoBufferSwap(false);    
}

double GLWidget::CPU_rad(const double* xx, const double* yy,const double* zz, int N)
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
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
    }
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
    if ( event->modifiers() & Qt::ShiftModifier ) {
        float z = zRot - event->delta();
        setZRotation(z);
    } else {
        int z = zoom + (event->delta() < 0 ? 50 : -50) ; // Zoom a bit slower...
        setZoom(z);
    }
}

void GLWidget::setZoom(int z)
{
    if ( z < 5 ) z = 5 ;
    if (z != zoom) {
        zoom=z;
    //  printf("zoomChanged = %d\n",zoom);
        emit zoomChanged(z);
    }
}

void GLWidget::updateThis()
{
    updateGL();
}

void GLWidget::initializeGL()
{
    return;
}

void GLWidget::paintGL()
{
    return;
}

void GLWidget::startRendering()
{
	 glt->start();
}

void GLWidget::stopRendering()
{
    printf("stopRendering()...");
	glt->stop();
	glt->wait();
    printf("done!");
}

void GLWidget::resizeEvent(QResizeEvent* /* event */)
{
//	glt->resizeViewport(evt->size());
}

void GLWidget::paintEvent(QPaintEvent* /* event */)
{
	// Handled by the GLThread.
}

void GLWidget::closeEvent(QCloseEvent* event)
{
	stopRendering();
    QGLWidget::closeEvent(event);
}

void GLWidget::drawGL(double *xx,double *yy,double *zz,double imbalance,double separation,double radsp,int Np,int Np2,int mode)
{
    int i,NNpin;
    float alpha;
    double rradius;

    GLfloat camYaw,camPitch,camRadius;
    GLfloat camX,camY,camZ;

    GLUquadric *glq;

    if(!is_drawing) {
        can_terminate=1;
        return;
    }

    makeCurrent();

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

    rradius=CPU_rad(xx, yy, zz, Np);

    alpha=0.9;

    NNpin=0;

    for(i=0;i<Np;i++)
    {
		glPushMatrix();
		GLfloat e=(GLfloat)(rrad[i]/rradius);
		if(mode==2 && Np2>0 && separation>0)
		{
			if(xx[i]>0.0){
                glColor4f(.5+.5*e,0,0,alpha);
			} else {
                glColor4f(0,0,.5+.5*e,alpha);
			}
		} else {
			if(i%2 && imbalance>1.0) glColor4f(e,0.2+0.8*(imbalance-1)/9,(1-e),alpha); else glColor4f(e,0,(1-e),alpha);
		}

		glTranslatef(xx[i], yy[i], zz[i]);
		glq=gluNewQuadric();
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

    swapBuffers();  // Double buffering

    gluDeleteQuadric(glq);
}

void GLWidget::resizeGL(int,int) // width, int height)
{/*
    int side = qMax(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 60, 1.0, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
*/}

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

void GLWidget::updateFrame()
{
    //printf("In updateFrame()\n");
    glt->loadData(xx,yy,zz,(double)zoom,xRot,yRot,radsp,separation,imbalance,Np,Np2,mode);
}

void GLWidget::receiveData(double *xxx,double *yyy,double *zzz,double rradsp,double ssep,double iimb,int NNp,int NNp2,int mmode)
{
    Np=NNp;
    Np2=NNp2;
    mode=mmode;

    radsp=rradsp;
    separation=ssep;
    imbalance=iimb;

    memcpy(xx,xxx,Np*sizeof(double));
    memcpy(yy,yyy,Np*sizeof(double));
    memcpy(zz,zzz,Np*sizeof(double));
}

// That's all Folks!
////
