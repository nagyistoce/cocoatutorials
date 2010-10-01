/*
 *  alerts.cpp
 *  CurConv
 *
 *  Created by Robin Mills on 1/16/10.
 *  Copyright 2010 Robin Mills, San Jose, CA. All rights reserved.
 *
 */

#import <Cocoa/Cocoa.h>
#import "../SharedCode/alerts.h"

char** __argv;
int    __argc;

static NSString* __alertTitle = nil ;

void alertSetTitle(NSString* title)
{
	[__alertTitle release];
	 __alertTitle = nil ;
	if ( title) __alertTitle = [[NSString alloc]initWithString:title];
}


NSInteger alert(NSString* msg)
{
	if ( !__alertTitle ) alertSetTitle(@"Alert");
	return NSRunAlertPanel(__alertTitle,msg,@"OK",nil,nil) ;
}

NSInteger ask(NSString* msg)
{
	if ( !__alertTitle ) alertSetTitle(@"Question");
	return NSRunAlertPanel(__alertTitle,msg,@"YES",@"NO",nil) ;
}

void arrayDump(NSArray* array)
{
	NSLog(@"arrayDump");
	for ( NSUInteger i = 0 ; i < [array count] ; i++ ) {
		NSLog(@"%d: %@",i,[array objectAtIndex:i]);
	}
}

void dictDump(NSDictionary* dictionary)
{
	NSLog(@"dictDump class = %@",[dictionary class]);
	for (id key in dictionary) {
		id value = [dictionary objectForKey:key] ;
		NSLog(@"key: %@, value: %@", key,value );
		
		if ([[[ value class ]description] isEqualTo: @"NSCFArray"]) {
			arrayDump(value) ;
		}
	}
}

NSObject* clientObject ;
SEL		  clientSelect ;

static void fsevents_callback
( ConstFSEventStreamRef streamRef
 , void *userData
 , size_t numEvents
 , void *eventPaths
 , const FSEventStreamEventFlags eventFlags[]
 , const FSEventStreamEventId eventIds[]
 ) {
	id self = (id) userData;
	[ self performSelector : clientSelect 
				 withObject: self
				afterDelay : 0 
	];
}


void initializeEventStream(NSString* path,NSObject* client,SEL select)
{
//  NSString*			myPath		= [NSString stringWithFormat:@"%@/Library/Preferences/",NSHomeDirectory()];
	NSString*		myPath			= [path stringByExpandingTildeInPath];
    NSArray*		pathsToWatch	= [NSArray arrayWithObject:myPath];
    FSEventStreamContext context	= {0, (void*) client, NULL, NULL, NULL};
    NSTimeInterval		latency		= 1.0;
	
	clientObject = client ;
	clientSelect = select ;
	
	FSEventStreamRef stream = FSEventStreamCreate(NULL
												  , &fsevents_callback
												  , &context
												  , (CFArrayRef) pathsToWatch
												  , 0// [[self lastEventId] unsignedLongLongValue]
												  , (CFAbsoluteTime) latency
												  , kFSEventStreamCreateFlagUseCFTypes 
												  ) ;
	// [client setStream:stream];
	
	FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	FSEventStreamStart(stream);
}


