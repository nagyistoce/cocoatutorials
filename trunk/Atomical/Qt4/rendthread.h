#ifndef RENDTHREAD_H
#define RENDTHREAD_H

#include <QThread>
#include "glwidget.h"

class GLWidget;

class rendthread : public QThread
{
    Q_OBJECT

public:
    rendthread(GLWidget *wglWidget); // Constructor
    void setup(double imb,double sep,double rrsp,int NNp,int NNp2,int mmode); // set up openGL
    ~rendthread(); // Destructor
    void doRend(); // Start the damned thing

    void loadPositions(double *xxx,double *yyy,double *zzz);

protected:
    void run(); // The loop goes here

signals:
    void done(); // Empty as of now
    void frameNeeded();

private:
    GLWidget *glWidget;

    double *xx,*yy,*zz;
    int Np,Np2,mode;
    double imbalance,E,separation,radsp;

};

#endif // RENDTHREAD_H
