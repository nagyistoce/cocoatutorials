#line 1 "timex.c"

#line 1 "d:\\msdev\\include\\stdio.h"



















#line 21 "d:\\msdev\\include\\stdio.h"







#pragma pack(push,8)
#line 30 "d:\\msdev\\include\\stdio.h"













#line 44 "d:\\msdev\\include\\stdio.h"
#line 45 "d:\\msdev\\include\\stdio.h"









#line 55 "d:\\msdev\\include\\stdio.h"
#line 56 "d:\\msdev\\include\\stdio.h"














#line 71 "d:\\msdev\\include\\stdio.h"
#line 72 "d:\\msdev\\include\\stdio.h"
#line 73 "d:\\msdev\\include\\stdio.h"






#line 80 "d:\\msdev\\include\\stdio.h"



typedef unsigned int size_t;

#line 86 "d:\\msdev\\include\\stdio.h"




typedef unsigned short wchar_t;

#line 93 "d:\\msdev\\include\\stdio.h"



typedef wchar_t wint_t;
typedef wchar_t wctype_t;

#line 100 "d:\\msdev\\include\\stdio.h"
#line 101 "d:\\msdev\\include\\stdio.h"









typedef char *  va_list;
#line 112 "d:\\msdev\\include\\stdio.h"

#line 114 "d:\\msdev\\include\\stdio.h"






#line 121 "d:\\msdev\\include\\stdio.h"

#line 123 "d:\\msdev\\include\\stdio.h"






























#line 154 "d:\\msdev\\include\\stdio.h"





struct _iobuf {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
        };
typedef struct _iobuf FILE;

#line 172 "d:\\msdev\\include\\stdio.h"











#line 184 "d:\\msdev\\include\\stdio.h"










#line 195 "d:\\msdev\\include\\stdio.h"




























#line 224 "d:\\msdev\\include\\stdio.h"
#line 225 "d:\\msdev\\include\\stdio.h"





















#line 247 "d:\\msdev\\include\\stdio.h"

 extern FILE _iob[];

#line 251 "d:\\msdev\\include\\stdio.h"

#line 253 "d:\\msdev\\include\\stdio.h"

#line 255 "d:\\msdev\\include\\stdio.h"










#line 266 "d:\\msdev\\include\\stdio.h"


typedef __int64 fpos_t;





#line 275 "d:\\msdev\\include\\stdio.h"

#line 277 "d:\\msdev\\include\\stdio.h"


#line 280 "d:\\msdev\\include\\stdio.h"




























 int __cdecl _filbuf(FILE *);
 int __cdecl _flsbuf(int, FILE *);




 FILE * __cdecl _fsopen(const char *, const char *, int);
#line 316 "d:\\msdev\\include\\stdio.h"

 void __cdecl clearerr(FILE *);
 int __cdecl fclose(FILE *);
 int __cdecl _fcloseall(void);




 FILE * __cdecl _fdopen(int, const char *);
#line 326 "d:\\msdev\\include\\stdio.h"

 int __cdecl feof(FILE *);
 int __cdecl ferror(FILE *);
 int __cdecl fflush(FILE *);
 int __cdecl fgetc(FILE *);
 int __cdecl _fgetchar(void);
 int __cdecl fgetpos(FILE *, fpos_t *);
 char * __cdecl fgets(char *, int, FILE *);




 int __cdecl _fileno(FILE *);
