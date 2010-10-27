/*
 *  ranmar.h
 *  Atomical
 *
 *  Created by Fabio Cavaliere on 07/10/10.
 *  Copyright 2010 Dipartimento di Fisica. All rights reserved.
 *
 */

#pragma once
#ifndef _RANMAR_H_
#define _RANMAR_H_

#ifdef __cplusplus
extern "C" {
#endif

void rmarin(int ij,int kl);
void ranmar(float rvec[],int len);


#ifdef __cplusplus
}
#endif

#endif


