#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	for ( int i = 1 ; i < argc ; i++ ) {
		char c = atoi(argv[i]) ;
		putc(c,stdout) ;
	}
	
	if ( argc > 1 ) putc('\n',stdout);
	
	return 0 ;

}