#line 340 "d:\\msdev\\include\\stdio.h"

 int __cdecl _flushall(void);
 FILE * __cdecl fopen(const char *, const char *);
 int __cdecl fprintf(FILE *, const char *, ...);
 int __cdecl fputc(int, FILE *);
 int __cdecl _fputchar(int);
 int __cdecl fputs(const char *, FILE *);
 size_t __cdecl fread(void *, size_t, size_t, FILE *);
 FILE * __cdecl freopen(const char *, const char *, FILE *);
 int __cdecl fscanf(FILE *, const char *, ...);
 int __cdecl fsetpos(FILE *, const fpos_t *);
 int __cdecl fseek(FILE *, long, int);
 long __cdecl ftell(FILE *);
 size_t __cdecl fwrite(const void *, size_t, size_t, FILE *);
 int __cdecl getc(FILE *);
 int __cdecl getchar(void);
 char * __cdecl gets(char *);
 int __cdecl _getw(FILE *);
 void __cdecl perror(const char *);
 int __cdecl _pclose(FILE *);
 FILE * __cdecl _popen(const char *, const char *);
 int __cdecl printf(const char *, ...);
 int __cdecl putc(int, FILE *);
 int __cdecl putchar(int);
 int __cdecl puts(const char *);
 int __cdecl _putw(int, FILE *);
 int __cdecl remove(const char *);
 int __cdecl rename(const char *, const char *);
 void __cdecl rewind(FILE *);
 int __cdecl _rmtmp(void);
 int __cdecl scanf(const char *, ...);
 void __cdecl setbuf(FILE *, char *);
 int __cdecl _setmaxstdio(int);
 int __cdecl setvbuf(FILE *, char *, int, size_t);
 int __cdecl _snprintf(char *, size_t, const char *, ...);
 int __cdecl sprintf(char *, const char *, ...);
 int __cdecl sscanf(const char *, const char *, ...);
 char * __cdecl _tempnam(const char *, const char *);
 FILE * __cdecl tmpfile(void);
 char * __cdecl tmpnam(char *);
 int __cdecl ungetc(int, FILE *);
 int __cdecl _unlink(const char *);
 int __cdecl vfprintf(FILE *, const char *, va_list);
 int __cdecl vprintf(const char *, va_list);
 int __cdecl _vsnprintf(char *, size_t, const char *, va_list);
 int __cdecl vsprintf(char *, const char *, va_list);








#line 395 "d:\\msdev\\include\\stdio.h"




 FILE * __cdecl _wfsopen(const wchar_t *, const wchar_t *, int);
#line 401 "d:\\msdev\\include\\stdio.h"

 wint_t __cdecl fgetwc(FILE *);
 wint_t __cdecl _fgetwchar(void);
 wint_t __cdecl fputwc(wint_t, FILE *);
 wint_t __cdecl _fputwchar(wint_t);
 wint_t __cdecl getwc(FILE *);
 wint_t __cdecl getwchar(void);
 wint_t __cdecl putwc(wint_t, FILE *);
 wint_t __cdecl putwchar(wint_t);
 wint_t __cdecl ungetwc(wint_t, FILE *);

 wchar_t * __cdecl fgetws(wchar_t *, int, FILE *);
 int __cdecl fputws(const wchar_t *, FILE *);
 wchar_t * __cdecl _getws(wchar_t *);
 int __cdecl _putws(const wchar_t *);

 int __cdecl fwprintf(FILE *, const wchar_t *, ...);
 int __cdecl wprintf(const wchar_t *, ...);
 int __cdecl _snwprintf(wchar_t *, size_t, const wchar_t *, ...);
 int __cdecl swprintf(wchar_t *, const wchar_t *, ...);
 int __cdecl vfwprintf(FILE *, const wchar_t *, va_list);
 int __cdecl vwprintf(const wchar_t *, va_list);
 int __cdecl _vsnwprintf(wchar_t *, size_t, const wchar_t *, va_list);
 int __cdecl vswprintf(wchar_t *, const wchar_t *, va_list);
 int __cdecl fwscanf(FILE *, const wchar_t *, ...);
 int __cdecl swscanf(const wchar_t *, const wchar_t *, ...);
 int __cdecl wscanf(const wchar_t *, ...);






 FILE * __cdecl _wfdopen(int, const wchar_t *);
 FILE * __cdecl _wfopen(const wchar_t *, const wchar_t *);
 FILE * __cdecl _wfreopen(const wchar_t *, const wchar_t *, FILE *);
 void __cdecl _wperror(const wchar_t *);
 FILE * __cdecl _wpopen(const wchar_t *, const wchar_t *);
 int __cdecl _wremove(const wchar_t *);
 wchar_t * __cdecl _wtempnam(const wchar_t *, const wchar_t *);
 wchar_t * __cdecl _wtmpnam(wchar_t *);



#line 446 "d:\\msdev\\include\\stdio.h"
#line 447 "d:\\msdev\\include\\stdio.h"


