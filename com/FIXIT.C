/* (c) Copyright 1995 Robin Mills Camberley England GU15 1ED
----------------------------------------------------------------- 
* File   : fixit.c
* Author : Robin Mills
* Date   : 26.12.95
-----------------------------------------------------------------

* Purpose:
  None.

* Useage :
  fixit <filename>

* Example:
  None.

* Description:
  None.

* Restrictions:
  None.

-----------------------------------------------------------------
Modified By Reason

-----------------------------------------------------------------
*/

# include <stdio.h>

static int errors = 0 ;

/* ------------------------------------------------------------- */
/* ErrorMessage - print an error message                         */
/* ------------------------------------------------------------- */
void ErrorMessage(char* format,char *data) 
{
    fprintf(stderr,format,data) ;
    fprintf(stderr,"\n")        ;
    errors ++ ;
}

/* ------------------------------------------------------------- */
/* fexists - test a file is readable                             */
/* ------------------------------------------------------------- */
int fexists(char* filename) 
{
    int   result = 0 ;
    FILE* file   = fopen(filename,"r") ;
    if ( file ) {
      result = 1 ;
      fclose(file) ;
    } 
    return result ;
}

/* ------------------------------------------------------------- */
/* copy - copy a file                                            */
/* ------------------------------------------------------------- */
int copy(char* filename) 
{
    int   result = 0 ;
    FILE* file   = fopen(filename,"r") ;

    if ( file ) {
      int    c                  ;
      char*  t   = tmpnam(NULL) ;
      FILE*  out = fopen(t,"w") ;
      int    col = 1            ;

      result     = 1            ;
     

     /*
      * copy filename to temp file
      */ 
      do {
        c = getc(file) ;
                                    
	if ( isprint(c) ) {
          putc(c,out) ;
          col++       ;
        } else switch (c) {
          case '\t' :
               putc(' ',out) ;
               while (++col % 8)
                 putc(' ',out) ;
          break;
                                            
          case '\n' :
          case '\r' :
          case '\f' :
               putc('\n',out) ;
               col = 0 ;
          break ;
        }
      } while ( c != EOF ) ;
                                         
      if ( file ) fclose(file) ;
      if ( out  ) fclose(out ) ;
                                       
     /*
      * copy temp file to filename
      */ 
      file = fopen(t,"r") ;
      out  = fopen(filename,"w") ;
                                                
      if ( file && out ) {
        while ( ( c = getc(file) ) != EOF )
          putc(c,out) ;
      }
                                                   
      if ( !out ) ErrorMessage("Unable to rewrite file %s",filename) ;        
      if ( !file) ErrorMessage("Unable to open temporary file %s",t) ;
                                                    
      if ( file ) fclose(file) ;
      if ( out  ) fclose(out)  ;
      unlink(t) ;
    } 

    return result ;
}

/* ------------------------------------------------------------- */
/* main - the main function of this program                      */
/* ------------------------------------------------------------- */
int main(int argc, char* argv[])
{
   
    if ( argc != 2 ) {
      ErrorMessage("Syntax: fixit <filename>",NULL) ;
    } else if ( !copy(argv[1]) ) {
      ErrorMessage("Cant open file %s",argv[1]) ;
    } 
    return errors ;
}
      
