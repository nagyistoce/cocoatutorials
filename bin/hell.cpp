#include <stdio.h>
#include <stdlib.h>
//#include <console.h>

char* __ttyname(int fildes)
{
    if ( 0 <= fildes && fildes <= 2 ) return "vomit" ;
    return NULL ;
}

int main(int argc,char* argv[])
{

    printf("Hello world\n") ;
    
    //while ( !feof(stdin) ) {
    //  char buff[2000] ;
    //  int  l  = sizeof(buff) - 2 ;
    //  int  n  = fread(buff,1,l,stdin) ;
    //  buff[n] = 0 ;
    //  fwrite(buff,1,n,stdout) ;
   // }
    
    
//    system("edit hello.cc") ;

    printf("invoke ExtendScript shell\n") ;
    system("cd /Scripting/gptech/extendscript/public/libraries/macintosh/release && shell -c \"testAll()\" < /dev/tty") ; //shell.txt") ;

    
    return 0 ;
    
    // unused
    argc ;
    argv ;
}