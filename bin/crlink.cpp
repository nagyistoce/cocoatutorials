#include <stdio.h>
#include <stdlib.h>

int syntax()
{
	printf("syntax: crlink linkname linkedobject\n") ;
	printf("eg:     crlink libcurl.dylib /usr/local/lib/libcurl.4.0.0.dylib\n") ;
	return 1 ;
}


int main(int argc, char** argv)
{
	if ( argc != 3 ) return syntax() ;
	
	char* linkname   = argv[1] ;
	char* linkobject = argv[2] ;
	
	char command[10000] ;
	sprintf(command,"ln -s '%s' '%s'",linkobject,linkname) ;
	return system(command) ;
}
