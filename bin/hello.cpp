#include <stdio.h>
#include <sys/stat.h>


#define VERCHECK_VERSION_STRING "2004.11.19"
#define BUILDVERSION            "1"
#define BUILDDATE               "Fri Nov 19 15:00:00 2004"

// @@@BUILDINFO@@@ vercheck 2004.11.19 1 Tue Nov 09 17:59:55 2004
//#if defined(__DATE__) && defined(__TIME__)
//#define BUILDDATE __DATE__ ## " " ## __TIME__
//#else
//#define BUILDDATE  "DateUnknown"
//#endif

// START IDENTIFICATION STAMP
#if defined(WIN32)
#define IDENT_STAMP "@@@BUILDINFO@@@ vercheck " ## VERCHECK_VERSION_STRING ## " " ## BUILDVERSION ## " " ## BUILDDATE
#else
#define IDENT_STAMP "@@@BUILDINFO@@@ vercheck " VERCHECK_VERSION_STRING " " BUILDVERSION " " BUILDDATE
#endif
static const int     identStampIgnore  = 16         ; // skip the @@@...@@@ stuff when printing the version
static const char*   identStamp        = IDENT_STAMP;
// END IDENTIFICATION STAMP



int main(int argc, char* argv)
{
   printf("%s\n",identStamp+identStampIgnore)  ;
   
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

//   for ( int i = 128 ; i < 256 ; i++ )
//   {
//      printf("%d %x %c\n",i,i,i) ;
//   }


   return argc ;
   //
   // #pragma unused(argv) ;
}