#line 450 "d:\\msdev\\include\\stdio.h"
















































 int __cdecl fcloseall(void);
 FILE * __cdecl fdopen(int, const char *);
 int __cdecl fgetchar(void);
 int __cdecl fileno(FILE *);
 int __cdecl flushall(void);
 int __cdecl fputchar(int);
 int __cdecl getw(FILE *);
 int __cdecl putw(int, FILE *);
 int __cdecl rmtmp(void);
 char * __cdecl tempnam(const char *, const char *);
 int __cdecl unlink(const char *);

#line 511 "d:\\msdev\\include\\stdio.h"

#line 513 "d:\\msdev\\include\\stdio.h"







#pragma pack(pop)
#line 522 "d:\\msdev\\include\\stdio.h"

#line 524 "d:\\msdev\\include\\stdio.h"
#line 3 "timex.c"
#line 1 "d:\\msdev\\include\\malloc.h"



















#line 21 "d:\\msdev\\include\\malloc.h"







#pragma pack(push,8)
#line 30 "d:\\msdev\\include\\malloc.h"

















































#line 80 "d:\\msdev\\include\\malloc.h"
























typedef struct _heapinfo {
        int * _pentry;
        size_t _size;
        int _useflag;
        } _HEAPINFO;

#line 111 "d:\\msdev\\include\\malloc.h"










#line 122 "d:\\msdev\\include\\malloc.h"

extern unsigned int _amblksiz;

#line 126 "d:\\msdev\\include\\malloc.h"

#line 128 "d:\\msdev\\include\\malloc.h"




 void * __cdecl calloc(size_t, size_t);
 void   __cdecl free(void *);
 void * __cdecl malloc(size_t);
 void * __cdecl realloc(void *, size_t);


#line 139 "d:\\msdev\\include\\malloc.h"



void * __cdecl _alloca(size_t);
 void * __cdecl _expand(void *, size_t);

 int __cdecl _heapadd(void *, size_t);
 int __cdecl _heapchk(void);
 int __cdecl _heapmin(void);
 int __cdecl _heapset(unsigned int);
 int __cdecl _heapwalk(_HEAPINFO *);
 size_t __cdecl _heapused(size_t *, size_t *);
#line 152 "d:\\msdev\\include\\malloc.h"
 size_t __cdecl _msize(void *);




#line 158 "d:\\msdev\\include\\malloc.h"



#line 162 "d:\\msdev\\include\\malloc.h"

#line 164 "d:\\msdev\\include\\malloc.h"



























#pragma pack(pop)
#line 193 "d:\\msdev\\include\\malloc.h"

#line 195 "d:\\msdev\\include\\malloc.h"
#line 4 "timex.c"
#line 1 "d:\\msdev\\include\\time.h"




















#line 22 "d:\\msdev\\include\\time.h"







#pragma pack(push,8)
#line 31 "d:\\msdev\\include\\time.h"

















































#line 81 "d:\\msdev\\include\\time.h"






#line 88 "d:\\msdev\\include\\time.h"




typedef long time_t;        

#line 95 "d:\\msdev\\include\\time.h"


typedef long clock_t;

#line 100 "d:\\msdev\\include\\time.h"



















struct tm {
        int tm_sec;     
        int tm_min;     
        int tm_hour;    
        int tm_mday;    
        int tm_mon;     
        int tm_year;    
        int tm_wday;    
        int tm_yday;    
        int tm_isdst;   
        };

#line 132 "d:\\msdev\\include\\time.h"


































































#line 199 "d:\\msdev\\include\\time.h"


 extern int _daylight;


 extern long _timezone;


 extern char * _tzname[2];

#line 210 "d:\\msdev\\include\\time.h"

#line 212 "d:\\msdev\\include\\time.h"




 char * __cdecl asctime(const struct tm *);
 char * __cdecl ctime(const time_t *);
 clock_t __cdecl clock(void);
 double __cdecl difftime(time_t, time_t);
 struct tm * __cdecl gmtime(const time_t *);
 struct tm * __cdecl localtime(const time_t *);
 time_t __cdecl mktime(struct tm *);
 size_t __cdecl strftime(char *, size_t, const char *,
        const struct tm *);
 char * __cdecl _strdate(char *);
 char * __cdecl _strtime(char *);
 time_t __cdecl time(time_t *);




 void __cdecl _tzset(void);
#line 234 "d:\\msdev\\include\\time.h"



