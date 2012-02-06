//
//  IO.cpp
//  ix4
//
//  Created by Robin Mills on 2012-02-05.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "IO.h"

static boolean_t gbVerbose = NO ;

void setVerbose(boolean_t verbose)
{
    gbVerbose = verbose;
}

boolean_t getVerbose() { return gbVerbose ; }

void warn(const char* msg)
{
    if ( gbVerbose ) {
        errors(msg);
    }
}

void errors(const char* msg)
{
    fputs(msg,stderr);
    fputc('\n',stderr);
}

void errorns(NSString* msg)
{
    errors([msg UTF8String]);
}

void report(const char* msg)
{
    puts(msg);
}

void reportns(NSString* msg)
{
    report([msg UTF8String]);
}

void warns(NSString* msg)
{
    warn([msg UTF8String]);
}
