#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lengthof(x) (sizeof(x)/sizeof(*x))

int main(int argc,char* argv[])
{
	char cmd[2000] ;
	int  lcmd = lengthof(cmd)-100;
	int  l = sprintf(cmd,"ps ax | grep [k]ate 2>&1 >/dev/null\nif [ \"$?\" != \"0\" ]; then\n  kate 2>&1 > /dev/null &\n  sleep 1\nfi\n");
	for ( int i = 1 ; i < argc ; i++ ) {
		const char* filename = argv[i];
		if ( (strlen(filename)+l) < lcmd )
			l+=sprintf(cmd+l,"kate \"%s\" 2>&1 >/dev/null\n",filename) ;
	}
	cmd[l]=0;
	printf("%s",cmd);
	system(cmd) ;

	return 0 ;
    
    // unused
    argc ;
    argv ;
}
