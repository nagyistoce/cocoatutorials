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

class GLWidget; // forward declaration

class GLThread : public QThread
{
    Q_OBJECT

public:
   GLThread(GLWidget* glWidget);
  ~GLThread() {};
  
   void resizeViewport(const QSize &size);
   void run();
   void stop();
   void loadData( double *xxx, double *yyy,double *zzz
   				, double zzoom, double xxRot, double yyRot
   				, double rradsp,double ssep,double iimb
   				, int NNp,int NNp2,int mmode
   				) ;

   void fog_on(float density);
   void fog_off(void);

   // getters/setters
   bool setAutoZoom(bool aAutoZoom) { bool result = bAutoZoom ; bAutoZoom = aAutoZoom ; return result ; }

   bool isPaused()                  { return bPaused ; }
   bool setPaused(bool aPaused)     { bool result = bPaused   ; bPaused = aPaused     ; return result ; }

   // allow GLWidget access to our private members
   friend class GLWidget;
signals:
   void frameNeeded();

private:
   bool         bAutoZoom;
   bool         bPaused;
   bool 		doRendering;
   bool 		doResize;
   int  		w;
   int  		h;
   int  		rotAngle;
   GLWidget*	glw;

   double CPU_rad(const double* xx, const double* yy,const double* zz, int N);

   double 		zoom,xRot,yRot;
   double 		rrad[5000],xx[5000],yy[5000],zz[5000];
   int    		Np,Np2;
   int			mode;
   double 		radsp, separation, imbalance;
};

#endif // GLTHREAD_H
