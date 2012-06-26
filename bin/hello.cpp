#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <iostream>


#define VERCHECK_VERSION_STRING "2004.11.19"
#define BUILDVERSION            "1"
#define BUILDDATE               "Fri Nov 19 15:00:00 2004"

// START IDENTIFICATION STAMP
#if defined(WIN32)
#define IDENT_STAMP "@@@BUILDINFO@@@ vercheck " ## VERCHECK_VERSION_STRING ## " " ## BUILDVERSION ## " " ## BUILDDATE
#else
#define IDENT_STAMP "@@@BUILDINFO@@@ vercheck " VERCHECK_VERSION_STRING " " BUILDVERSION " " BUILDDATE
#endif
static const int     identStampIgnore  = 16         ; // skip the @@@...@@@ stuff when printing the version
static const char*   identStamp        = IDENT_STAMP;
// END IDENTIFICATION STAMP

using namespace std;
typedef list<string> strings;

int main(int argc,const char* argv[])
{
	strings args;
	for ( int a = 1 ; a < argc ; a++ ) {
		args.push_back(string(argv[a]));
	}

	for ( strings::iterator i = args.begin() ; i != args.end() ; i++ ) {
		cout << *i << endl;
	}
	return 0 ;
}
