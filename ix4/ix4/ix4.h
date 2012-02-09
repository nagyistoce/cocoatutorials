//
//  ix4.h
//  ix4
//
//  Created by Robin Mills on 2012-02-05.
//  Copyright 2012 RMSC San Jose, CA, USA. All rights reserved.
//

#ifndef _ix4_h
#define _ix4_h

#include "PDFImages.h"
#include "IO.h"

#define    GETOPT_LONG   1
#define    DDCLI         2
#define    BUILD_WITH    2

#define    VERSION_MAJOR 0
#define    VERSION_MINOR 2

typedef enum error_e
{	OK = 0 
,	e_SYNTAX 
,   e_NOT_IMAGE
,	e_NO_IMAGES
,	e_NO_PDF_FILENAME
,   e_INVALIDSORT
,   e_UNRECOGNIZED_ARG
} error_e;

// horrible global variables
extern error_e     error;
extern const char* ix4;

#ifndef lengthof
#define lengthof(x) sizeof(x)/sizeof(x[0])
#endif

void display_usage(const char* program,bool bAll);
void display_version( void );

void unrecognizedArgument(const char* arg);
void notImage(NSString* filename);
void noImages(bool bCreate /*= true*/);
void invalidSortKey(const char* key);
void noPDFFileName(NSString* filename);
void syntaxError();

int  main_longopt(int argc,char** argv);
int  main_ddcli  (int,char**);

#endif
