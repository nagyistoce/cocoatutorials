#include <stdio.h>

int main(int argc,char* argv[])
{
	unsigned char c ;
	for ( c = 32 ; c <= 127 ; c++ ) {
		printf("%d %x %c %o\n",c,c,c,c) ;
    }

	return 0 ;
}

