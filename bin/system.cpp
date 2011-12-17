#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* expand(char* arg)
{
	static char result[1000] ;
	result[0] = 0     ;
	size_t l = strlen(arg) ;
	size_t i = 0           ;
	while ( i < l ) {
		char b[3] ;
		char c = arg[i] ;
		if ( c == '"') {
			b[0] = '\\' ;
			b[1] = c ;
			b[2] = 0 ;
		} else {
			b[0] = c ;
			b[1] = 0 ;
		}
		strcat(result,b) ;
		i++ ;
	}
	return result ;
}

int main(int argc,char* argv[])
{
	int i = 1 ;
	char command[1000] ;
	command[0] = 0 ;
	while ( i < argc ) {
		strcat(command,"\""   ) ;
		strcat(command,expand(argv[i])) ;
		strcat(command,"\" "  ) ;
		i++ ;
	}
	system(command) ;
}
