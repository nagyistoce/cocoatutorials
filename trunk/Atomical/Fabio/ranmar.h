//
//  ranmar.h
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

// That's all Folks!
////
