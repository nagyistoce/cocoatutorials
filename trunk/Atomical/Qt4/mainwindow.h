//
//  mainwindow.cpp
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QLabel>
#include <QSlider>
#include "Qt4.h"

class GLWidget ;
class calcThread;

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int maxNp,QWidget *parent = 0);
   ~MainWindow();

private:
    Ui::MainWindowClass* ui;
    QAction* actionAbout;

public slots:
    void on_actionAbout();
    void on_actionNewProblem();
    void on_actionExit();
    void pauseResume();
    void zoomChanged(int z);
    void xRotChanged(int z);
    void yRotChanged(int z);
    void npSliderChanged(int n);
    void fogChanged (int n);
private:
    void changed(QLabel* label,QSlider* slider,int v);

// -------------------

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void updatePositions(double *xxx,double *yyy,double *zzz/*,double *E*/);
    void shuffle();
    void performShutdown();
    void ackIsConverged();
    void turnOffFullScreen();

private:
    GLWidget*   openGLWidget;

    QTimer*     timer;

    calcThread* cThread;

    int      Np,Np2,mode;
    double   separation,imbalance,radsp,precision;

    int      nMaxNp;
    double_p xx,yy,zz;
    double_p xx_old,yy_old,zz_old;
    double_p E;

    void init_double_layer();
    void Initialize();
    void initProblem(double imb,double sep,double prec,int NNp,int NNp2,int mmode,int flag);
    void initRandomProblem(bool bMode = false,int aMode=3);

    void closeEvent(QCloseEvent* event);

    void toggleFullScreen();
    void toggleWindowed();
};

#endif // MAINWINDOW_H
