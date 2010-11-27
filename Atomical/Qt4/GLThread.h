//
//  GLThread.h
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

#pragma once
#ifndef GLTHREAD_H
#define GLTHREAD_H

#include <QThread>
#include "glwidget.h"

struct SetColor
{
    bool    bSet ;
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
};

class GLWidget; // forward declaration

class GLThread : public QThread
{
    Q_OBJECT

public:
   GLThread(GLWidget* glWidget,int maxNp);
  ~GLThread() ;
  
   void resizeViewport(const QSize &size);
   void run();
   void stop();
   void loadData( double *xxx, double *yyy,double *zzz
   				, double zzoom, double xxRot, double yyRot
   				, double rradsp,double ssep,double iimb
   				, int NNp,int NNp2,int mmode
   				) ;

   void fog(float density=0.0);

   // getters/setters
   bool setAutoZoom(bool aAutoZoom) { bool result = bAutoZoom ; bAutoZoom = aAutoZoom ; return result ; }

   bool isPaused() { return bPaused ; }
   bool setPaused(bool aPaused);

   volatile bool renderingHasStopped;

signals:
   void frameNeeded();
public:
   SetColor     background ;
   double 		radsp, separation, imbalance;

private:
   bool         bAutoZoom;
   bool         bPaused;
   bool 		doRendering;
   bool 		doResize;
   bool         doFog;
   double       fogDensity;
   int  		width;
   int  		height;
   int  		rotAngle;
   GLWidget*	openGLWidget;

   double CPU_rad(const double* xx, const double* yy,const double* zz, int N);

   int          nMaxNp;
   double_p 	rrad,xx,yy,zz;

   double 		zoom,xRot,yRot;
   int    		Np,Np2;
   int			mode;

   // allow GLWidget access to our private members
   friend class GLWidget;
};

#endif // GLTHREAD_H

// That's all Folks!
////
