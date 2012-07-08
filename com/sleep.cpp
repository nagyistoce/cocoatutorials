#include <io.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>

#include <time.h>
#include <stdio.h>

struct tm *newtime;
time_t aclock;

int main(int argc, char* argv[])
{
    int l = 10 ;
    if ( argc < 2 ) {
		printf("Syntax: sleep <seconds>\n") ;
		return 0 ;
    } else {
	  l = atoi(argv[1]) ;
    }

    bool bSilent = argc > 2 ;

    time( &aclock );                 /* Get time in seconds */
    newtime = localtime( &aclock );  /* Convert time to struct */
                                     /* tm form */

    /* Print local time as a string */
    if ( !bSilent ) {
      fprintf(stderr,"Sleep.exe run at: %s", asctime( newtime ) );
	  fflush(stderr) ;
    }

	int i = 0 ;
	while ( i < l ) {
	  Sleep(1000) ;
      ++ i ;
	  if (!bSilent ){
        printf("Sleeping %d seconds\n",i) ;
	    fflush(stdout) ;
      }
    }

	return 0 ;
}