// jacobi.h
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef struct sort_struct{
  double E;
  int n;
} srtstr;

#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau); a[k][l]=h+s*(g-h*tau);

void jacobi(double **a, int n, double d[], double **v, int *nrot);

double QS_COMPARE(srtstr a, srtstr b);

void QuickSort(srtstr *list, int beg, int end);

void eigen_driver(double **M,double **V,double *L,int *lst, int N);
#ifdef __cplusplus
}
#endif
