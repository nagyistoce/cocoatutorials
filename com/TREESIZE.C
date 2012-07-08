/*    (c)  Copyright  Robin Mills Camberley Surrey England 1992
 --------------------------------------------------------------
 * File    : treesize.c
 * Date    : 19.06.92
 * Author  : Robin Mills
 --------------------------------------------------------------
 Date      By  Modification

 --------------------------------------------------------------
 * Purpose :
   Find the total storage used in a directory

   This program recursively searches sub-directories


 * Usage   :
   treesize [directory] [-p]

   directory - name of the directory for which the size is required
               if no directory is given, the current directory is used

 * Returns :
   None.


 * Example :
   treesize c:


 * Description :
   This program recursively reads directories.


 * Restrictions and Side Effects :

   This program can only be compiled with Borland Turbo C++
   as it uses non-standard calls to the library.


   The depth of the directory tree is limited by
   the number of files which can be opened simultaneously.


 --------------------------------------------------------------
*/


/*------------------------------------------------------------*/
/* Include files                                              */
/*------------------------------------------------------------*/

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <time.h>
# include <dos.h>
# include <io.h>

/*------------------------------------------------------------*/
/* Turbo C++ / MSC specific code                              */
/*------------------------------------------------------------*/

#ifdef __TURBOC__
# include <alloc.h>
# include <dir.h>
#else
# include <malloc.h>
# define findfirst(name,block) _findfirst(name,block)
# define findnext(handle,block)       _findnext(handle,block)
# define FA_DIREC  _A_SUBDIR
# define FA_ARCH   _A_ARCH
# define ffblk     _finddata_t
# define ff_ftime  wr_time
# define ff_fdate  wr_date
# define ff_attrib attrib
# define ff_fsize  size
# define ff_name   name
#endif

/*------------------------------------------------------------*/
/* Macros - inline code expansions                            */
/*------------------------------------------------------------*/
# define BOOL  int
# define TRUE  1
# define FALSE 0

# define SYNTAX_ERROR(szText) { \
    printf("Syntax error %s\n",szText) ; \
    Syntax() ; \
    exit(1)  ; \
}

/*------------------------------------------------------------*/
/* Static variables                                           */
/*------------------------------------------------------------*/
static long   age     ; /* delete files older than this       */
static BOOL   dd      ; /* setting of the -dd flag            */
static int    today   ; /* number of days since 1.1.70        */
static unsigned long total = 0     ; /* bytes in directory    */
static BOOL   bPrint       = FALSE ; /* print filenames       */

/*------------------------------------------------------------*/
/* Error - report an error                                    */
/*------------------------------------------------------------*/
void Error
( char *szMessage
) {
    printf("Error - %s\n",szMessage) ;
}

/*------------------------------------------------------------*/
/* Syntax - report the syntax                                 */
/*------------------------------------------------------------*/
void Syntax
( void
) {
    printf("Syntax: treesize [directory] [-p]\n") ;
}

/*------------------------------------------------------------*/
/* SyntaxError - report a Syntax Error                        */
/*------------------------------------------------------------*/
void SyntaxError
( char *szMessage
) {
    printf("Syntax Error - %s\n",szMessage) ;
    Syntax() ;
}

/*------------------------------------------------------------*/
/* MakeFile - build a new file name                           */
/*------------------------------------------------------------*/
char *MakeFile
( char *szDirectory
, char *name
) {
    char *szName = malloc( strlen(szDirectory) + strlen(name) + 2) ;
    strcpy(szName,szDirectory) ;
    strcat(szName,"\\"       ) ;
    strcat(szName,name       ) ;
    return szName ;
}

/*------------------------------------------------------------*/
/* isNumber - is a string a number                            */
/*------------------------------------------------------------*/
BOOL isNumber
( char      *szString
, long int  *number
) {
    int   n = atoi(szString) ;
   *number  = n   ;
    return  n > 0 ;
}

/*------------------------------------------------------------*/
/* treesize - the recursive treesize routine                  */
/*------------------------------------------------------------*/
BOOL treesize
( char *szDirectory
, BOOL topmost
) {
    BOOL   result        = 1 ;
    struct ffblk block   ;
    long   handle        ;

    char  *szWild = MakeFile(szDirectory,"*.*") ; /* search all */
    int    attrib = FA_DIREC | FA_ARCH          ; /* files & dir*/

    handle = findfirst(szWild,&block) ;
    /*
    if ( topmost ) result = !done ;
    else           result = TRUE  ;
    */

    /* ------------------------------------------------------ */
    /* Cycle the directory - delete old files, search dirs    */
    /* ------------------------------------------------------ */
    while ( handle != -1 ) {
      char   attrib   = block.ff_attrib ;
      char  *name     = block.ff_name   ;
      long   size     = block.ff_fsize  ;
      char  *szName   = MakeFile(szDirectory,name) ;

      if ( attrib & FA_DIREC ) {
        if ( name[0] != '.' ) {
          treesize(szName,FALSE) ;
        }
      } else {
        if ( bPrint )
          printf("%12d %s\n",size,szName) ; // <--------------
        total += size ;
      }

      free(szName)            ;
      if ( findnext(handle,&block) != 0 ) {
        _findclose(handle) ;
        handle = -1       ;
      }
    }
    free(szWild)  ;

    return result ;
}

/*------------------------------------------------------------*/
/* main - the main function of this program                   */
/*------------------------------------------------------------*/
void main
( int   argc
, char *argv[]
) {
    char *dir ;
    if ( argc > 3 )
      SYNTAX_ERROR("Too many arguments") ;

    if ( stricmp(argv[argc-1],"-p")==0 ) {
      argc -- ;
      bPrint = TRUE ;
    }

    if ( argc < 2 )
      dir = strdup(".") ;
    else
      dir = argv[1]     ;

    if ( !treesize(dir,TRUE) )
      SYNTAX_ERROR("Illegal directory argument") ;

    printf("Total = %lu\n",total) ;

}
