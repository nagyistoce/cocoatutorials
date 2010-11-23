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
    QAction* actionFullScreen;

public slots:
    void showAbout();
    void showHelp();
    void showPreferences();
    void exit();
    void fullScreen();
    void newProblem();
    void pauseResume();
    void zoomChanged(int z);
    void xRotChanged(int z);
    void yRotChanged(int z);
    void npChanged(int n);
    void fogChanged (int n);
    void updatePositions(double *xxx,double *yyy,double *zzz/*,double *E*/);
    void performShutdown();
    void ackIsConverged();
    void setBackground(int c);
    void red();
    void green();
    void blue();
    void cyan();
    void magenta();
    void yellow();
    void white();
    void black();

public:
    GLWidget*   openGLWidget;
    calcThread* cThread;

private:
    QTimer*  timer;
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

    void changed(QLabel* label,QSlider* slider,int v);
    void keyPressEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent* event);
};

#endif // MAINWINDOW_H
