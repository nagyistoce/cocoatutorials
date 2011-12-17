#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char print(unsigned char c)
{
	return ( 32 <= c && c <= 127 ) ? c : '?' ;
}

int main(int argc, char* argv[])
{
	int i = 0 ;

	while ( i < argc ) {
		printf("%-2d: %s\n",i,argv[i]) ;
		i++ ;
	}
/*
    char* arg = argv[1] ;
    for ( i = 0 ; i < strlen(arg) ; i++ ) {
    	unsigned char c = (unsigned char) arg[i] ;
    	printf("%2d %ud=%c\n",i,c,print(c)) ;
    }
*/
/*
	while ( !feof(stdin) )
	{
		int  c = getc(stdin) ;
		if ( c != EOF ) putc(c,stdout) ;
	}
*/
    return 0 ;
}