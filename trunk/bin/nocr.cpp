#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

int print(int c)
{
	int result = 1 ;
	if ( c == 10 || c == 13 ) result = 0 ;
	return result ;
}

#ifdef  _MSC_VER 
#define lstat _lstat
#endif

#define CR '\r'
#define LF '\n'


bool fixcr(const char* path,void* buff,long l)
{
	bool bCR     = false ;
	char* buffer = (char*) buff ;
	if ( buffer && l ) { 
		long i   ;
		for ( i = 0 ; !bCR && i < l ; i++ )
			bCR = buffer[i] == CR ;
	
		if ( bCR ) {
			FILE* f = fopen(path,"wb") ;
			if ( f ) {
				for ( i = 0 ; i < l ; i++ ) {
					if ( buffer[i] == CR )
						if ( i < l && buffer[i+1] == LF ) // DOS
							i++ ;
						else                              // MAC
							buffer[i] = LF ;

					if ( buffer[i] !=  CR ) 
						fputc(buffer[i],f) ;
				}
				
				fclose(f) ;
			}
		}
	}
	return bCR ;
}

int main(int argc,char* argv[])
{
	if ( argc < 2 ) return printf("syntax: nocr filename ...\n") ;
	
	enum 
	{ OK = 0
	, NOMEM 
	, NOOPEN
	, FIXED
	} state = OK ;


	for ( int i = 1 ; i < argc ; i++ )
	{
		long         l       = 0       ;
		void*        buffer  = NULL    ;
		bool         bOpened = false   ;
		const char*  path    = argv[i] ;
		struct stat  buf     ;
		
		int error   = lstat(path, &buf );

		if ( !error ) {
	   		l = buf.st_size ;
	   		if ( l ) buffer = malloc(l) ;
	   		if ( buffer ) {
	   			FILE* f = fopen(path,"rb") ;
	   			if (  f ) {
	   				fread(buffer,l,1,f) ;
	   				fclose(f) ;
	   			} else {
	   				state = NOOPEN ;
	   			}
	   		} else if ( l ) state = NOMEM ;
	   		if ( !state && fixcr(path,buffer,l) ) state= FIXED ;
	    } else state = NOOPEN ;

	   	free(buffer) ;
	    printf("%s %s\n",path
	                    , state == NOMEM  ? "insufficient memory"
	                    : state == NOOPEN ? "cannot open"
	                    : state == FIXED  ? "fixed"
	                    : "clean"
	                    ) ;

	}
	return 0 ;
}


