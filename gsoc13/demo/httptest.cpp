/*
 http.cpp
 */

#include "http.hpp"
#include <iostream>
using namespace std;

int main(int argc,const char** argv)
{
    if ( argc < 3 ) {
        cout << "usage  : " << argv[0] << " [key value]+" << endl;
        cout << "example: " << argv[0] << " [-url url | -server clanmills.com -page /LargsPanorama.jpg] -header \"Range: bytes=0-200\"" << endl;
		cout << "or     : " << argv[0] << " -url http://clanmills.com/robin.shtml" << endl;
        cout << "useful  keys: -verb {GET|HEAD|PUT}  -page str -server str -port number -version [-header something]+ " << endl;
        cout << "default keys: -verb GET -server clanmills.com -page robin.shtml -port 80 -version 1.0" << endl;
        cout << "export http_proxy=url eg http_proxy=http://64.62.247.244:80" << endl;
        return 0;
    }

    Exiv2::dict_t response;
    Exiv2::dict_t request;
    string errors;

    request["page"  ]   = "robin.shtml";
    request["server"]   = "clanmills.com";

    // convert the command-line arguments into the request dictionary
    for ( int i = 1 ; i < argc-1 ; i +=2 ) {
        const char* arg = argv[i];
        // skip past the -'s on the key
        while ( arg[0] == '-' ) arg++;

        if ( string(arg) == "header" ) {
            string header = argv[i+1];
            if ( ! strchr(argv[i+1],'\n') ) {
                header += "\r\n";
            }
            request[arg] += header;
        } else if ( string(arg) == "uri" || string(arg) == "url" ) {
        	Exiv2::Uri uri = Exiv2::Uri::Parse(argv[i+1]);
        	if ( uri.Protocol == "http" ) {
        	    request["server"] = uri.Host;
        	    request["page"]   = uri.Path;
        	    request["port"]   = uri.Port;
        	}
        } else {
            request[arg]=argv[i+1];
        }
    }

    int result = Exiv2::http(request,response,errors);
    cout << "result = " << result << endl;
    cout << "errors = " << errors << endl;
    cout << endl;

    for ( Exiv2::dict_i it = response.begin() ; it != response.end() ; it++ ) {
        // don't show request header
        if (it->first ==  "requestheaders") continue;

        cout << it->first << " -> ";

        if ( it->first ==  "body") {
        	string& value = it->second;
        	cout << "# " << value.length();
        	if ( value.length() < 1000 ) cout << " = " << value ;
        } else {
        	cout << it->second;
        }

        cout << endl;
    }

    return 0;
}

// That's all Folks!
////
