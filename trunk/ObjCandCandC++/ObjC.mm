/*
 *  ObjC.m
 *  ObjCandCandC++
 *
 *  Created by Robin Mills on 9/22/10.
 *  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
 *
 */

#include "ObjC.h"
#include "C++.h"

@implementation MyObject
- (id) init
{
	self = [super init];
	return self ;
}

- (int) sayHello : (const char*) msg
{
	NSLog(@"MyObject sayHello says \"%s\"",msg) ;
	return 0 ;
}

- (int) sayHello : (const char*) msg cppObject:(void*)cppObject
{
	MyClass* that = (MyClass*) cppObject ;
	that->hello(msg);
	return 0 ;
}


@end

