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
#include <QSettings>
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
    MainWindow(int maxNp,QWidget* parent = 0);
   ~MainWindow();

private:
    Ui::MainWindowClass* ui;

public slots:
    void showAbout();
    void showHelp();
    void showPreferences();
    void exit();
    void fullScreen();
    void newProblem();
    void newProblemEnable();
    void pauseResume();
    void zoomChanged(int z);
    void xRotChanged(int z);
    void yRotChanged(int z);
    void npChanged(int n);
    void np2Changed(int n);
    void fogChanged (int n);
    void radChanged(int n);
    void radChanged(double v);
    void sepChanged(int n);
    void precChanged(int n);
    void eigenmodeValueChanged();
    void normalModesChanged();
    void updatePositions(double *xxx,double *yyy,double *zzz/*,double *E*/);
    void performShutdown();
    void ackIsConverged();
    void setBackground(int c); // c = unsigned char 'r' = Red etc....
    void setBackground(QColor& c);
    void getBackground(QColor& c);
  QColor getBackgroundColor();
    void red();
    void green();
    void blue();
    void cyan();
    void magenta();
    void yellow();
    void white();
    void black();
    void other();

public:
    GLWidget*   openGLWidget;
    calcThread* cThread;

private:
const char*  sHelpURL; // url of help file (eg "http://clanmills.com/robin.shtml")

    QTimer*  timer;
    int      Np,Np2,mode;
    double   separation,imbalance,radsp,precision;

    int      nMaxNp;
    double_p xx,yy,zz;
    double_p xx_old,yy_old,zz_old;
    double_p E;
    int      eigenModeMaxNp;

    void init_double_layer();
    void Initialize();
    void initProblem(double imb,double sep,double prec,int NNp,int NNp2,int mmode,int flag);
    void initRandomProblem(bool bMode = false,int aMode=3);
    void rranmar(float tmp[],int N);

    void changed(QLabel* label,QSlider* slider,int v);
    void keyPressEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent* event);

    bool bFullScreenMenubar;
    bool bFullScreenControls;
    bool bNativeDialogs ;

    void fullScreenMenubar(bool);
    void fullScreenControls(bool);
    void nativeDialogs(bool);

    double rad();

    friend class Preferences;
};

#endif // MAINWINDOW_H
