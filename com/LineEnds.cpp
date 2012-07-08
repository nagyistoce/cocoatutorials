#include <stdio.h>

int main(int argc, char* argv[])
{
    char* filename = argv[1] ;
    FILE* f = fopen(filename,"rb") ;
    int   n = 0 ;
    while ( !feof(f) ) {
        unsigned char c = fgetc(f) ;
        if ( c == '\n' || c == '\r') {
          unsigned char O = 'U' ;
          if ( c == '\r' && !feof(f) ) {
            unsigned char d = fgetc(f) ;
            if ( d == '\n' ) O = 'D' ;
            else {
              ungetc(d,f) ;
              O = 'M' ;
            }
          }
          printf("%c", O) ;
          n++ ;
          if ( n >50 ) {
              printf("\n") ;
              n = 0 ;
          }
        }
    }
    return 0 ;
}
