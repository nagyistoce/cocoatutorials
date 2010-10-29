#ifndef CALCTHREAD_H
#define CALCTHREAD_H

#include <QThread>
#include "math.h"

class calcThread : public QThread
{
    Q_OBJECT

public:
    calcThread();
    void setup(double *xxx,double *yyy,double *zzz,double imb,int NNp,int NNp2,int mmode);
    ~calcThread();
    void doCalc();

protected:
    void run();

signals:
    void done();
    void stepDone(double *xxx,double *yyy,double *zzz,double *EE);

private:
//    QMutex mutex;
//    QWaitCondition condition;
    bool restart;
    bool abort;

    double *xx,*yy,*zz,*xx_old,*yy_old,*zz_old;
    int Np,Np2,mode;
    double imbalance,E;

//private slots:
};

#endif // CALCTHREAD_H
