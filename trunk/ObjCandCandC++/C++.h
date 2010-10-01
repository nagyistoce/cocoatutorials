/*
 *  C++.h
 *  ObjCandCandC++
 *
 *  Created by Robin Mills on 9/22/10.
 *  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
 *
 */

#include <objc/message.h>

int helloCxx(const char* msg);

class MyClass
{
public:
	MyClass() {} ;
	virtual ~MyClass() {} ;

// methods
	int hello(const char* msg);
	
	void hello(objc_object* obj,SEL sel,const char* msg);
};


