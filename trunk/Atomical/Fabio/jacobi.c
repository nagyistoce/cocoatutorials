#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../Qt2/Qt2.h"
#include "nrutil.h"
#include "jacobi.h"
#include "globals.h"

void jacobi(double **a, int n, double d[], double **v, int *nrot){
	int j,iq,ip,i;
	double tresh,theta,tau,t,sm,s,h,g,c,*b,*z;

	b=vector(1,n);
	z=vector(1,n);
	for (ip=1;ip<=n;ip++) {
		for (iq=1;iq<=n;iq++) v[ip][iq]=0.0;
		v[ip][ip]=1.0;
	}
	for (ip=1;ip<=n;ip++) {
		b[ip]=d[ip]=a[ip][ip];
		z[ip]=0.0;
	}
	*nrot=0;
	for (i=1;i<=250;i++) { // It was 50
		sm=0.0;
		for (ip=1;ip<=n-1;ip++) {
			for (iq=ip+1;iq<=n;iq++)
				sm += fabs(a[ip][iq]);
		}
		if (sm == 0.0) {
			free_vector(z,1,n);
			free_vector(b,1,n);
			return;
		}
		if (i < 4)
			tresh=0.2*sm/(n*n);
		else
			tresh=0.0;
		for (ip=1;ip<=n-1;ip++) {
			for (iq=ip+1;iq<=n;iq++) {
				g=100.0*fabs(a[ip][iq]);
				if (i > 4 && (double)(fabs(d[ip])+g) == (double)fabs(d[ip])
					&& (double)(fabs(d[iq])+g) == (double)fabs(d[iq]))
					a[ip][iq]=0.0;
				else if (fabs(a[ip][iq]) > tresh) {
					h=d[iq]-d[ip];
					if ((double)(fabs(h)+g) == (double)fabs(h))
						t=(a[ip][iq])/h;
					else {
						theta=0.5*h/(a[ip][iq]);
						t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c=1.0/sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0+c);
					h=t*a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq]=0.0;
					for (j=1;j<=ip-1;j++) {
						ROTATE(a,j,ip,j,iq)
					}
					for (j=ip+1;j<=iq-1;j++) {
						ROTATE(a,ip,j,j,iq)
					}
					for (j=iq+1;j<=n;j++) {
						ROTATE(a,ip,j,iq,j)
					}
					for (j=1;j<=n;j++) {
						ROTATE(v,j,ip,j,iq)
					}
					++(*nrot);
				}
			}
		}
		for (ip=1;ip<=n;ip++) {
			b[ip] += z[ip];
			d[ip]=b[ip];
			z[ip]=0.0;
		}
	}
	nrerror("Too many iterations in routine jacobi");
}

double QS_COMPARE(srtstr a, srtstr b){
  return a.E-b.E;
}

void QuickSort(srtstr *list, int beg, int end){
  srtstr piv; srtstr tmp;
    
  int  l,r,p;

  while (beg<end)    // This while loop will substitude the second recursive call
    {
      l = beg; p = (beg+end)/2; r = end;

      piv = list[p];

      while (1)
        {
	  while ( (l<=r) && ( QS_COMPARE(list[l],piv) <= 0 ) ) l++;
	  while ( (l<=r) && ( QS_COMPARE(list[r],piv)  > 0 ) ) r--;

	  if (l>r) break;

	  tmp=list[l]; list[l]=list[r]; list[r]=tmp;

	  if (p==r) p=l;
            
	  l++; r--;
        }

      list[p]=list[r]; list[r]=piv;
      r--;

      // Select the shorter side & call recursion. Modify input param. for loop
      if ((r-beg)<(end-l))   
        {
	  QuickSort(list, beg, r);
	  beg=l;
        }
      else
        {
	  QuickSort(list, l, end);
	  end=r;
        }
    }   
}

void eigsrt(double *d, double **v, int n){
	int k,j,i;
	double p;
	
	for (i=1;i<n;i++) {
		p=d[k=i];
		for (j=i+1;j<=n;j++)
			if (d[j] >= p) p=d[k=j];
		if (k != i) {
			d[k]=d[i];
			d[i]=p;
			for (j=1;j<=n;j++) {
				p=v[j][i];
				v[j][i]=v[j][k];
				v[j][k]=p;
			}
		}
	}
}

void eigen_driver(double **M,double **V,double *L,int *lst, int N){
	double **MM, **VV, *LL;
	int i,j,nr;
	srtstr *evls;
	
	MM=matrix(1,N,1,N);
	VV=matrix(1,N,1,N);
	LL=vector(1,N);
	
	evls=(srtstr *)malloc(N*sizeof(srtstr));
	printf("\n\n--- eigen_driver() ---\n");
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			MM[i+1][j+1]=M[i][j];	
			printf("%e ",M[i][j]);
		}
		printf("\n");
	}

	jacobi(MM,N,LL,VV,&nr);
	eigsrt(LL,VV,N);
	
	for(i=0;i<N;i++){
		L[i]=LL[i+1];
		evls[i].E=L[i];
		evls[i].n=i;
		for(j=0;j<N;j++){
		  V[i][j]=VV[i+1][j+1];	
		}
	}
	
	free_vector(LL,1,N);
	free_matrix(MM,1,N,1,N);
	free_matrix(VV,1,N,1,N);
	
	free(evls);
        UNUSED(lst);
}

