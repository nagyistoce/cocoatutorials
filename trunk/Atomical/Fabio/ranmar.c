//
//  ranmar.c
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

#include<stdio.h>
#include<stdlib.h>
#include"globals.h"

void rmarin(int ij,int kl){
  int i,j,k,l,ii,jj,m;
  float s,t;
  
  if (ij<0 || ij>31328 || kl<0 || kl>30081) {
    printf("The first random number seed must have a value between 0 and 31328.");
    printf("The second seed must have a value between 0 and 30081.");
    exit(1);
  }
  
  i = (ij/177)%177 + 2;
  j = ij%177 + 2;
  k = (kl/169)%178 + 1;
  l = kl%169;
  
  for (ii=1; ii<=97; ii++) {
    s = 0.0;
    t = 0.5;
    for (jj=1; jj<=24; jj++) {
      m = (((i*j)%179)*k) % 179;
      i = j;
      j = k;
      k = m;
      l = (53*l + 1) % 169;
      if ((l*m)%64 >= 32) s += t;
      t *= 0.5;
    }
    u[ii] = s;
  }
  
  c = 362436.0 / 16777216.0;
  cd = 7654321.0 / 16777216.0;
  cm = 16777213.0 / 16777216.0;
  
  i97 = 97;
  j97 = 33;
  
  test = TRUE;
}

void ranmar(float rvec[],int len){
  int ivec;
  float uni;
  
  if (test==FALSE) {
    printf("Call the init routine rmarin() before calling ranmar().");
    exit(2);
  }
  for (ivec=1; ivec<=len; ivec++) {
    uni = u[i97] - u[j97];
    if (uni < 0.0) uni += 1.0;
    u[i97] = uni;
    i97--;
    if (i97==0) i97 = 97;
    j97--;
    if (j97==0) j97 = 97;
    c -= cd;
    if (c<0.0) c += cm;
    uni -= c;
    if (uni<0.0) uni += 1.0;
    rvec[ivec] = uni;
  }
}

// That's all Folks!
////
