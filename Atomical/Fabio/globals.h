/*
 *  globals.h
 *  Cocoa_GL
 *
 *  Created by Fabio Cavaliere on 05/10/10.
 *  Copyright 2010 Dipartimento di Fisica. All rights reserved.
 *
 */

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
int test;

float distance,angle,radsp,offsetx,offsety,delR,delC;
float u[98], c, cd, cm;

double myE,myEold,dperc,PREC,resc,DSTEP,Emax,radius,delta2,pprec,dtt,separation;
double *xx,*yy,*zz,*xx_old,*yy_old,*zz_old,*E,*rad,imbalance,*f_x,*f_y,*f_z;
double *ax,*ay,*az,*vx,*vy,*vz,*vxt,*vyt,*vzt,*vxn,*vyn,*vzn,faket;
double **Eivs,*Evls;

GLfloat tx[500],ty[500],tz[500];
GLfloat camYaw,camPitch;
#ifdef __cplusplus
}
#endif
#endif

