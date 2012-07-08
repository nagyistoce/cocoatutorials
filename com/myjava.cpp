#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
	// printf("arguments: %d\n",argc) ;

	for ( int i = 0 ; i < argc ; i++ ) {
      // printf("arg: %d %s\n",i,argv[i]) ;
    }

	if ( i == 2 ) {
	  char  buffer[1000]       ;
	  char* filename = argv[1] ;
      char* pdest    = strstr( filename, ".class" );
      int   result   = pdest - filename ;
      if( pdest) {
	    strncpy(buffer,filename,result) ;
		buffer[result] = 0 ;
		// printf(".class = %s\n",buffer) ;

		int   c = 0     ; // count to last \ char
		char* p         ; // position of next \ char
		int   count = 0 ; // safety - prevent runaways
		while ( (p = strchr(filename+c,'\\')) && count++ < 100 ) {
          c  = p - filename +1 ;
	    }

		char command[1200] ;
		strcpy(command,"java ")  ;
		strcat(command,buffer+c) ;
		printf("Command = %s\n",command) ;
		system(command) ;
      }
    }

	return 0 ;
}
