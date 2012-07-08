
# include <stdio.h>
# include <malloc.h>
# include <time.h>
# include <string.h>
# include <stdlib.h>

char* now()
{
    long  tloc = 1       ;
    time(&tloc)          ;
    return strdup((ctime(&tloc))) ;
}

int main(int argc,char* argv[])
{
    int     i               ;
    int     l        = 0    ;
    char*   line     = NULL ;
    int     result   = 0    ;
    char*   oldtime  = NULL ;
    char*   newtime  = NULL ;
    clock_t oldclock        ;
    clock_t newclock        ;
    float   n,o,d,c         ;

    for ( i = 1 ; i < argc ; i ++ ) l += strlen(argv[i]) + 1 ;

    if ( l ) line = malloc(l+1) ;

    if ( line ) {
      line[0] = 0 ;

      for ( i = 1 ; i < argc ; i++ ) {
        strcat(line,argv[i]) ;
        strcat(line," "    ) ;
      }

      oldclock = clock()      ;
      oldtime  = now()        ;
      result   = system(line) ;
      newtime  = now()        ;
      newclock = clock()      ;

      fprintf(stderr,"------------------------\n") ;
      fprintf(stderr,"%s%s",oldtime,newtime) ;
      o = oldclock ; n = newclock ; c = CLOCKS_PER_SEC ; d = (n - o) / c ;
      fprintf(stderr,"%f seconds\n",d) ;
      fprintf(stderr,"------------------------\n") ;
    }

    if ( oldtime ) free(oldtime) ;
    if ( newtime ) free(newtime) ;
    return result ;

}
