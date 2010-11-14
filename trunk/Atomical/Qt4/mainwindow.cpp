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

#include "Qt4.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include <time.h>

#define RANDOM_SEED() srandom(time(NULL))
#define RANDOM_INT(__MIN__, __MAX__) ((__MIN__) + random() % ((__MAX__+1) - (__MIN__)))

#define DEMO_MODE  1
#define DEMO_DELAY_IN_SECONDS 10

static void rranmar(float tmp[],int N)
{
    int i;
    for(i=0;i<N;i++){
        tmp[i]=(double)random()/(double)RAND_MAX;
    }
}

void MainWindow::init_double_layer()
{
    int i;
    Printf("MainWindow::init_double_layer() started\n");
    if(Np2>0){
        if(Np2>Np) return;
        for(i=0;i<Np2;i++){
            // Printf("MainWindow::init_double_layer() %d in layer 1\n",i);
            xx_old[i]=-separation;
            xx[i]=-separation;
        }
        for(i=Np2;i<Np;i++){
            // Printf("MainWindow::init_double_layer() %d in layer 2\n",i);
            xx_old[i]=+separation;
            xx[i]=separation;
        }
    }
}

void MainWindow::Initialize(void)
{
    int i;
    float tmp[4];
    double rr,th,ph;
    double resc=0;

    if(Np>MaxNp) return;

    for(i=0;i<MaxNp;i++){
        xx[i]=0.0;
        yy[i]=0.0;
        zz[i]=0.0;
        xx_old[i]=0.0;
        yy_old[i]=0.0;
        zz_old[i]=0.0;
        E[i]=0.0;
    }

//  srandom(time(NULL));
    if(mode==2) resc=sqrt((double)Np);
    if(mode==3) resc=pow((double)Np,1./3.);

    for(i=0;i<Np;i++){
        rranmar(tmp, 4);
        rr=tmp[1]*resc;
        th=tmp[2]*2*M_PI;
        ph=tmp[3]*M_PI;
        if(mode==2){
            xx_old[i]=0.0;
            yy_old[i]=rr*cos(th);
            zz_old[i]=rr*sin(th);
        } else {
            xx_old[i]=rr*cos(ph);
            yy_old[i]=rr*cos(th)*sin(ph);
            zz_old[i]=rr*sin(th)*sin(ph);
        }
    }

    if(mode==2) init_double_layer();
}

void MainWindow::initProblem(double imb,double sep,double prec,int NNp,int NNp2,int mmode,int flag)
{
    radsp=0.0;
    separation=sep;
    imbalance=imb;
    Np=NNp;
    Np2=NNp2;
    mode=mmode;
    precision=prec;

    wglWidget->glt->setPaused(true);
    mSleep(16);
    wglWidget->glt->setAutoZoom(true) ;

    Printf("%e %e %d %d %d\n",imbalance,separation,Np,Np2,mode);

    if(flag) {
        Initialize();
        cThread->resume(); // Resume calculation if that was paused
        //if(mode==3){
        //    wglWidget->glt->fog_on(0.07);
        //} else {
        //    wglWidget->glt->fog_off();
        //}
    }

    cThread->setup(xx_old,yy_old,zz_old,imbalance,precision,Np,Np2,mode);
    wglWidget->glt->setPaused(false);
}

void MainWindow::initRandomProblem(bool bMode /*=false*/,int aMode /*= 3 */)
{
    float tmp[3];
    int toggle;

    int mmode,NNp,NNp2;
    double iimb,ssep;

    iimb=1.0; // Random?!?
    mmode=bMode ? aMode : 2+RANDOM_INT(0,1);

    toggle=RANDOM_INT(0,1);
    if(toggle){
        NNp=RANDOM_INT(51,512);
    } else {
        NNp=RANDOM_INT(3,50);
    }

    rranmar(tmp,2);
    if(mmode==3){
        NNp2=0;
        ssep=0.0;
    } else {
        toggle=RANDOM_INT(0,1);
        if(toggle){
                NNp2=RANDOM_INT(1,NNp/2);
                ssep=2*tmp[0];
        } else {
                NNp2=0;
                ssep=0.0;
        }
    }

    toggle=RANDOM_INT(0,1);
    if(toggle) iimb=1.1+0.3*tmp[1];

    initProblem(iimb,ssep,1e-9,NNp,NNp2,mmode,1);
}


QSlider *MainWindow::createSlider(int min/* =0*/,int max/* =360*16*/)
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(min,max);
    slider->setSingleStep((max-min)/360);
    slider->setPageStep((max-min)/12);
    slider->setTickInterval((max-min)/12);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    char c = e->key();
    if (e->key() == Qt::Key_Escape || ::tolower(c) == 'q')
        on_actionExit();
    else
        QWidget::keyPressEvent(e);
}

void MainWindow::updatePositions(double *xxx,double *yyy,double *zzz/*,double *EE*/)
{
    memcpy(xx,xxx,Np*sizeof(double));
    memcpy(yy,yyy,Np*sizeof(double));
    memcpy(zz,zzz,Np*sizeof(double));

    wglWidget->receiveData(xx,yy,zz,radsp,separation,imbalance,Np,Np2,mode);
}

void MainWindow::shuffle()
{
   newProblem();
}

void MainWindow::performShutdown()
{
    Printf("About to quit!\n");
    wglWidget->stopRendering();
}