unsigned __cdecl _getsystime(struct tm *);
unsigned __cdecl _setsystime(struct tm *, unsigned);












 
 wchar_t * __cdecl _wasctime(const struct tm *);
 wchar_t * __cdecl _wctime(const time_t *);
 size_t __cdecl wcsftime(wchar_t *, size_t, const wchar_t *,
        const struct tm *);
 wchar_t * __cdecl _wstrdate(wchar_t *);
 wchar_t * __cdecl _wstrtime(wchar_t *);


#line 261 "d:\\msdev\\include\\time.h"
#line 262 "d:\\msdev\\include\\time.h"






























#line 293 "d:\\msdev\\include\\time.h"

 extern int daylight;
 extern long timezone;
 extern char * tzname[2];

#line 299 "d:\\msdev\\include\\time.h"

 void __cdecl tzset(void);

#line 303 "d:\\msdev\\include\\time.h"

#line 305 "d:\\msdev\\include\\time.h"








#pragma pack(pop)
#line 315 "d:\\msdev\\include\\time.h"

#line 317 "d:\\msdev\\include\\time.h"
#line 5 "timex.c"
#line 1 "d:\\msdev\\include\\string.h"



















#line 21 "d:\\msdev\\include\\string.h"


















































#line 72 "d:\\msdev\\include\\string.h"













#line 86 "d:\\msdev\\include\\string.h"




#line 91 "d:\\msdev\\include\\string.h"
























        void *  __cdecl memcpy(void *, const void *, size_t);
        int     __cdecl memcmp(const void *, const void *, size_t);
        void *  __cdecl memset(void *, int, size_t);
        char *  __cdecl _strset(char *, int);
        char *  __cdecl strcpy(char *, const char *);
        char *  __cdecl strcat(char *, const char *);
        int     __cdecl strcmp(const char *, const char *);
        size_t  __cdecl strlen(const char *);
#line 124 "d:\\msdev\\include\\string.h"
 void *  __cdecl _memccpy(void *, const void *, int, unsigned int);
 void *  __cdecl memchr(const void *, int, size_t);
 int     __cdecl _memicmp(const void *, const void *, unsigned int);





 void *  __cdecl memmove(void *, const void *, size_t);
#line 134 "d:\\msdev\\include\\string.h"


 char *  __cdecl strchr(const char *, int);
 int     __cdecl _strcmpi(const char *, const char *);
 int     __cdecl _stricmp(const char *, const char *);
 int     __cdecl strcoll(const char *, const char *);
 int     __cdecl _stricoll(const char *, const char *);
 int     __cdecl _strncoll(const char *, const char *, size_t);
 int     __cdecl _strnicoll(const char *, const char *, size_t);
 size_t  __cdecl strcspn(const char *, const char *);
 char *  __cdecl _strdup(const char *);
 char *  __cdecl _strerror(const char *);
 char *  __cdecl strerror(int);
 char *  __cdecl _strlwr(char *);
 char *  __cdecl strncat(char *, const char *, size_t);
 int     __cdecl strncmp(const char *, const char *, size_t);
 int     __cdecl _strnicmp(const char *, const char *, size_t);
 char *  __cdecl strncpy(char *, const char *, size_t);
 char *  __cdecl _strnset(char *, int, size_t);
 char *  __cdecl strpbrk(const char *, const char *);
 char *  __cdecl strrchr(const char *, int);
 char *  __cdecl _strrev(char *);
 size_t  __cdecl strspn(const char *, const char *);
 char *  __cdecl strstr(const char *, const char *);
 char *  __cdecl strtok(char *, const char *);
 char *  __cdecl _strupr(char *);
 size_t  __cdecl strxfrm (char *, const char *, size_t);









#line 171 "d:\\msdev\\include\\string.h"





















 void * __cdecl memccpy(void *, const void *, int, unsigned int);
 int __cdecl memicmp(const void *, const void *, unsigned int);
 int __cdecl strcmpi(const char *, const char *);
 int __cdecl stricmp(const char *, const char *);
 char * __cdecl strdup(const char *);
 char * __cdecl strlwr(char *);
 int __cdecl strnicmp(const char *, const char *, size_t);
 char * __cdecl strnset(char *, int, size_t);
 char * __cdecl strrev(char *);
        char * __cdecl strset(char *, int);
 char * __cdecl strupr(char *);

