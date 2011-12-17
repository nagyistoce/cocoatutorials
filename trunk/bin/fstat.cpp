/* fstat.cpp:
 * This program uses the _stat function to report information about a file
 */

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <sys/param.h>
#include <errno.h>

const char* ext(const char* filename)
{
	int l = strlen(filename) -1 ;
	if ( !l ) l = 1 ;
	while ( l && filename[l] != '.' && filename[l] != '/') l-- ;
	if ( filename[l] == '/' ) l = 0 ;
	return  l ? filename+l : "--none--" ;
}

int Syntax()
{
	return printf("Syntax: fstat filename\n") ;
}

long long _filelength(int fd)
{
	struct stat buf ;
	long long result = -1 ;
	if ( fstat(fd,&buf) == 0 ) {
		result = buf.st_size ;
	}
	return result ;
}

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
   			char pathname[PATH_MAX] ;
   			realpath(filename,pathname) ;
    	//	Output some of the statistics:
    	  	printf( "Filename      : %s\n",filename) ;
    	  	printf( "Base          : %s\n",basename(filename)) ;
    	  	printf( "Extension     : %s\n",ext(filename)) ;
    	  	printf( "Directory     : %s\n",dirname(filename)) ;
    	  	printf( "Realpath      : %s\n",pathname) ;
    	  	printf( "Realdirectory : %s\n",dirname(pathname)) ;
    	  	printf( "File size     : %lld\n", buf.st_size );
    	 // printf( "Drive         : %c:\n", buf.st_dev + 'A' );
    	  	printf( "Time modified : %s", ctime( &buf.st_mtime ) );

      
    	  	char      sShortDate[100] ;
    	  	char      sLongDate[100]  ;
    	  	struct tm*  ftime         ;

    	// 	what's the current date?
    	// 	time_t      ltime ;
    	// 	time      (&ltime);
    	// 	struct tm*  today = localtime( &ltime );

    	// 	strftime( sShortDate, sizeof sShortDate,"%c" ,today );
    	// 	strftime( sLongDate , sizeof sLongDate ,"%+"/*"%#c"*/,today );

    	//	printf("\n") ;
    	// 	printf("Current ShortDate = %s\n",sShortDate) ;
    	//	printf("Current LongDate  = %s\n",sLongDate) ;


    		ftime = localtime( &buf.st_ctime );
    	  	strftime( sShortDate, sizeof sShortDate,"%c" ,ftime );
    	  	strftime( sLongDate , sizeof sLongDate ,"%+"/*"%#c"*/,ftime );
    	  	printf("FileChanged:    ShortDate = %s LongDate = %s\n",sShortDate,sLongDate) ;

    		ftime = localtime( &buf.st_mtime );
    	  	strftime( sShortDate, sizeof sShortDate,"%c" ,ftime );
    	  	strftime( sLongDate , sizeof sLongDate ,"%+"/*"%#c"*/,ftime );
    	  	printf("FileModified:   ShortDate = %s LongDate = %s\n",sShortDate,sLongDate) ;

    		ftime = localtime( &buf.st_atime );
    	  	strftime( sShortDate, sizeof sShortDate,"%c" ,ftime );
    	  	strftime( sLongDate , sizeof sLongDate ,"%+"/*"%#c"*/,ftime );
    	  	printf("FileAccessed:   ShortDate = %s LongDate = %s\n",sShortDate,sLongDate) ;

    	//  printf("File    Size      = %lld\n",buf.st_size) ;
    		
    	//	printf("sizeof of size = %d\n",sizeof(buf.st_size)) ;
    	//	FILE* f = fopen(arg,"r") ;
    	//	if (  f ) printf("%lld\n",_filelength(fileno(f))) ;
    	//	if (  f ) fclose(f) ;
   		}
   	}
   	
	return result ;
}
