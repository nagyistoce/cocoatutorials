//
//  jacobi.h
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

// That's all Folks!
////
