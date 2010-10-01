/*
 *  ObjC.h
 *  ObjCandCandC++
 *
 *  Created by Robin Mills on 9/22/10.
 *  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
 *
 */
#include <Cocoa/Cocoa.h>

@interface MyObject : NSObject
{
	// outlets
	
	// properties
}
// actions
- (int) sayHello : (const char*) msg ;
- (int) sayHello : (const char*) msg cppObject:(void*) cppObject;

// methods

// getters/setters

@end

