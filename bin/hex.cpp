#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	if ( argc > 1 ) {
		for ( int i = 1 ; i < argc ; i++ ) {
			char* arg = argv[i] ;
			printf("%s = %#x\n",arg,atoi(arg)) ;
		}
	}
	return 0 ;
}