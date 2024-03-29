//
//  calcthread.h
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
#ifndef CALCTHREAD_H
#define CALCTHREAD_H

#include <QThread>
#include <Qt4.h>
#include "math.h"

class calcThread : public QThread
{
    Q_OBJECT

public:
    calcThread(int maxNp,int aEigenModeMaxNp);
   ~calcThread();

    void setup(double *xxx,double *yyy,double *zzz,double imb,double tprec,int NNp,int NNp2,int mmode);
    void doCalc();
    void pause();
    void resume();
    int  maxEigenmode();
    void eigen(int n);

    void startEigenmodes(int n); // call this from another thread
    void calculateEigenmodes(int n); // this is called from the run() loop
    void chooseEigenmode(int n);
    void stopEigenmodes(void);
    // data
  double oldE;
    bool isPaused;
  double imbalance,E,targetPrecision;

protected:
    void run();

signals:
    void isConverged();
    void done();
    void stepDone(double *xxx,double *yyy,double *zzz,double* EE);
    void calculateEigenmodesDone();
    void calcProgress(QString);

private:
    // General purpose
    void  __CPU_update();
  double  __CPU_energy();
    void  __CPU_poscpy();

    // The eigenvalue problem
    void eigsrt(double *d, double **v, int n);
    void twst(double **m, int i,int j,double cc,double ss,int n);
    void eigen_driver(int dim);

    void cshift(int ci,int i,double dd);
  double HH(int ci,int i,int cj,int j);
     int encode(int ci,int i);
    void decode(int ii,int *ci,int *i);
    void calc_Hessian(int dim);
    void alloc_Evals(void);
    void free_Evals(void);
    void Eigenmodes(void);

    //  data
    bool     restart;
    bool     abort;
    volatile int calculateEigenmode;

    int      nMaxNp;
    int      eigenModeMaxNp;
    double_p xx,yy,zz;
    double_p xx_old,yy_old,zz_old;

    int      Np,Np2,mode,do_calc;
    int      n_eigmode;

    // Vibrational modes
    bool    allocated_evals;
    double *Evls, **m2, **Eivs, **H;
    double *ax, *ay, *az, faket;
    int     olddim;

    int     lastNp;
    int     lastMode;
    int     lastDim;
};

#endif // CALCTHREAD_H

// That's all folks!
////
