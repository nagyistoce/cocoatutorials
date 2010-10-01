#include <CoreFoundation/CoreFoundation.h>

#include "ObjC.h"
#include "C++.h"
#include "C.h"

int main (int argc, const char * argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
    // insert code here...
    CFShow(CFSTR("Hello, World!\n"));
	
	//helloC   ("calling from main") ;
	//helloObjC("calling from main") ;
	helloCxx("calling from main") ;
	
	MyClass myClass ;
	myClass.hello("calling c++ from main");
	
	MyObject* myObject = [[MyObject alloc]init];
	[myObject sayHello:"hello from main"];
	
	SEL sel = NSSelectorFromString(@"sayHello:");
	myClass.hello(myObject,sel,"get C++ to say hello using an Obj/C objcect");
	
	[myObject sayHello:"get Obj/C to say hello using a C++ object" cppObject:&myClass];
	
    [pool drain];
    
	return 0;
}
