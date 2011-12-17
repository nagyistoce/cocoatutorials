#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int syntax()
{
	printf("syntax: crpipe pipename string\n") ;
	printf("eg:     crpipe /usr/bin/edit \"what is this?\"\n") ;
	printf("or:     crpipe /bin/csh      \"ls -alt > ${HOME}/test.txt\\n\"\n") ;
	return 1 ;
}


char* dup_and_tweak(char* string)
{
	char* s = strdup(string) ;
	
	const char* base = "\n\r\t\e\f\0" ;
	const char* X    = "nrtef\0"      ;
	int         l    = strlen(base)   ;
	int         i                     ;
	char        alphabet[256]         ;
	
	for ( i = 0 ; i < 255 ; i++ ) {
	   alphabet[i] = i ;
	}
	for ( i = 0 ; i < l ; i++ ) {
	 	alphabet[X[i]] = base[i] ;
	}
	
	    i = 0                 ; // input
	int o = 0                 ; // output
	    l = s ? strlen(s) : 0 ;
	
	while ( i < l-1 )
	{
	  	s[o++] = s[i] == '\\' ? alphabet[s[++i]] : s[i] ;
	  	i++ ;
	}
	if ( i == l-1) s[o++] = s[i++] ;
	s[o++] = 0      ;

	return s ;
}


int main(int argc, char** argv)
{
	if ( argc != 3 ) return syntax() ;
	
	char* pipename   = argv[1] ;
	char* string     = argv[2] ;
	
	FILE* pipe = popen(pipename,"w") ;
//	FILE* pipe = stdout ;
	if ( pipe ) {
		char* s = dup_and_tweak(string) ;
	 	fprintf(pipe,"%s",s) ;
	 	free((void*) s) ;
	 	fclose(pipe) ;
	} else {
		printf("unable to open pipe %s\n",pipename) ;
	}
	
//	int x ; printf("enter>") ; fflush(stdout) ; fscanf(stdin,"%c",&x) ;
	
	return 0 ;
}