void MainWindow::closeEvent(QCloseEvent* /*event*/)
{
    Printf("About to quit!\n");
    wglWidget->stopRendering();
}

MainWindow::MainWindow(int maxNp,QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);

    nMaxNp = maxNp;
    xx     = new double[maxNp];
    yy     = new double[maxNp];
    zz     = new double[maxNp];
    xx_old = new double[maxNp];
    yy_old = new double[maxNp];
    zz_old = new double[maxNp];
    E      = new double[maxNp];

//  Init the RNGs
    srandom(time(NULL));
    RANDOM_SEED();

//  Init the OpenGL pixel format (other initialization in GLThread)
    QGLFormat pixelFormat;
    pixelFormat.setDoubleBuffer(TRUE);
    pixelFormat.setAlpha(TRUE);
    pixelFormat.setRgba(TRUE);
    pixelFormat.setDirectRendering(TRUE);
    pixelFormat.setAccum(TRUE);
    pixelFormat.setDepth(TRUE);
    QGLFormat::setDefaultFormat(pixelFormat);

    wglWidget = new GLWidget;
    ui->verticalLayout_2->addWidget( wglWidget );

    // manually add to the menu (this is sample code to be removed later)
    actionAbout = new QAction(tr("A&bout"), this);
    ui->menuBar->addAction( actionAbout );
    connect( actionAbout , SIGNAL( triggered() ), this, SLOT( slotAbout() ) );

    //  Alloc a new calculation thread
    cThread= new calcThread(MaxNp);

    //  When a step is calculated, the stepDone() signal is emitted.
    //  It is caught here by updatePositions() so that this class is aware of these positions.
    //  Additionally, updatePositions() loads the particles positions into the glWidget
    connect(cThread, SIGNAL(stepDone(double* ,double* ,double* /*,double* */)), this, SLOT(updatePositions(double* ,double* ,double* /*,double * */)));
    newProblem();

    //  This call initializes a problem
    //
    //  initProblem(IMBALANCE,SEPARATION,TARGET_PRECISION,Np,Np2,MODE,FLAG)
    //
    //  IMBALANCE: a double parameter, must be >= 1.0
    //  SEPARATION: a double parameter, must be >= 0.0
    //  TARGET_PRECISION: a double parameter in the range 1e-16 <= x <= 1
    //  Np: an integer parameter, in the range 2 <= x <= MaxNp
    //  Np2: an integer parameter, in the range 0 <= x <= Np
    //  MODE: an integer parameter, with value 2 or 3   -   2d or 3d of course)
    //  FLAG: an integer parameter, with value 0 or 1   -   ?? fog on/off for 3d ??
    //
    //  Whenever a new molecule needs to be calculated, FLAG should be set to 1.
    //
    //  If IMBALANCE, SEPARATION or TARGET_PRECISION are modified, we assume the user
    //  does not want to reshuffle the particles, so we call is with FLAG set to 0
    //
    //  Examples
    //
    //  initProblem(1.0,0.2,1e-9,256,128,2,1); // To init a new molecule calculation
    //
    //  initProblem(1.0,0.5,1e-9,256,128,2,0); // To update the SEPARATION parameter

    //  Then, we start the calculation and the rendering. They both live in their threads
    //  and communicate via signals and slots.
    cThread->doCalc();
    wglWidget->startRendering();

    //  At this point, the UI should take care of interacting with the user and call
    //  initProblem() as needed. To exemplify this, here is a "demo mode" (from my
    //  screensaver). Note that initProblem() can be called without stopping any thread.

//#if DEMO_MODE
//    timer = new QTimer(this);
//    connect(timer,SIGNAL(timeout()),this,SLOT(shuffle()));
//    timer->start(1000*DEMO_DELAY_IN_SECONDS); // Shuffle the parameters every so seconds
//#endif
}

MainWindow::~MainWindow()
{
    delete ui;
    delete [] xx     ;
    delete [] yy     ;
    delete [] zz     ;
    delete [] xx_old ;
    delete [] yy_old ;
    delete [] zz_old ;
    delete [] E      ;
}

void MainWindow::on_actionExit()
{
    this->window()->close();
}

void MainWindow::slotAbout()
{
    about*  dialog = new about;
    dialog->setWindowFlags(Qt::Tool);
    dialog->move( int(this->x() + 0.5 * this->width()  - 0.5 * dialog->width())
                , int(this->y() + 0.5 * this->height() - 0.5 * dialog->height())
                );

    dialog->setWindowIcon(QIcon(":/icon/ikona_32.png"));
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->show();
    dialog->activateWindow();
}

#if 0
void MainWindow::S_labelUpdate()
{
QVariant v;
/*
     v = glWidget->rotX;
    ui->label_X->setText( v.toString() );
     v = glWidget->rotY;
    ui->label_Y->setText( v.toString() );
     v = glWidget->rotZ;
    ui->label_Z->setText( v.toString() );
*/
}
#endif

void MainWindow::pause()
{
    cThread->pause() ;
}

void MainWindow::resume()
{
    cThread->resume() ;
}

void MainWindow::newProblem()
{
    int amode = ui->threeD->checkState() ? 3 : 2 ;
    initRandomProblem(true,amode);
}

void MainWindow::on_actionNewProblem_triggered()
{
    newProblem();
}

void MainWindow::on_actionAbout_triggered()
{
    slotAbout();
}

// That's all Folks!
////

