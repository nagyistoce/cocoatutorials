#include <io.h>
#include <stdio.h>
#define FULL 0

int main(int argc, char* argv[],char* env[])
{
#if FULL
    printf ("------ arguments -------\n") ;
#endif
    int i ;
    for ( i = 0 ; i < argc ; i++ ) {
	  printf("%d: %s\n",i,argv[i]) ;
	  fflush(stdout)        ;
    }
#if FULL
    printf ("------ environment -------\n") ;
	char* e = env[0] ;
	i       = 0 ;
	while ( e ) {
		printf("%s\n",e) ;
		e = env[++i] ;
    }
#endif
    fflush(stdout)        ;
	return 0 ;
}