#line 205 "d:\\msdev\\include\\string.h"

#line 207 "d:\\msdev\\include\\string.h"







 wchar_t * __cdecl wcscat(wchar_t *, const wchar_t *);
 wchar_t * __cdecl wcschr(const wchar_t *, wchar_t);
 int __cdecl wcscmp(const wchar_t *, const wchar_t *);
 wchar_t * __cdecl wcscpy(wchar_t *, const wchar_t *);
 size_t __cdecl wcscspn(const wchar_t *, const wchar_t *);
 size_t __cdecl wcslen(const wchar_t *);
 wchar_t * __cdecl wcsncat(wchar_t *, const wchar_t *, size_t);
 int __cdecl wcsncmp(const wchar_t *, const wchar_t *, size_t);
 wchar_t * __cdecl wcsncpy(wchar_t *, const wchar_t *, size_t);
 wchar_t * __cdecl wcspbrk(const wchar_t *, const wchar_t *);
 wchar_t * __cdecl wcsrchr(const wchar_t *, wchar_t);
 size_t __cdecl wcsspn(const wchar_t *, const wchar_t *);
 wchar_t * __cdecl wcsstr(const wchar_t *, const wchar_t *);
 wchar_t * __cdecl wcstok(wchar_t *, const wchar_t *);

 wchar_t * __cdecl _wcsdup(const wchar_t *);
 int __cdecl _wcsicmp(const wchar_t *, const wchar_t *);
 int __cdecl _wcsnicmp(const wchar_t *, const wchar_t *, size_t);
 wchar_t * __cdecl _wcsnset(wchar_t *, wchar_t, size_t);
 wchar_t * __cdecl _wcsrev(wchar_t *);
 wchar_t * __cdecl _wcsset(wchar_t *, wchar_t);

 wchar_t * __cdecl _wcslwr(wchar_t *);
 wchar_t * __cdecl _wcsupr(wchar_t *);
 size_t __cdecl wcsxfrm(wchar_t *, const wchar_t *, size_t);
 int __cdecl wcscoll(const wchar_t *, const wchar_t *);
 int __cdecl _wcsicoll(const wchar_t *, const wchar_t *);
 int __cdecl _wcsncoll(const wchar_t *, const wchar_t *, size_t);
 int __cdecl _wcsnicoll(const wchar_t *, const wchar_t *, size_t);






















 wchar_t * __cdecl wcsdup(const wchar_t *);
 int __cdecl wcsicmp(const wchar_t *, const wchar_t *);
 int __cdecl wcsnicmp(const wchar_t *, const wchar_t *, size_t);
 wchar_t * __cdecl wcsnset(wchar_t *, wchar_t, size_t);
 wchar_t * __cdecl wcsrev(wchar_t *);
 wchar_t * __cdecl wcsset(wchar_t *, wchar_t);
 wchar_t * __cdecl wcslwr(wchar_t *);
 wchar_t * __cdecl wcsupr(wchar_t *);
 int __cdecl wcsicoll(const wchar_t *, const wchar_t *);

#line 276 "d:\\msdev\\include\\string.h"

#line 278 "d:\\msdev\\include\\string.h"


#line 281 "d:\\msdev\\include\\string.h"

#line 283 "d:\\msdev\\include\\string.h"





#line 289 "d:\\msdev\\include\\string.h"
#line 6 "timex.c"
#line 1 "d:\\msdev\\include\\stdlib.h"





















#line 23 "d:\\msdev\\include\\stdlib.h"







#pragma pack(push,8)
#line 32 "d:\\msdev\\include\\stdlib.h"

















































#line 82 "d:\\msdev\\include\\stdlib.h"
































typedef int (__cdecl * _onexit_t)(void);



#line 119 "d:\\msdev\\include\\stdlib.h"

#line 121 "d:\\msdev\\include\\stdlib.h"






typedef struct _div_t {
        int quot;
        int rem;
} div_t;

typedef struct _ldiv_t {
        long quot;
        long rem;
} ldiv_t;


#line 139 "d:\\msdev\\include\\stdlib.h"





























#line 169 "d:\\msdev\\include\\stdlib.h"

 extern int __mb_cur_max;
#line 172 "d:\\msdev\\include\\stdlib.h"

