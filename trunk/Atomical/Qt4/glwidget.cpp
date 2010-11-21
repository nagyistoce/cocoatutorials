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

GLWidget::GLWidget(int aMaxNp,QWidget *parent)
: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
, nMaxNp(aMaxNp)
{
    mainWindow=NULL;
    xRot = 0;
    yRot = 0;
    zRot = 0;
    zoom = 7000;

    rrad = new double [nMaxNp];
    xx   = new double [nMaxNp];
    yy   = new double [nMaxNp];
    zz   = new double [nMaxNp];

    openGLThread = new GLThread(this,MaxNp);
    connect(openGLThread, SIGNAL(frameNeeded()), this, SLOT(updateFrame()));
    setAutoBufferSwap(false);    
}

GLWidget::~GLWidget()
{
    delete [] rrad;
    delete [] xx  ;
    delete [] yy  ;
    delete [] zz  ;
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
        angle += 360 ;
    while (angle > 360 )
        angle -= 360 ;
}

void GLWidget::setXRot(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotChanged(angle);
    }
}

void GLWidget::setYRot(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotChanged(angle);
    }
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
    int z = zoom + (event->delta() < 0 ? 50 : -50) ; // Zoom a bit slower...
    setZoom(z);
}

void GLWidget::keyPressEvent(QKeyEvent* event)
{
    int  c =  event->key();
    if ( c == Qt::Key_Escape )
         c = isFullScreen() ? 'f' : 'q' ;

    if ( ::tolower(c) == 'q')
        mainWindow->exit();
    else if (::tolower(c) == 'f')
        mainWindow->fullScreen();
    else
        QWidget::keyPressEvent(event);
}

void GLWidget::setZoom(int z)
{
    if ( z < 5 ) z = 5 ;
    if (z != zoom) {
        zoom=z;
        emit zoomChanged(z);
    }
}

void GLWidget::forceZoom(int z){
    zoom=z;
    emit zoomChanged(z);
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
     openGLThread->start();
}

void GLWidget::stopRendering()
{
    Printf("stopRendering()...");
    openGLThread->stop();
    do{
        mSleep(10);
    } while ( !openGLThread->renderingHasStopped );
//  openGLThread->wait();
    Printf("done!");
}

void GLWidget::resizeEvent(QResizeEvent* event)
{
    openGLThread->resizeViewport(event->size()); // defer to the openGL Thread
//  Printf("sizeEvent %dx%d\n",event->size().width(),event->size().height());
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

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->y() - lastPos.y();
    int dy = event->x() - lastPos.x();

    if (event->buttons() & Qt::LeftButton) {
        setXRot(xRot + dx);
        setYRot(yRot + dy);
    } else if (event->buttons() & Qt::RightButton) {
        setXRot(xRot - dx);
        setYRot(yRot - dy);
    }
    lastPos = event->pos();
}

void GLWidget::updateFrame()
{
//    Printf("In updateFrame()\n");
//    Printf("LoadData: Np=%d\n",Np);
    openGLThread->loadData(xx,yy,zz,(double)zoom,xRot,yRot,radsp,separation,imbalance,Np,Np2,mode);
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

void GLWidget::getCam(int *xR,int *yR,int *z){
    *xR=xRot;
    *yR=yRot;
    *z=openGLThread->zoom;
}



// That's all Folks!
////
