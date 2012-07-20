#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// #include <tzfile.h>

#include <string>
#include <vector>
#include <list>
#include <map>
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
typedef list<string>           string_l;
typedef list<string>::iterator string_li;


/*
 CELLMODEM_DOWN_SIG
 CELLMODEM_UP_SIG
 CELL_STATUS_IND_SIG
 FC_LIST_SIG
 INFO_BASELINE_SIG
 MFGTEST_IN_CANCEL_SIG
 MFGTEST_IN_ENABLE_SIG
 MFGTEST_IN_FINAL_SIG
 MFGTEST_IN_FINALSTATUS_SIG
 MFGTEST_IN_LINEBIAS_SIG
 MFGTEST_IN_LINEBIASSTATUS_SIG
 MFGTEST_IN_STATUS_SIG
 MFGTEST_IN_THERMAL_SIG
 MFGTEST_IN_THERMALSTATUS_SIG
 MFGTEST_OUT_FINALPROG_SIG
 MFGTEST_OUT_FINAL_SIG
 MFGTEST_OUT_FINALSTATUS_SIG
 MFGTEST_OUT_ITEMCHANGE_SIG
 MFGTEST_OUT_STATUS_SIG
 MFGTEST_OUT_THERMAL_SIG
 MFGTEST_OUT_THERMALSTATUS_SIG
 MFGTEST_OUT_WRONG_QUERY_SIG
 MFGTEST_STOP_THERMAL_SIG
 OMNISTAR_STATUS_IND_SIG
 PORT_OPEN_SIG
 Q_ENTRY_SIG
 Q_EXIT_SIG
 Q_NEW_ATKPHASE_SIG
 Q_PHASE_SIG
 Q_POSVEL_SIG
 REQ_FC_LIST_SIG
 _SIG
 SIGTRAC_NSV_TRACKING_SIG
 SIGTRAC_OSC_GLITCH_DETECT_SIG
 SNR_DIFF_METRICS_SIG
 SNR_DIFF_START_SIG
 SNR_DIFF_STOP_SIG
 TIME_SIG
*/


#define TEN 10
#define TWENTY 20

#define addSignal(s,n) s[n]=#n

#if defined(_MSC_VER)
#include <windows.h>
#endif

int main(int argc,const char* argv[])
{
#if 0
	string_l args;
	for ( int a = 1 ; a < argc ; a++ ) {
		args.push_back(string(argv[a]));
	}

	for ( string_li it = args.begin() ; it != args.end() ; it++ ) {
		cout << *ti << endl;
	}
#endif

#if 0
	map<int, string> signals;
	addSignal(signals,TEN);
	signals[20] = "ABC" ;

	for ( int a = 1 ; a < argc ; a++ ) {
		const char* arg = argv[a];
		int   n         = atoi(arg);

		if ( signals.count(n) ) {
			printf("%s\n",signals[n].c_str());
		} else {
			printf("do not know %s\n",arg);
		}
	}
#endif

	time_t now = time(NULL);
    struct tm local ; memcpy(&local,localtime(&now),sizeof(local));
    struct tm utc   ; memcpy(&utc  ,gmtime   (&now),sizeof(utc  ));
#if   defined(__CYGWIN__)
    struct tm loc   ; memcpy(&loc  ,localtime(&now),sizeof(loc  ));
	time_t gmt = timegm(&loc);
	int offset = (int) ( ((long signed int) gmt) - ((long signed int) now) ) ;
#elif defined(_MSC_VER)
	TIME_ZONE_INFORMATION TimeZoneInfo;
	GetTimeZoneInformation( &TimeZoneInfo );
	int offset = - ((int)TimeZoneInfo.Bias + (int)TimeZoneInfo.DaylightBias) * 60;
#else
	int offset = local.tm_gmtoff ;
#endif
    printf("local: offset = %6ld dst = %d time = %s", offset,local.tm_isdst, asctime(&local));
    printf("utc  : offset = %6ld dst = %d time = %s", 0     ,utc  .tm_isdst, asctime(&utc  ));

	return 0 ;
}


