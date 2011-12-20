#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
   if ( argc < 2 ) return printf("syntax: linky <linkname>+\n") ;
   
   for ( int a = 1 ; a < argc ; a++ ) {
      char* arg = argv[a] ;
      char  buffer[2000]  ;
      int result = readlink(arg,buffer,sizeof (buffer)-2) ;
      
      if ( result >= 0 ) {
        buffer[result] = 0 ;
        printf("%s -> %s\n",arg,buffer) ;
      } else {
        printf("**ERROR on %s\n",arg) ;
      }
   }
   

#if 0
   printf("named pipe = %0x\n",S_IFIFO) ;
   printf("character special = %0x\n",S_IFCHR) ;
   printf("directory = %0x\n",S_IFDIR) ;
   printf("block special = %0x\n",S_IFBLK) ;
   printf("regular = %0x\n",S_IFREG) ;
   printf("symbol link = %0x\n",S_IFLNK) ;
   printf("socket = %0x\n",S_IFSOCK) ;
   printf("whiteout = %0x\n",S_IFWHT) ;
   printf("set user id on execution = %0x\n",S_ISUID) ;
   printf("set group id on execution = %0x\n",S_ISGID) ;
   printf("save swapped text even after use = %0x\n",S_ISVTX) ;
#endif
    return argc ;
    //
    // #pragma unused(argv) ;
}
