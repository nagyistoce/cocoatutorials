// if you use carbon, you can build with make fullpath "LDFLAGS=-framework Carbon"
#define CARBON 0

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <windows.h>
bool realpath(char* file,char* path)
{
	GetFullPathName(file,512,path,NULL);
	return true;
}
#else
#include <unistd.h>
#include <sys/param.h>
#endif

#if CARBON
#include <Carbon/Carbon.h>
static OSStatus err = 0 ;
#endif

static int err = 0 ;

enum { kPathBufferSize = 10000 } ;

int main(int argc, char** argv)
{
	char buff[kPathBufferSize] ;

	if ( argc < 2 )
	{
		printf("syntax: pathname [file]+\n") ;
		err = 1 ;
	}

#if CARBON
	for ( int i = 1 ; !err && i < argc ; i++ )
	{
		char* filename = argv[i] ;
		FSRef ref;
		err = ::FSPathMakeRef ((const UInt8*) filename, &ref, NULL);

		if ( !err ) {
			err = ::FSRefMakePath (&ref, (uint8_t*) buff, kPathBufferSize);
		}

		if ( err ) sprintf(buff,"ERROR %d",err) ;
		printf("%s\n",buff) ;
	}
#else
	for ( int i = 1 ; !err && i < argc ; i++ )
	{
		char* filename = argv[i] ;
		if ( !realpath(filename,buff) )
		{
			err = errno ;
			sprintf(buff,"ERROR %d",err) ;
		}
		printf("%s\n",buff) ;
	}
#endif
	return err ;
}