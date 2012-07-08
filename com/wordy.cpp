#include <stdio.h>

int Syntax()                 { return printf("Syntax: wordy <filename>\n") ; }
int CantOpen(char* filename) { return printf("Cannot open %s\n",filename)  ; }

int main(int argc, char* argv[])
{
    if ( argc != 2 ) return Syntax() ;

    // open the file
    char* filename = argv[1]              ;
    FILE* f        = fopen(filename,"rb") ;
    if ( !f ) return CantOpen(filename) ;


    // initialize the counters
    int k     = 0  ;
    int count[256] ;
    int i          ;
    for ( i = 0 ; i < sizeof count/sizeof count[0] ; i++ ) {
      count[i] = 0 ;
    }

    // read file and count the bytss
    int c ;
    while ( (c = fgetc(f)) != EOF ) {
        count[c] ++ ;
        k++ ;
    }
    fclose(f) ;

    // report the result
    for ( i = 0 ; i < sizeof count/sizeof count[0] ; i++ ) {
      if (count[i] ) {
        double r = 100.0 * ((double) count[i]) / ((double) k ) ;
        printf ( "%3d , %d , %f\n",i,count[i],r) ;
      }
    }
}