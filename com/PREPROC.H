/*    (c)  Copyright  Robin Mills Camberley Surrey England 1992
 --------------------------------------------------------------
 * File    : preproc.h
 * Date    : 21.06.92
 * Author  : Robin Mills
 --------------------------------------------------------------
 Date      By  Modification

 --------------------------------------------------------------
 * Purpose :   
   Provide a command line preprocessor

   The macro in this file should be calleded
   at the beginning of every program to process -help

 * Usage   :   
   PREPROCESSOR(szName,szVersion,szSyntax)

 * Returns :   
   If -help is found Exit(0)
   If -help not found, nothing

 * Example :   
   None.

 * Description : 
   This program preprocesses the command line
   for the -help argument.

 * Restrictions and Side Effects : 
   None.

 --------------------------------------------------------------
*/
#ifdef MB_OK
# define PREPROCESSOR(hWnd,cmdline,szProgram,szVersion,szSyntax) \
  MessageBox(hWnd,cmdline,szProgram,MB_OK) ; \
  PostQuitMessage(0) ;
#else
# define PREPROCESSOR(argc,argv,szProgram,szVersion,szSyntax) { \
    int i ;                                 \
    for ( i = 0 ; i < argc ; i++ )          \
      if ( !stricmp(argv[i],"-help") ) {    \
        printf("Program: %s\n",szProgram) ; \
        printf("Version: %s\n",szVersion) ; \
        printf("Syntax : %s " ,szProgram) ; \
        printf("%s\n"         ,szSyntax ) ; \
        exit(0) ;                           \
      }                                     \
  }
#endif

