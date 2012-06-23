#include <stdio.h>
#include <stdlib.h>
#ifdef _MSC_VER
#include <windows.h>
void getcwd(char* p,int max_p)
{
	GetCurrentDirectoryA(max_p,p);
}
#else
#include <unistd.h>
#endif

int main(int argc, char* argv[])
{
	char path[1000] ;
	getcwd(path,sizeof path) ;
	printf("%s\n",path) ;


	return 0 ;
}