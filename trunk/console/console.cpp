/*
 *  console.cpp
 *  console+library
 *
 *  Created by Robin Mills on 8/13/10.
 *  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
 *
 */
#include <stdio.h>
#include "myLib.h"

int main (int argc, const char * argv[])
{
    // insert code here...
    printf("Hello, World!\n");
	int n = helloLib() ;
	printf("n = %d\n",n) ;
    return 0;
}




