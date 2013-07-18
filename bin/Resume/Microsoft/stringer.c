#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ----------------------------------------------------------- */
/* normalize - convert DOS,UNIX,MAC line endings to UNIX       */
/* DOS = \r\n  MAC = \r  UNIX = \n                             */
/* ----------------------------------------------------------- */
void normalize(char* s)
{
    int   r = 0 ; // read position
    int   w = 0 ; // write position
	for ( r = 0 ; s[r] ; r++ ) {
		char c =  s[r] ;
		if ( c == '\r' )
			if ( s[r+1] == '\n')
				r++ ; // ignore \n following \r
		if ( c == '\r' )
			c = '\n' ; // convert every \r to \n
		s[w++] = c ;
	}
	s[w] = 0 ;
}

char* a(char c) /* return ascii version of a char */
{
	static char buffer[10] ;
	sprintf(buffer,(32 <= c && c <= 127) ? "%c" : " %#02x " ,c);
	return buffer ;
}

int main(int argc,const char* argv[])
{
	int   i ;
	
	const char* S = "Apple\r\nBanana\r\nCarrot\rbanana\npear\r\napple\rstrawberry" ;
	char        s[256]  ;
	strcpy(s,S) ;
	
	normalize(s) ;
	
	for ( i = 0 ; i < strlen(s) ; i++ )
		printf("%s",a(s[i])) ;
	printf("\n");

	return 0 ;
}
