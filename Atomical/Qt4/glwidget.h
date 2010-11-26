//
//  glwidget.h
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
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QWidget>
#include <QTimer>

#include "Qt4.h"
#include "calcthread.h"
#include "mainwindow.h"
#include "GLThread.h"

class GLThread;
class MainWindow;

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(int aMaxNp,QWidget* parent = 0);
   ~GLWidget();

    // void drawGL(double *xx,double *yy,double *zz,double imb,double sep,double rrsp,int NNp,int NNp2,int mmode);
    void startRendering();
    void stopRendering();

    void receiveData(double *xxx,double *yyy,double *zzz,double rradsp,double ssep,double iimb,int NNp,int NNp2,int mmode);
    void getCam(int *xR,int *yR,int *z);
    void forceZoom(int z);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    GLThread* openGLThread;
    MainWindow* mainWindow;

    // allow GLThread and MainWindow to access our private members
    friend class GLThread;
    friend class MainWindow;

public slots:
    void setXRot(int angle);
    void setYRot(int angle);
    void setZoom(int z);
    void setBackground(int c); // c = unsigned int 'r' = Red
    void setBackground(QColor& c);
    void getBackground(QColor& c);
  QColor getBackgroundColor();


    void updateThis();
    void updateFrame();

signals:
    void xRotChanged(int angle);
    void yRotChanged(int angle);
    void zoomChanged(int z);
    void exit();
    void fullScreen();

protected:
    void initializeGL();
    void paintGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
    void resizeEvent(QResizeEvent *evt);
    void paintEvent(QPaintEvent *evt);
    void closeEvent(QCloseEvent *evt);
    void keyPressEvent(QKeyEvent* event);

    double CPU_rad(const double* xx, const double* yy,const double* zz, int N);

    int       xRot;
    int       yRot;
    int       zRot;
    int       zoom;
private:
    int       is_drawing;
    int       can_terminate;
    QPoint    lastPos;
    QTimer*   timer;

    int       nMaxNp;
    double_p  rrad,xx,yy,zz;
    double    radsp,separation,imbalance;
    int       Np,Np2;
    int       mode;
};

#endif

// That's all Folks!
////
