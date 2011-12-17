#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	char path[1000] ;
	getcwd(path,sizeof path) ;
	printf("%s\n",path) ;


	return 0 ;
}