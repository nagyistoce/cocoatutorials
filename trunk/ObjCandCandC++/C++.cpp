/*
 *  C++.cpp
 *  ObjCandCandC++
 *
 *  Created by Robin Mills on 9/22/10.
 *  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
 *
 */

#include "C++.h"
#include <iostream>

using namespace std;

int helloCxx(const char* msg)
{
	cout << "helloC++ says \"" << msg << "\"" << endl ;
	return 0;
}
	
int MyClass::hello(const char* msg)
{
	cout << "MyClass::hello says \"" << msg << "\"" << endl ;
	return 0;
}

void MyClass::hello(objc_object* obj,SEL sel,const char* msg)
{
	objc_msgSend(obj,sel,msg);
}



