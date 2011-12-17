#include <stdio.h>

int main(int argc, char* argv[])
{
	if ( argc < 2 ) {
		printf("usage rmshit file+\n") ;
	} else {
		for ( int i = 1 ; i < argc ; i++ ) {
			char filename[10000] ;
			sprintf(filename,"%s/..namedfork/rsrc",argv[i]) ;
			FILE* f = fopen(filename,"w") ;
			if ( !f ) printf("cannot open %s\n",filename) ;
			if (  f ) fclose(f) ;
		}
	}
	return 0 ;
}