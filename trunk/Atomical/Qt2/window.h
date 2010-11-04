//
//  window.h
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
#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include "calcthread.h"
#include "rendthread.h"
#include "glwidget.h"

class GLWidget;
class rendthread;

QT_BEGIN_NAMESPACE
class QSlider;
QT_END_NAMESPACE

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void updatePositions(double *xxx,double *yyy,double *zzz/*,double *E*/);
    void shuffle();
    void performShutdown();

private:
    GLWidget*   wglWidget;

    QSlider*    createSlider(int min=0,int max=360*16);

    QSlider*    xSlider;
    QSlider*    ySlider;
    QSlider*    zSlider;
    QSlider*    zoomSlider;

    QTimer*     timer;

    calcThread* cThread;

    int Np,Np2,mode;
    double separation,imbalance,radsp,precision;

    double xx[5000],yy[5000],zz[5000];
    double xx_old[5000],yy_old[5000],zz_old[5000];
    double E[5000];

    void init_double_layer();
    void Initialize();
    void initProblem(double imb,double sep,double prec,int NNp,int NNp2,int mmode,int flag);
    void initRandomProblem();

    void closeEvent(QCloseEvent *event);

};

#endif

// That's all Folks!
////