#line 174 "d:\\msdev\\include\\stdlib.h"

#line 176 "d:\\msdev\\include\\stdlib.h"





















#line 198 "d:\\msdev\\include\\stdlib.h"
















#line 215 "d:\\msdev\\include\\stdlib.h"
 extern int errno;           
 extern unsigned long _doserrno;     
#line 218 "d:\\msdev\\include\\stdlib.h"



#line 222 "d:\\msdev\\include\\stdlib.h"

































































































 extern char * _sys_errlist[];   
 extern int _sys_nerr;       







































#line 361 "d:\\msdev\\include\\stdlib.h"
 extern int __argc;          
 extern char ** __argv;      

 extern wchar_t ** __wargv;  
#line 366 "d:\\msdev\\include\\stdlib.h"

 extern char **    _environ;     

 extern wchar_t ** _wenviron;    
#line 371 "d:\\msdev\\include\\stdlib.h"

 extern int _fmode;          
 extern int _fileinfo;       

 extern char * _pgmptr;      

 extern wchar_t * _wpgmptr;  
#line 379 "d:\\msdev\\include\\stdlib.h"



 extern unsigned int _osver;
 extern unsigned int _winver;
 extern unsigned int _winmajor;
 extern unsigned int _winminor;
#line 387 "d:\\msdev\\include\\stdlib.h"

#line 389 "d:\\msdev\\include\\stdlib.h"



 void   __cdecl abort(void);



#line 397 "d:\\msdev\\include\\stdlib.h"
        int    __cdecl abs(int);
#line 399 "d:\\msdev\\include\\stdlib.h"
        int    __cdecl atexit(void (__cdecl *)(void));
 double __cdecl atof(const char *);
 int    __cdecl atoi(const char *);
 long   __cdecl atol(const char *);



 void * __cdecl bsearch(const void *, const void *, size_t, size_t,
        int (__cdecl *)(const void *, const void *));
 void * __cdecl calloc(size_t, size_t);
 div_t  __cdecl div(int, int);
 void   __cdecl exit(int);
 void   __cdecl free(void *);
 char * __cdecl getenv(const char *);
 char * __cdecl _itoa(int, char *, int);


#line 417 "d:\\msdev\\include\\stdlib.h"
        long __cdecl labs(long);
#line 419 "d:\\msdev\\include\\stdlib.h"
 ldiv_t __cdecl ldiv(long, long);
 char * __cdecl _ltoa(long, char *, int);
 void * __cdecl malloc(size_t);
 int    __cdecl mblen(const char *, size_t);
 size_t __cdecl _mbstrlen(const char *s);
 int    __cdecl mbtowc(wchar_t *, const char *, size_t);
 size_t __cdecl mbstowcs(wchar_t *, const char *, size_t);
 void   __cdecl qsort(void *, size_t, size_t, int (__cdecl *)
        (const void *, const void *));
 int    __cdecl rand(void);
 void * __cdecl realloc(void *, size_t);
 int    __cdecl _set_error_mode(int);
 void   __cdecl srand(unsigned int);
 double __cdecl strtod(const char *, char **);
 long   __cdecl strtol(const char *, char **, int);



 unsigned long __cdecl strtoul(const char *, char **, int);

 int    __cdecl system(const char *);
#line 441 "d:\\msdev\\include\\stdlib.h"
 char * __cdecl _ultoa(unsigned long, char *, int);
 int    __cdecl wctomb(char *, wchar_t);
 size_t __cdecl wcstombs(char *, const wchar_t *, size_t);






 wchar_t * __cdecl _itow (int, wchar_t *, int);
 wchar_t * __cdecl _ltow (long, wchar_t *, int);
 wchar_t * __cdecl _ultow (unsigned long, wchar_t *, int);
 double __cdecl wcstod(const wchar_t *, wchar_t **);
 long   __cdecl wcstol(const wchar_t *, wchar_t **, int);
 unsigned long __cdecl wcstoul(const wchar_t *, wchar_t **, int);
 wchar_t * __cdecl _wgetenv(const wchar_t *);
 int    __cdecl _wsystem(const wchar_t *);
 int __cdecl _wtoi(const wchar_t *);
 long __cdecl _wtol(const wchar_t *);


