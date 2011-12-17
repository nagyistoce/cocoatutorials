#include <stdio.h>

int main(int argc,char** argv,char **env)
{
	int i = 0 ;
	while ( *argv ) printf("%2d: %s\n",i++,*argv++) ;
	printf("\n") ;
	while ( *env  ) printf("%s\n",*env++) ;
}