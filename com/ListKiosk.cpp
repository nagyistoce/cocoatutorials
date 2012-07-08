#include <io.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[],char* env[])
{
    char* kiosk = getenv("KIOSK_HOST_DEBUG") ;
	printf("KIOSK_HOST_DEBUG = %s\n",kiosk?kiosk:"--undefined--") ;
    fflush(stdout)        ;
	return 0 ;
}