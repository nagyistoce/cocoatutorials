/*
 *  alerts.h
 *
 *  Created by Robin Mills on 1/16/10.
 *  Copyright 2010 Robin Mills, San Jose, CA. All rights reserved.
 *
 */

#define lengthof(x) sizeof(x)/sizeof(x[0])
#define CALLBACK_KEY @"eventCallback"

void		alertSetTitle(NSString* title);
NSInteger	alert(NSString* msg) ;
NSInteger	ask(NSString* msg) ;

void arrayDump(NSArray*);
void dictDump(NSDictionary*);

extern char** __argv;
extern int    __argc;


// static void fsevents_callback(ConstFSEventStreamRef,void*,size_t,void* ,const FSEventStreamEventFlags* ,const FSEventStreamEventId* );

void   initializeEventStream(NSString* path,NSObject* client,SEL select);
// extern NSObject* clientObject ;
// extern SEL		 clientSelect ;


