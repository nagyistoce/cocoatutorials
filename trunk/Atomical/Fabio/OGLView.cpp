//
//  OGLView.cpp
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

#include "../Qt2/Qt2.h"
#include "../Fabio/OGLView.h"
#include "../Qt2/window.h"


double CPU_rad(const double* xx, const double* yy,const double* zz, double *rad, int N){
        int i;
        double x0,y0,z0;

        double res=0.0;

        for(i=0;i<N;i++){
                x0=xx[i];y0=yy[i];z0=zz[i];
                rad[i]=sqrt(x0*x0+y0*y0+z0*z0);
                if(rad[i]>res) res=rad[i];
        }

        return res;
}

// Assuming that all the calculations are done in glWidget, this method needs as arguments
//
// double *xxx, *yyy, *zzz, *rad <--- DONE
// double imbalance, radsp
// int Np, Np2, mode
//

void OGLView::drawRect(double *xxx,double *yyy,double *zzz,double sep, double imb, double rrsp, int NNp, int NNp2, int mmode) const
{
        int i,NNpin;
        float alpha;
        double rradius;

        // this should go in the constructor later
        if ( !rrad_ ) {
            OGLView* that = (OGLView*) this ; // cast away the const
            that->rrad_ = (double*) malloc(NNp*sizeof(double));
        }

        rradius=CPU_rad(xxx, yyy, zzz, rrad_, NNp);
/*
        printf("drawRect()\n");
        printf("Np=%d mode=%d\n",NNp,mmode);
        printf("[***] %e %e %e\n",xxx[0],yyy[0],zzz[0]);
*/
	GLUquadric *glq;
        alpha=0.9;
	glq=gluNewQuadric();

        NNpin=0;
		
        for(i=0;i<NNp;i++){
                glPushMatrix();
                GLfloat e=(GLfloat)(rrad_[i]/rradius);
                if(mmode==2 && NNp2>0 && sep>0){
                        if(xxx[i]>0.0){
                                glColor4f(.5+.5*e,0,0,alpha);
                        } else {
                                glColor4f(0,0,.5+.5*e,alpha);
                        }
				
                } else {
                        if(i%2 && imb>1.0) glColor4f(e,0.2+0.8*(imb-1)/9,(1-e),alpha); else glColor4f(e,0,(1-e),alpha);
                }

//                if(!allocated_evals && verlet && i==NNp-1) glColor4f(0,1,0,alpha); // Bullet color

                glTranslatef(xxx[i], yyy[i], zzz[i]);
                if(i%2) gluSphere(glq, 0.2+0.2*(imb-1)/4, 40, 40); else gluSphere(glq, 0.2, 40, 40);
                if(rrad_[i]<=rrsp) NNpin++;
                glPopMatrix();
	}

	glPushMatrix();
	glColor4f((GLfloat)1,(GLfloat)1,(GLfloat)0,(GLfloat)0.7);
	glTranslatef(0, 0, 0);
        gluSphere(glq, rrsp, 40, 40);
	glPopMatrix();
	
	gluDeleteQuadric(glq);
	
        glFlush();	// Single buffering...
}

/*
void OGLView::oneShot()
{
    CPU_update(xx_old, yy_old,zz_old, xx, yy, zz, Np);
    CPU_poscpy(xx_old, yy_old,zz_old, xx, yy, zz, Np);
    CPU_energy(xx, yy, zz, E, Np);
}

void OGLView::oneShot2(){
    drawRect(xx, yy, zz, imbalance, radsp, Np, Np2, mode); // This call will be in glWidget
}
*/

OGLView::OGLView(QObject *parent)
: QObject(parent)
, rrad_(NULL)
{
    /*
    MaxNp=5001;
    mem_alloc();

    mode=3;
    Np=96;
    Np2=0;

    separation=0.0;
    imbalance=1.0;

    PREC=1e-11;
    radsp=0.0;

    srand(time(NULL));
    rmarin(rand()%31329,rand()%30081);

    Initialize();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(oneShot()));
    timer->start(16);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(oneShot2()));
    timer->start(0);
    */
}

OGLView::~OGLView()
{
    if ( rrad_) ::free(rrad_);
}

void OGLView::draw(double *xxx, double *yyy, double *zzz, double sep, double imb, double rrsp, int NNp, int NNp2, int mmode) const // (xx, yy, zz,imb, rrsp, NNp, NNp2, mmode)
{
//    printf("draw() %e %e %e\n",xxx[0],yyy[0],zzz[0]);
    drawRect(xxx, yyy, zzz, sep, imb, rrsp, NNp, NNp2, mmode);
}

// That's all Folks!
////
