#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

int syntax()
{
	printf("syntax: timestamp <seconds>+\n") ;
	return 1 ;
}

int main(int argc,char* argv[])
{
	if ( argc < 2  ) return syntax() ;

	int 	arg = 1 ;
	while ( arg < argc )
	{
		unsigned int time  = (long) atoi(argv[arg++]) ;
		time_t clock = (clock_t) time           ;
		printf("%u = %s",time,ctime(&clock))    ;
	}

	return 0 ;
}




