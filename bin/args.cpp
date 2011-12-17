#include <stdio.h>
int main(int argc, char* argv[])
{
	int i = 0 ;
	while ( i++ < argc ) printf("%-2d: %s\n",i,argv[i-1]) ;
    return 0 ;
}
