// crfile.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

FILE* fileopen(const char* filename,const char* options)
{
	FILE* f ;
#ifdef WIN32
		fopen_s(&f,filename,options) ;
#else
		f = fopen(filename,options) ;
#endif
	return f ;
}

int syntax() { printf("syntax: crfile filename count [unit]\n") ; return 1 ; }

int main(int argc, const char* argv[])
{
	if ( argc < 3 || argc > 4 ) return syntax() ;

	static char buffer[1024*1024] ;
	memset(buffer,0,sizeof buffer) ;

	int			err			= 0				; // error counter

	const char* filename	= argv[1]		;
	int			count		= atoi(argv[2]) ;
	char		unit		= argc == 4 ? argv[3][0] : 'b' ;
	int			size		= 0				;

	if ( !err && count < 0 ) {
		printf("illegal count = %d\n",count) ;
		err ++ ;
	}

	switch ( unit ) {
		case 'b' : size = 1				; break ;
		case 'k' : size = 1000			; break ;
		case 'm' : size = 1000 * 1000	; break ;
		case 'K' : size = 1024			; break ;
		case 'M' : size = 1024 * 1024	; break ;
	}

	if ( !size ) {
		printf("invalid unit %c - try b|k|m|K|M\n",unit) ;
		err ++ ;
	}

	FILE* f = fileopen(filename,"wb") ;
	if ( !f ) {
		printf("unable to open file %s\n",filename) ;
		err ++ ;
	}

	if ( !err ) {
		// printf("creating file %s of size %d x %d = %d\n",filename,size,count,size*count) ;
		for ( int i = 0 ; i < count ; i++ ) {
			fwrite(buffer,size,1,f) ;
		}
		fclose(f) ;
	}

	return err ? 2 : 0 ;
}

