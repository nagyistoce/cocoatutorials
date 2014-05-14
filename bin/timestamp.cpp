#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

const char* timestring(time_t clock)
{
	static char buffer[100];

	// 2012:04:07 16:12:44
	struct tm*  tmp = localtime(&clock);
	size_t      l   = strftime(buffer, sizeof(buffer)/sizeof(buffer[0]),"%Y:%m:%d %H:%M:%S",tmp);
	buffer[l]       = 0;
	return buffer;
}

int syntax()
{
    time_t c     ;
    time_t clock = time(&c);
	printf("syntax: timestamp <seconds>+  Eg: %lu %s\n",clock,timestring(clock)) ;
	return 1 ;
}

//559 rmills@rmills-imac:~/bin $ exiv2 -pa ~/R.jpg | grep -i date
//Exif.Image.DateTime                          Ascii      20  2009:08:03 08:58:57
//Exif.Photo.DateTimeOriginal                  Ascii      20  2009:08:03 08:58:57
//Exif.Photo.DateTimeDigitized                 Ascii      20  2009:08:03 08:58:57
//Exif.GPSInfo.GPSDateStamp                    Ascii      21  2009-08-03T15:58:57Z

int main(int argc,char* argv[])
{
	if ( argc < 2  ) return syntax() ;

	int 	 a = 1 ;
	while (  a < argc )
	{
		char* arg = argv[a++];
		unsigned int time = 999 ;
		if ( strstr(arg,":") || strstr(arg,"-") ) {
			int  Y,M,D,H,m,S ;
			char a,b,c,d,e,f ;
			printf("parsing %s\n",arg);
			sscanf(arg,"%d%c%d%c%d%c%d%c%d%c%d",&Y,&a,&M,&b,&D,&d,&H,&e,&m,&f,&S);
			printf("date = %02d-%02d-%02d  time = %02d:%02d:%02d\n",Y,M,D,H,m,S);
			
			struct tm T;
#if 0
		    int tm_sec;     /* seconds (0 - 60) */
            int tm_min;     /* minutes (0 - 59) */
            int tm_hour;    /* hours (0 - 23) */
            int tm_mday;    /* day of month (1 - 31) */
            int tm_mon;     /* month of year (0 - 11) */
            int tm_year;    /* year - 1900 */
            int tm_wday;    /* day of week (Sunday = 0) */
            int tm_yday;    /* day of year (0 - 365) */
            int tm_isdst;   /* is summer time in effect? */
            char *tm_zone;  /* abbreviation of timezone name */
            long tm_gmtoff; /* offset from UTC in seconds */
#endif
			memset(&T,sizeof(T),0);
			T.tm_min  = m  ;
			T.tm_hour = H  ;
			T.tm_sec  = S  ;
			T.tm_year = Y -1900 ;
			T.tm_mon  = M -1    ;
			T.tm_mday = D  ;
			time = mktime(&T);
		} else {
			time  = (long) atoi(arg) ;
		}
		time_t     clock = (clock_t) time ;
		printf("%u = %s\n",time,timestring(time)/*,ctime(&clock)*/)    ;
	}

	return 0 ;
}




