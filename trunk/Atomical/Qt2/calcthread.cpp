#include "calcthread.h"

void __CPU_update(const double* oldx, const double* oldy,const double* oldz, double* x, double *y,double *z, double imbalance, int N, int mode){
        int i,j;
        double f_x,f_y,f_z,x0,y0,z0,x1,y1,z1,ud,ud3,pref;
        double DSTEP=0.01;

        for (i=0;i < N;i++) {
                x0=oldx[i];
                y0=oldy[i];
                z0=oldz[i];

                if(mode==3) f_x=-2*x0;
                f_y=-2*y0; // if sep>0 && Np2>0 fabs!
                f_z=-2*z0;

                for(j=0;j<N;j++) {
                        if(j!=i){
                                x1=oldx[j];
                                y1=oldy[j];
                                z1=oldz[j];
                                pref=1.0;
                                if(i%2) pref*=imbalance;
                                if(j%2) pref*=imbalance;
                                ud=1.0/sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)+(z1-z0)*(z1-z0));
                                ud3=pref*ud*ud*ud;
                                if(mode==3) f_x+=(x0-x1)*ud3;
                                f_y+=(y0-y1)*ud3;
                                f_z+=(z0-z1)*ud3;
                        }
                }

                if(mode==3) x[i]=oldx[i]+f_x*DSTEP;
                y[i]=oldy[i]+f_y*DSTEP;
                z[i]=oldz[i]+f_z*DSTEP;
        }
}

double __CPU_energy(const double* xx, const double* yy,const double* zz, double imbalance, int N){
        int i,k;
        double EE,x0,y0,x1,y1,z0,z1,pref,res;

        res=0.0;

        for (i=0;i < N;i++){
                EE=0.0;
                x0=xx[i];y0=yy[i];z0=zz[i];
                EE+=x0*x0+y0*y0+z0*z0;
                //EE+=0.5*(vxt[i]*vxt[i]+vyt[i]*vyt[i]+vzt[i]*vzt[i]);
                for(k=i+1;k<N;k++){
                        x1=xx[k];y1=yy[k];z1=zz[k];
                        pref=1.0;
                        if(i%2) pref*=imbalance;
                        if(k%2) pref*=imbalance;
                        EE+=pref/sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)+(z1-z0)*(z1-z0));
                }
                res+=EE;
        }

        return res;
}

void __CPU_poscpy(double* oldx, double* oldy, double *oldz,const double* newx, const double *newy,const double *newz, int N){
        memcpy(oldx,newx,N*sizeof(double));
        memcpy(oldy,newy,N*sizeof(double));
        memcpy(oldz,newz,N*sizeof(double));
}


// This stuff is based on the Mandelbrot example

void calcThread::setup(double *xxx,double *yyy,double *zzz,double imb,int NNp,int NNp2,int mmode){
    int i;

    printf("IN SETUP\n");

    Np=NNp;
    Np2=NNp2;
    imbalance=imb;
    mode=mmode;

    printf("Np=%d Np2=%d mode=%d imb=%e\n",Np,Np2,mode,imbalance);

    // setup memory for xx_old... and copy the passed positions to local variables
    // this is executed once at the creation of the object...

    xx=(double *)malloc(Np*sizeof(double));
    yy=(double *)malloc(Np*sizeof(double));
    zz=(double *)malloc(Np*sizeof(double));
    printf("1-2-3\n");
    xx_old=(double *)malloc(Np*sizeof(double));
    yy_old=(double *)malloc(Np*sizeof(double));
    zz_old=(double *)malloc(Np*sizeof(double));
    printf("4-5-6\n");

    // Do this the stupid way, then use memcpy...

    for(i=0;i<Np;i++){
        xx_old[i]=xxx[i];
        yy_old[i]=yyy[i];
        zz_old[i]=zzz[i];
        printf("Particle %d initialized\n",i+1);
    }
}

calcThread::calcThread() : QThread(){ // Constructor
    printf("IN CONSTRUCTOR\n");
    moveToThread(this); // Do I need this?

}

calcThread::~calcThread(){ // Destructor (...BAH!)
/*
    mutex.lock();
    abort=true;
    condition.wakeOne();
    mutex.unlock();
*/
    printf("IN DESTRUCTOR\n");
//    wait();

    free(xx);
    free(yy);
    free(zz);
    printf("1-2-3\n");
    free(xx_old);
    free(yy_old);
    free(zz_old);
    printf("4-5-6\n");

}

void calcThread::run(){
    printf("This will last...FOREVER!\n");fflush(stdout);
    forever{
        __CPU_update(xx_old, yy_old,zz_old, xx, yy, zz, imbalance, Np, mode);
        E=__CPU_energy(xx, yy, zz, imbalance, Np);
        __CPU_poscpy(xx_old, yy_old, zz_old, xx, yy, zz, Np);
        //printf("[IN] %e %e %e\n",xx[0],yy[0],zz[0]);
        emit stepDone(xx,yy,zz,&E);
        usleep(6000);
    }
    // run this thing
}

void calcThread::doCalc(){
    start(HighPriority);
}

/*
void calcThread::stepDone(double *xxx,double *yyy,double *zzz,double *EE){
    int i;

    *EE=E;

    for(i=0;i<Np;i++){
        xxx[i]=xx[i];
        yyy[i]=yy[i];
        zzz[i]=zz[i];
    }
    // YES! Then copy into xxx,yyy,zzz,E the local variables
}
*/
