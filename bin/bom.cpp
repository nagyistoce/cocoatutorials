// bom.cpp : Defines the entry point for the console application.
//

//#if defined (_WINDOWS)
//#include "stdafx.h"
//#endif

#include <stdio.h>
#include <string.h>

#define ERROR_OK         0
#define ERROR_PROCESSING 2
#define ERROR_SYNTAX     1

int Syntax()
{
	printf("usage: bom [+|-] input-file output-file\n") ;
	return ERROR_SYNTAX ;
}

int main(int argc,const char* argv[])
{
	if ( argc != 4 ) return Syntax() ;

	const char  bom   	= argv[1][0] ;
	const char* input 	= argv[2] 	;
	const char* output	= argv[3] 	;

	if ( bom != '+' && bom != '-' ) return Syntax() ;
	unsigned char BOM[2] ;
	if ( bom == '+' ) {
		BOM[0] = 0xff ; BOM[1] = 0xfe ;
	} else {
		BOM[0] = 0xfe ; BOM[1] = 0xff ;
	}

	FILE* finput = strcmp(input,"-") ? fopen(input,"rb") : stdin ;
	if ( !finput ) {
		printf("unable to open input file %s\n",input) ;
		return ERROR_PROCESSING ;
	}

	FILE* foutput = strcmp(output,"-") ? fopen(output,"wb") : stdout ;
	if ( !foutput ) {
		printf("unable to open input file %s\n",output) ;
		return ERROR_PROCESSING ;
	}

	fwrite(BOM,2,1,foutput) ;
	while ( !feof(finput) ) {
		char buffer[32*1024] ;
		size_t n = fread(buffer,1,sizeof buffer,finput) ;
		fwrite(buffer,1,n,foutput) ;
	}
	fclose(finput) ;
	fclose(foutput) ;

	return ERROR_OK ;
}
