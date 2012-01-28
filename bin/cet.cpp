#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char* argv[])
{
    char  buffer [16000] ;
    char  filename[1000] ;
    
    sprintf(filename,"%s",tmpnam(buffer)) ;
 // printf("filename = %s\n",filename) ;

    int count = 0 ;
    FILE* f = fopen(filename,"w") ;
    if ( f ) {
      int n ;
      while ( (n = fread(buffer,1,sizeof buffer,stdin)) )
      {
        fwrite(buffer,1,n,f) ;
        count += n ;
      }
      fclose(f) ;
      
      char cmd[2000] ;
      sprintf(cmd,"$CE \"%s\" $CE2",filename) ;
      system(cmd) ;
 //   printf("%s\n",cmd) ;
    }

    return 0 ;
    
    // unused
    argc ;
    argv ;
}