#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lengthof(x) (sizeof(x)/sizeof(*x))

int main(int argc,char* argv[])
{
	char cmd[2000] ;
	int  lcmd = lengthof(cmd)-100;
	int  l = sprintf(cmd,"kate ");
	for ( int i = 1 ; i < argc ; i++ ) {
		const char* filename = argv[i];
		if ( (strlen(filename)+l) < lcmd )
			l+=sprintf(cmd+l," \"%s\"",filename) ;
	}
	l+=sprintf(cmd+l," 2>/dev/null >/dev/null &") ;
	cmd[l]=0;
	system(cmd) ;

	return 0 ;
    
    // unused
    argc ;
    argv ;
}
