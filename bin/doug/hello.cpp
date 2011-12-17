
#define HelloWorld 1

#define xstr(s) str(s)
#define str(s) #s

// 64 or 32 bit build
#if defined(_LP64)
	#define SC_64 1
	#define SC_32 0
#else
	#define SC_64 0
	#define SC_32 1
#endif


// what's the compiler?
#if defined(__APPLE_CC__)
	#define SC_XCODE 1
	#if __APPLE_CC__ >= 5442
		#define SC_XCODE3 1
		#define SC_XCODE2 0
	#else
		#define SC_XCODE3 0
		#define SC_XCODE2 1
	#endif
#else
	#define SC_XCODE 0
#endif

// endian
#ifdef  __LITTLE_ENDIAN__ && __LITTLE_ENDIAN__
#define SC_BIGENDIAN    0
#define SC_LITTLEENDIAN 1
#endif

#ifdef _BIG_ENDIAN && _BIG_ENDIAN
#define SC_BIGENDIAN    1
#define SC_LITTLEENDIAN 0
#endif


#include <stdio.h>
// #include <Carbon/Carbon.h>
#define TF(b) (b?"true":"false")



#if 0
#include <Carbon/Carbon.h>
/* ----------------------------------------------------------- */
/* getUTCTime() - get the current time()                       */
/* ----------------------------------------------------------- */
double getUTCTime()
{
	UTCDateTime dt;
#if SC_XCODE2	
	GetUTCDateTime (&dt, kUTCDefaultOptions);
#else
//  http://lists.apple.com/archives/Carbon-dev/2005/Jul/msg00868.html
    CFAbsoluteTime    adt = CFAbsoluteTimeGetCurrent();
	UCConvertCFAbsoluteTimeToUTCDateTime( adt, &dt );
#endif	
	
	int64_t time = (int64_t (dt.highSeconds) << 32) + dt.lowSeconds;
	// move from 1/1/1904 to 1/1/1970 and convert to msecs
	time = (time - 24107 * 86400) * 1000;
	int32_t msec = (((int32_t) dt.fraction * 1000) >> 16);
	time += msec;
	return (double) time;
}
#endif

int main()
{
	int   i ;
	int x   = 0x04030201 ;
	char* p = (char*) &x  ;
	
	bool bBigEndian    = SC_BIGENDIAN ? true : false ;
	bool bLittleEndian = SC_LITTLEENDIAN  ? true : false ;
	
//	int n ; printf("enter > ") ; fscanf(stdin,"%d",&n) ;


//	Debugger() ;
	
#if SC_XCODE3
	char* compiler = "XCode3 or later" ;
#endif
#if SC_XCODE2
	char* compiler = "XCode2" ;
#endif

#if SC_32
    bool sc_32 = true  ;
	bool sc_64 = false ;
#endif
#if SC_64
	bool sc_32 = false ;
	bool sc_64 = true  ;
#endif

#if 0
	printf("%s= %s = %d"
		,xstr(HelloWord),str(HelloWorld),HelloWorld) ;
	printf("hello world %f\n",getUTCTime()/1000.0);
#endif

	printf(" ptr size = %d"
	       " littleEndian, bigEndian = %s,%s"
	       " compiler = %s"
	       " SC_32,SC_64 = %s %s"
	//     " now() = %f"
	       "\n"
	      , sizeof p
	      , TF(bLittleEndian)
	      , TF(bBigEndian)
	      , compiler
	      , TF(sc_32),TF(sc_64)
	 //   , getUTCTime()
	      ) ;
	      
	for ( i = 0 ; i < sizeof x ; i++ )
	{
		printf("%d ",p[i]) ;
	}
	printf("\n") ;
	
	return 0;
}