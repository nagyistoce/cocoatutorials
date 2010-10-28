//
//  globals.h
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
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "../Qt2/Qt2.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define REND_20 50000
#define REND_30 33333
#define REND_50 13333

#define CALC_H 800
#define CALC_M 5000
#define CALC_L 13333

#define ALPHAVAL 0.95

#ifdef __cplusplus
extern "C" {
#endif

int Np2,trail,rendering,running,running2,started,do_update,impurity,centered,mode,Np,mx,my,zmx,zmy,MaxNp,wishNp,converged,is_frozen,Npin,is_paused,autozoom;
int allocated_evals;
int delayC,delayR,verlet,*idx,olddim,selected_eigenmode,terminate,terminatedR,terminatedC,POV,dbuf;
int i97, j97, completed;
int test,status;

//float distance,angle,radsp,offsetx,offsety,delR,delC;
float u[98], c, cd, cm;
float angle,radsp,offsetx,offsety,delR,delC;

double myE,myEold,dperc,PREC,resc,DSTEP,Emax,radius,delta2,pprec,dtt,separation;
double *xx,*yy,*zz,*xx_old,*yy_old,*zz_old,*E,*rad,imbalance,*f_x,*f_y,*f_z;
double *ax,*ay,*az,*vx,*vy,*vz,*vxt,*vyt,*vzt,*vxn,*vyn,*vzn,faket;
double **Eivs,*Evls;

GLfloat tx[500],ty[500],tz[500];
//GLfloat camYaw,camPitch;
#ifdef __cplusplus
}
#endif
#endif

// That's all Folks!
////
