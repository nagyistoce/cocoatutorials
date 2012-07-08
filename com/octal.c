#include <stdio.h>

int main(int argc,char* argv[])
{
	unsigned char c ;
	if ( argc != 2 ) {
		printf("octal <string>\n") ;
		return 0 ;
    } else {
		char* arg  = argv[1]     ;
	    int   l    = strlen(arg) ;
		int   i    ;
	    for ( i = 0 ; i < l ; i++ ) {
		  printf("\\%o",arg[i]) ;
	    }
		printf("\n") ;
    }

	return 0 ;
}
