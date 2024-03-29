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
#include "qt4settings.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glwidget.h"
#include "about.h"
#include "preferences.h"
#include <time.h>
#include <QtGui/QFrame>

#define B(b) ((b) ? Qt::Checked:Qt::Unchecked)
#define RANDOM_SEED() srandom(time(NULL))
#define RANDOM_INT(__MIN__, __MAX__) ((__MIN__) + random() % ((__MAX__+1) - (__MIN__)))

#define DEMO_MODE  1
#define DEMO_DELAY_IN_SECONDS 10

MainWindow::MainWindow(int maxNp,QWidget* parent)
: QMainWindow(parent)
, ui(new Ui::MainWindowClass)
, Np(0)
, Np2(0)
, eigenModeMaxNp(100)
, bConverged(false)
{
    ui->setupUi(this);

    sHelpURL="http://clanmills.com/robin.shtml";
    bFullScreenControls = theSettings->value(theSettings->sFullScreenControls,theSettings->vTrue).toBool();
    bFullScreenMenubar  = theSettings->value(theSettings->sFullScreenMenubar ,theSettings->vFalse).toBool();
    bNativeDialogs      = theSettings->value(theSettings->sNativeDialogs     ,theSettings->vTrue).toBool();
    QColor bgColor      = theSettings->backgroundColor();

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

    openGLWidget = new GLWidget(MaxNp,this);
    openGLWidget->setBackground(bgColor);
    ui->mainLayout->insertWidget(0, openGLWidget,Qt::AlignLeft );

    //  Alloc a new calculation thread
    cThread= new calcThread(MaxNp,eigenModeMaxNp);

    connect(openGLWidget,SIGNAL(zoomChanged(int)),this,SLOT(zoomChanged(int)));
    connect(openGLWidget,SIGNAL(xRotChanged(int)),this,SLOT(xRotChanged(int)));
    connect(openGLWidget,SIGNAL(yRotChanged(int)),this,SLOT(yRotChanged(int)));

    //  When a step is calculated, the stepDone() signal is emitted.
    //  It is caught here by updatePositions() so that this class is aware of these positions.
    //  Additionally, updatePositions() loads the particles positions into the glWidget
    connect(cThread, SIGNAL(stepDone(double* ,double* ,double* ,double* )), this, SLOT(updatePositions(double* ,double* ,double* ,double* )));
    connect(cThread, SIGNAL(isConverged()), this, SLOT(ackIsConverged()));
    connect(cThread, SIGNAL(calculateEigenmodesDone()), this, SLOT(calculateEigenmodesDone()));
    connect(cThread, SIGNAL(calcProgress(QString)),this,SLOT(calcProgress(QString)));

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
    openGLWidget->startRendering();
    npChanged(13);
    newProblem();

    //  At this point, the UI should take care of interacting with the user and call
    //  initProblem() as needed. To exemplify this, here is a "demo mode" (from my
    //  screensaver). Note that initProblem() can be called without stopping any thread.

//#if DEMO_MODE
//    timer = new QTimer(this);
//    connect(timer,SIGNAL(timeout()),this,SLOT(shuffle()));
//    timer->start(1000*DEMO_DELAY_IN_SECONDS); // Shuffle the parameters every so seconds
//#endif
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

void MainWindow::exit()
{
    if ( isFullScreen() )
        fullScreen();
    else
        close() ;
}

void MainWindow::rranmar(float tmp[],int N)
{
    int i;
    for(i=0;i<N;i++){
        tmp[i]=(double)random()/(double)RAND_MAX;
    }
}



#if 0
// leave this for now.
// I like the font being used here and might decide to use this everywhere in the UI
void MainWindow::S_labelUpdate()
{
QVariant v;
     v = glWidget->rotX;
    ui->label_X->setText( v.toString() );
     v = glWidget->rotY;
    ui->label_Y->setText( v.toString() );
     v = glWidget->rotZ;
    ui->label_Z->setText( v.toString() );
}
#endif

void MainWindow::pauseResume()
{
    bool paused = openGLWidget->openGLThread->isPaused();
    ui->pauseResume->setText(QString(paused ? "Pause" : "Resume") );
    // openGLWidget->openGLThread->setPaused(!paused);
    if ( paused ) cThread->resume(); else cThread->pause();
    Printf("MainWindow::pauseResume\n");
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

void MainWindow::initProblem(double imb,double sep,double prec,int NNp,int NNp2,int mmode,int flag)
{
    radsp = this->rad();
    separation=sep;
    imbalance=imb;
    bConverged=false;
    Np=NNp;
    Np2=NNp2;
    mode=mmode;
    precision=prec;

    openGLWidget->openGLThread->setPaused(true);
    mSleep(16);
    openGLWidget->openGLThread->setAutoZoom(true) ;

    Printf("%e %e %d %d %d\n",imbalance,separation,Np,Np2,mode);

    if(flag) {
        Initialize();
        cThread->resume(); // Resume calculation if that was paused
        cThread->oldE=1e99; // Needed to ensure restart
    }

    cThread->setup(xx_old,yy_old,zz_old,imbalance,precision,Np,Np2,mode);
    openGLWidget->openGLThread->setPaused(false);
    Printf("MainWindow::initProblem maxEigenmodes = %d\n",cThread->maxEigenmode());
}

void MainWindow::initRandomProblem(bool bMode /*=false*/,int aMode /*= 3 */)
{
    float tmp[3];
    int toggle;

    int mmode,NNp,NNp2;
    double iimb,ssep;

    iimb=1.0;
    mmode=bMode ? aMode : 2+RANDOM_INT(0,1);

    NNp=ui->npValue->text().toInt();
    rranmar(tmp,2);
    if(mmode==3){
        NNp2=0;
        ssep=0.0;
    } else {
        //toggle=RANDOM_INT(0,1);
        //if(toggle){
        //        NNp2=RANDOM_INT(1,NNp/2);
        //        ssep=2*tmp[0];
        //} else {
        //        NNp2=0;
        //        ssep=0.0;
        //}
        // Robin's version to respect NNp2
        NNp2=ui->np2SpinBox->value();
        if ( NNp2 > NNp ) NNp2=NNp-1;
        if ( NNp2 < 0   ) NNp2=0;
        ssep = NNp2 ? 2*tmp[0] : 0.0;
    }

    toggle=RANDOM_INT(0,1);
    if(toggle) iimb=1.1+0.3*tmp[1];

    double prec = 1e-2;

    initProblem(iimb,ssep,prec,NNp,NNp2,mmode,1);
}

void MainWindow::updatePositions(double *xxx,double *yyy,double *zzz,double* EE)
{
    memcpy(xx,xxx,Np*sizeof(double));
    memcpy(yy,yyy,Np*sizeof(double));
    memcpy(zz,zzz,Np*sizeof(double));

    openGLWidget->receiveData(xx,yy,zz,radsp,separation,imbalance,Np,Np2,mode);
    char    sE[100];
    sprintf(sE,"%f",*EE);
    ui->feedbackE->setText(QString(sE));
    // I should format with QString, however I haven't studied that yet.
    // QString sE;
    // sE.setNum(EE);
}

void MainWindow::ackIsConverged()
{
    Printf("MainWindow::ackIsConverged\n");
    bConverged = true;
    ui->eigenmodeValue->setEnabled(true);
    ui->normalModes->setEnabled(true);
    if ( ui->normalModes->checkState()) {
        int mode = ui->eigenmodeValue->value();
        cThread->startEigenmodes(mode); // Once this is toggled, the problem type should not be changed
                                        // Otherwise the whole thing goes crazy :)
                                        // Problem may be changed only after this returns.
    }
}

void MainWindow::performShutdown()
{
    Printf("About to quit!\n");
    openGLWidget->stopRendering();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    openGLWidget->keyPressEvent(event);
}

void MainWindow::closeEvent(QCloseEvent* /*event*/)
{
    performShutdown();
}

void MainWindow::newProblem()
{
    Printf("MainWindow::newProblem\n");
    if ( openGLWidget->openGLThread->isPaused() )
        pauseResume();
    cThread->stopEigenmodes();
    int amode = ui->threeD->checkState() ? 3 : 2 ;
    initRandomProblem(true,amode);
    ui->newProblem->setEnabled(false);
    ui->eigenmodeValue->setEnabled(false);
    openGLWidget->setFocus(); // bring focus to widget (to capture key strokes)

}

void MainWindow::surprise()
{
    newProblemEnable(false);
    int maxPoints=eigenModeMaxNp + eigenModeMaxNp/2;
    npChanged(RANDOM_INT(1,maxPoints));
    np2Changed(RANDOM_INT(1,maxPoints-1));
    eigenmodeChanged(RANDOM_INT(0,30),false);
    ui->threeD->setCheckState(B(RANDOM_INT(0,2)));
    ui->normalModes->setCheckState(B(RANDOM_INT(0,2)));
    newProblem();
}

void MainWindow::newProblemEnable(bool bFlashAndBeep /* = false */)
{
    if ( !ui->newProblem->isEnabled() ) {
        ui->newProblem->setEnabled(true);
        if ( bFlashAndBeep ) {
            QApplication::beep();
            QColor c = getBackgroundColor();
            setBackground('w');
            mSleep(200);
            setBackground('b');
            mSleep(200);
            setBackground(c);
        }
    }
    ui->np2SpinBox->setEnabled(!ui->threeD->isChecked());
}

void MainWindow::showAbout()
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

void MainWindow::showHelp()
{
    Printf("MainWindow::showHelp - not implemented yet!\n");
    ::LinkActivated(sHelpURL);
}

void MainWindow::showPreferences()
{
    Preferences*  preferences = new Preferences(this);
    preferences->setWindowFlags(Qt::Tool);
    preferences->move( int(this->x() + 0.5 * this->width()  - 0.5 * preferences->width())
                , int(this->y() + 0.5 * this->height() - 0.5 * preferences->height())
                );

    preferences->setWindowIcon(QIcon(":/icon/ikona_32.png"));
    preferences->setWindowModality(Qt::ApplicationModal);
    preferences->show();
    preferences->activateWindow();
}

void MainWindow::changed(QLabel* label,QSlider* slider,int v)
{
    QString S;
    S.setNum(v);
    label->setText(S);
    slider->setValue(v);
}

void MainWindow::zoomChanged(int v)
{
    changed(ui->zoomValue,ui->zoomSlider,v);
    openGLWidget->setZoom(v) ;
}

void MainWindow::xRotChanged(int v)
{
    changed(ui->xRotValue,ui->xRotSlider,v);
    openGLWidget->setXRot(v);
}

void MainWindow::yRotChanged(int v)
{
    changed(ui->yRotValue,ui->yRotSlider,v);
    openGLWidget->setYRot(v);
}

void MainWindow::npChanged(int n)
{
    ui->npValue->setNum(n);
    ui->npSpinBox->setValue(n);
    ui->npSlider->setValue(n);
    ui->normalModes->setEnabled(n <= this->eigenModeMaxNp);
    newProblemEnable();

    // UI visual feedback
    char style[100];
    if ( n > eigenModeMaxNp ) {
        sprintf(style,"color:red;");
    } else {
        style[0]=0;
    }
    ui->npValue->setStyleSheet(style);
    ui->npLabel->setStyleSheet(style);
    ui->npSpinBox->setStyleSheet(style);
    ui->eigenmodeValue->setStyleSheet(style);

    // oddly, the spin box doesn't respect the color, so change his background
    if ( n > eigenModeMaxNp )
        sprintf(style,"background:pink;");
    ui->normalModes->setStyleSheet(style);
}

void MainWindow::np2Changed(int n)
{
    newProblemEnable();
    UNUSED(n);
}

void MainWindow::fogChanged(int n)
{
    double f = n ;
    f /= 100.0   ;
    ui->fogValue->setNum(f);
    openGLWidget->openGLThread->fog(f);
}

void MainWindow::radChanged(int n)
{
    double v = n ;
    v    /= 100.0;
    ui->radValue->setNum(v);
    ui->radSlider->setValue(n);
    radsp=v;
    openGLWidget->radsp = v   ;
    openGLWidget->openGLThread->radsp = v ;
}

void MainWindow::radChanged(double v)
{
    radChanged((int)(v*100.0)) ;
}

double MainWindow::rad()
{
    double v = (double) ui->radSlider->value() ;
    return v / 100.0 ;
}


void MainWindow::sepChanged(int n)
{
    ui->sepValue->setNum(n);
//  openGLWidget->openGLThread->fog(f);
}

void MainWindow::precChanged(int n)
{
    ui->precValue->setNum(n);
//    openGLWidget->openGLThread->fog(f);
}

void MainWindow::eigenmodeChanged(int n /*=-1 */,bool bStartEigenmodes /* = true */)
{
//  Printf("MainWindow::eigenmodeValueChanged\n");
    if ( n >= 0 ) ui->eigenmodeValue->setValue(n); // yes, we can pass negative
    if ( bStartEigenmodes && ui->normalModes->checkState() ) {
        n = ui->eigenmodeValue->value();
        cThread->startEigenmodes(n);
        ui->eigenmodeValue->setEnabled(false);
    }
}

void MainWindow::calculateEigenmodesDone()
{
    Printf("MainWindow::calculateEigenmodesDone\n");
    ui->eigenmodeValue->setEnabled(true);
}

void MainWindow::normalModesChanged()
{
    Printf("MainWindow::normalModesChanged\n");
    if ( ui->eigenmodeValue->isEnabled() ) {
        if ( !ui->normalModes->checkState()) {
            cThread->stopEigenmodes();
        } else {
            eigenmodeChanged(-1,bConverged);
        }
    }
}

void MainWindow::setBackground(int c)
{
    openGLWidget->setBackground(c);
}

void MainWindow::setBackground(QColor& c)
{
    openGLWidget->setBackground(c);
}

void MainWindow::getBackground(QColor& c)
{
    openGLWidget->getBackground(c);
}

QColor MainWindow::getBackgroundColor()
{
    return openGLWidget->getBackgroundColor();
}

void MainWindow::red()      { setBackground('r') ; }
void MainWindow::green()    { setBackground('g') ; }
void MainWindow::blue()     { setBackground('b') ; }
void MainWindow::cyan()     { setBackground('c') ; }
void MainWindow::magenta()  { setBackground('m') ; }
void MainWindow::yellow()   { setBackground('y') ; }
void MainWindow::white()    { setBackground('w') ; }
void MainWindow::black()    { setBackground('k') ; }

void MainWindow::other()
{
    QColor color;
    getBackground(color);
    if ( bNativeDialogs )
        color = QColorDialog::getColor(color, this);
    else
        color = QColorDialog::getColor(color, this, "Select Color", QColorDialog::DontUseNativeDialog);
    if ( color.isValid())
        setBackground(color);
}

void MainWindow::fullScreen()
{
//  Printf("MainWindow::fullScreen\n");
//  pauseResume();
//  Qt::WindowFlags flags = this->windowFlags();
//  Qt::WindowFlags ontop = Qt::WindowStaysOnTopHint ;//  | Qt::Tool;

    if ( isFullScreen() ) {
        showNormal() ;
        ui->menuBar->show();
        ui->controls->show();
//      setWindowFlags(flags & ~ontop);
    } else {
        showFullScreen();
        if ( !bFullScreenMenubar ) ui->menuBar->hide();
        if ( !bFullScreenControls) ui->controls->hide();
//      setWindowFlags(flags | ontop);
    }
//  pauseResume();
}

void MainWindow::fullScreenControls(bool b)
{
    Printf(" MainWindow::fullScreenControls");
    bFullScreenControls = b ;
    theSettings->setValue(theSettings->sFullScreenControls,b?theSettings->vTrue:theSettings->vFalse);
}

void MainWindow::fullScreenMenubar(bool b)
{
    Printf(" MainWindow::fullScreenMenubar");
    bFullScreenMenubar  = b ;
    theSettings->setValue(theSettings->sFullScreenMenubar,b?theSettings->vTrue:theSettings->vFalse);
}

void MainWindow::nativeDialogs(bool b)
{
    Printf(" MainWindow::nativeDialogs\n");
    bNativeDialogs      = b ;
    theSettings->setValue(theSettings->sNativeDialogs,b?theSettings->vTrue:theSettings->vFalse);
}

void MainWindow::calcProgress(QString s)
{
    ui->calcLabel->setText(s);
}

// That's all Folks!
////
