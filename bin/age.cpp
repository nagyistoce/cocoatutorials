#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include <errno.h>

int Syntax()
{
    return printf("Syntax: age [filename]+\n") ;
}

typedef struct buff_s { char buffer[100] ; } buff_t ;
#define lengthof(x) (sizeof(x)/sizeof(x[0]))

const char* age(unsigned long delta)
{
    // static data tables
    static int     t[] = { 365*24*60*60 , 30*24*60*60 , 24*60*60, 60*60,  60,   1 } ;
    static char    u[] = {          'y' ,         'M' ,      'd',   'h', 'm', 's' } ;
    
    // static result buffers
    static buff_t results[3]            ;
    static int  b = 0                       ;
    if ( b >= lengthof(results) ) b = 0 ;
    
    // set up buffer and length
    char* result = results[b++].buffer  ;
    int l = result[0]   = 0 ;
    
    // calculate and format result
    unsigned long T = delta ;
    for ( int i = 0 ; i < lengthof(t) ; i++ ) {
        unsigned long R      = T / t[i] ;
        T                   -= R * t[i] ;
        bool bEnd            = i == (lengthof(t) - 1) ;
        if ( R || bEnd ) l  += sprintf(result+l,"%d%c ",(int)R,u[i]) ;
    }
    
    return result ;
}
// wee change from Xcode
int main( int argc, const char* argv[] )
{
    int result = 1 ;
    if ( argc < 2 ) return Syntax() ;
    for ( int a = 1 ; a < argc ; a ++ ) 
    {
        const char* arg = argv[a] ;
        struct stat buf      ;
        const char* filename = arg ;

        result   = stat(arg, &buf );

        /* Check if statistics are valid: */
        if( result != 0 )
            printf( "Problem getting information from file %s\n",arg );
            else
            {
        //      what's the current date?
            time_t      now ;
            time      (&now);
            printf("%s: ",filename) ;
            printf(" created: %s" ,age(now-buf.st_ctime)) ;
            printf(" modified: %s",age(now-buf.st_mtime)) ;
            printf(" accessed: %s",age(now-buf.st_atime)) ;
            printf("\n") ;
            }
        }
        
    return result ;
}