#line 463 "d:\\msdev\\include\\stdlib.h"
#line 464 "d:\\msdev\\include\\stdlib.h"



 char * __cdecl _ecvt(double, int, int *, int *);
 void   __cdecl _exit(int);
 char * __cdecl _fcvt(double, int, int *, int *);
 char * __cdecl _fullpath(char *, const char *, size_t);
 char * __cdecl _gcvt(double, int, char *);
        unsigned long __cdecl _lrotl(unsigned long, int);
        unsigned long __cdecl _lrotr(unsigned long, int);

 void   __cdecl _makepath(char *, const char *, const char *, const char *,
        const char *);
#line 478 "d:\\msdev\\include\\stdlib.h"
        _onexit_t __cdecl _onexit(_onexit_t);
 void   __cdecl perror(const char *);
 int    __cdecl _putenv(const char *);
        unsigned int __cdecl _rotl(unsigned int, int);
        unsigned int __cdecl _rotr(unsigned int, int);
 void   __cdecl _searchenv(const char *, const char *, char *);

 void   __cdecl _splitpath(const char *, char *, char *, char *, char *);
#line 487 "d:\\msdev\\include\\stdlib.h"
 void   __cdecl _swab(char *, char *, int);






 wchar_t * __cdecl _wfullpath(wchar_t *, const wchar_t *, size_t);
 void   __cdecl _wmakepath(wchar_t *, const wchar_t *, const wchar_t *, const wchar_t *,
        const wchar_t *);
 void   __cdecl _wperror(const wchar_t *);
 int    __cdecl _wputenv(const wchar_t *);
 void   __cdecl _wsearchenv(const wchar_t *, const wchar_t *, wchar_t *);
 void   __cdecl _wsplitpath(const wchar_t *, wchar_t *, wchar_t *, wchar_t *, wchar_t *);


#line 504 "d:\\msdev\\include\\stdlib.h"
#line 505 "d:\\msdev\\include\\stdlib.h"




 void _seterrormode(int);
 void _beep(unsigned, unsigned);
 void _sleep(unsigned long);
#line 513 "d:\\msdev\\include\\stdlib.h"


#line 516 "d:\\msdev\\include\\stdlib.h"


 int __cdecl tolower(int);
#line 520 "d:\\msdev\\include\\stdlib.h"


 int __cdecl toupper(int);
#line 524 "d:\\msdev\\include\\stdlib.h"







































#line 564 "d:\\msdev\\include\\stdlib.h"





 char * __cdecl ecvt(double, int, int *, int *);
 char * __cdecl fcvt(double, int, int *, int *);
 char * __cdecl gcvt(double, int, char *);
 char * __cdecl itoa(int, char *, int);
 char * __cdecl ltoa(long, char *, int);
        _onexit_t __cdecl onexit(_onexit_t);
 int    __cdecl putenv(const char *);
 void   __cdecl swab(char *, char *, int);
 char * __cdecl ultoa(unsigned long, char *, int);

#line 580 "d:\\msdev\\include\\stdlib.h"

#line 582 "d:\\msdev\\include\\stdlib.h"







#pragma pack(pop)
#line 591 "d:\\msdev\\include\\stdlib.h"

#line 593 "d:\\msdev\\include\\stdlib.h"
#line 7 "timex.c"

char* now()
{
    long  tloc = 1       ;
    time(&tloc)          ;
    return strdup((ctime(&tloc))) ;
}

int main(int argc,char* argv[])
{
    int   i              ;
    int   l       = 0    ;
    char* line    = ((void *)0) ;
    int   result  = 0    ;
    char* oldtime = ((void *)0) ;
    char* newtime = ((void *)0) ;


    for ( i = 1 ; i < argc ; i ++ ) l += strlen(argv[i]) + 1 ;

    if ( l ) line = malloc(l+1) ;

    if ( line ) {
      line[0] = 0 ;

      for ( i = 1 ; i < argc ; i++ ) {
        strcat(line,argv[i]) ;
        strcat(line," "    ) ;
      }
      oldtime = now()        ;
      result  = system(line) ;
      newtime = now()        ;

      printf("------------------------\n") ;
      printf("%s%s",oldtime,newtime) ;
      printf("------------------------\n") ;
    }

    if ( oldtime ) free(oldtime) ;
    if ( newtime ) free(newtime) ;
    return result ;

}
