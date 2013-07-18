#include <stdio.h>
#include <stdlib.h>

size_t undup(int a[],size_t l)
{
	size_t w = 0 ; // write position
	size_t r = 0 ; // read  position
	while ( r < l ) {
		if ( a[w] != a[r++]  )
			a[++w] = a[r-1] ;
	}
	return w ;
}

int main(int argc,char* argv[])
{
	int A[] = {1,1,2,2,2,3,4,5,6,7,8,8,0};
	int a[100];
	size_t l;
	size_t i;
	for ( i = 0 ; A[i] ;i++ ) 
		a[i] = A[i] ;
	l = i+1 ;
	
	l = undup(a,l) ;

	for ( i = 0 ; i < l ; i++ ) 
		printf("%d ",a[i]);
	if ( l ) printf("\n") ;

	return 0 ;
}
