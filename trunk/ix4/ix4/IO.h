//
//  IO.h
//  ix4
//
//  Created by Robin Mills on 2012-02-05.
//  Copyright 2012 RMSC San Jose, CA, USA. All rights reserved.
//

#ifndef ix4_IO_h
#define ix4_IO_h

void setVerbose(boolean_t verbose);
boolean_t getVerbose();

// report => stdout error => stderr warn => stderr IF verbose
void warn    (const char* msg);
void errors  (const char* msg);
void report  (const char* msg);

void warns   (NSString* msg);
void errorns (NSString* msg);
void reportns(NSString* msg);

#endif
