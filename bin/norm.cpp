// norm.cpp

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#ifdef  _MSC_VER
#define lstat _stat
#define stat  _stat
#endif

#define CR '\r'
#define TB '\t'
#define LF '\n'
#define BL ' '

#define TAB 4

/* ------------------------------------------------------------ */
/* detab                                                        */
/* ------------------------------------------------------------ */
void detab(FILE* f,char* line,size_t n,unsigned tab)
{
	size_t k = 0 ;
	for (  size_t i = 0 ; i < n ; i++)
	{
		int c = line[i] ;
		switch ( c )
		{
			case '\t' :
			{
				int n      = k % tab     ;
				int spaces = tab - n     ;
				for ( int j = 0; j < spaces+1  ; ++j) 	putc(BL,f);
				k = 1 ;
			}
			break ;
			
			default	:
				putc(c,f);
				k++ ;
			break ;
		}
	}
}

/* ------------------------------------------------------------ */
/* fix - rewrite file                                           */
/* ------------------------------------------------------------ */
bool fix(const char* path,void* buff,long l,unsigned int tab)
{
	char* buffer   = (char*) buff ;
	// do we have anything to do?
	bool  bFix     = false ;
	for ( size_t i = 0 ; !bFix && i < l ; i++ ) {
		bFix = buffer[i] == CR || buffer[i] == TB;
	}
		
	// yes, open the file and write the buffer line by line
	if ( bFix )
	{
		FILE* f = fopen(path,"wb") ;
		if (  f ) {
			size_t n = 0 ;
			char* line     = buffer       ;
			for ( size_t i = 0 ; i < l ; i++ ) {
				n++ ;
				if ( buffer[i] == CR )
					if ( i < l && buffer[i+1] == LF ) // DOS
						buffer[i++] = LF ;
					else                              // MAC
						buffer[i] = LF ;
				
				if ( buffer[i] == LF ) {              // UNIX (or MAC or DOS)
					detab(f,line,n,tab) ;
					n    = 0            ;
					line = buffer+i     ;
				}
			}			
			detab(f,line,n,tab) ;
			fclose(f) ;
		}
		
	}

	return bFix ;
}

/* ------------------------------------------------------------ */
/* main - main function of course                               */
/* ------------------------------------------------------------ */
int main(int argc,char* argv[])
{
	if ( argc < 2 ) {
		printf("syntax: norm filename ...\n") ;
		return 1 ;
	}
	int errors = 0 ;
	
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
				if ( !state && fix(path,buffer,l,TAB) ) state= FIXED ;
			} else state = NOOPEN ;
			
			free(buffer) ;
			printf("%s %s\n",path
				   , state == NOMEM  ? "insufficient memory"
				   : state == NOOPEN ? "cannot open"
				   : state == FIXED  ? "fixed"
				   : "clean"
				   ) ;
			if ( state == NOMEM || state == NOOPEN ) errors ++ ;
	}
	
	return errors ? 2 : 0  ;
}
