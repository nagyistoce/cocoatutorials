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
#include "math.h"

class calcThread : public QThread
{
    Q_OBJECT

public:
             calcThread();
    virtual ~calcThread();

    void setup(double *xxx,double *yyy,double *zzz,double imb,int NNp,int NNp2,int mmode);
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

// That's all Folks!
////
