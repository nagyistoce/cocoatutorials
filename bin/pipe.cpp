#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,const char* argv[])
{
    int    buff_l = 8*1024;
    char*  buff[buff_l];
    int    buffer_l = 8*1024*1024;
    char*  buffer = (char*) malloc(buffer_l);

    size_t L=0;
    if ( buffer ) {
        size_t l;
        while ( (l=fread(buff,1,buff_l,stdin)) >0 ) {
        	// printf("read %d\n",l);
            if ( L+l < buffer_l )
                memcpy(buffer+L,buff,l);
	        L+=l;
        }
    }
    // printf("L = %u buffer = %#lx\n",(unsigned int)L,(long unsigned int)buffer);
    
    FILE* out = argc < 2 ? stdout : fopen(argv[1],"w");
	if ( out ) {
    	if ( L < buffer_l ) fwrite(buffer,1,L,out);
    	fclose(out);
    }
    
    return (L && L < buffer_l)?0:1;
 }

