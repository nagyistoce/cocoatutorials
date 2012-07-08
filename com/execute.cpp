#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

int main(int argc,const char* argv[])
{
	if ( argc < 2 ) {
		printf("syntax: execute file...\n") ;
		return 0 ;
    }

	for ( int i = 1 ; i < argc ; i++ ) {
		char* arg = strdup(argv[1]) ;
		int   x   = 0 ;
		int   l   = strlen(arg) ;
		if ( arg[0] == '"' ) {
			x ++ ;
			arg[l-1] = 0 ;
	    }

		WinExec(arg+x,SW_NORMAL) ;

		free(arg) ;
    }

	return 0 ;
}

