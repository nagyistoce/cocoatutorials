/*
 * http.cpp
 */

#include "exv_conf.h"
#include <exiv2/exiv2.hpp>

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>

#ifdef  _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

#if EXV_USE_CURL == 0

#ifdef  _MSC_VER
#pragma message("Using exiv2 http support")
#endif

/**********************
 * Using EXIV2_HTTP
 **********************
 */
#define SLEEP       1000
#define SNOOZE         0


#if defined(__MINGW32__) || defined(__MINGW64__)
#define __MINGW__
#define  fopen_S(f,n,a)  f=fopen(n,a)
#endif

////////////////////////////////////////
// platform specific code
#if defined(WIN32) || defined(_MSC_VER)
#include <string.h>
#include <windows.h>
#include <io.h>
#ifndef  __MINGW__
#define  snprintf sprintf_s
#define  write    _write
#define  read     _read
#define  close    _close
#define  strdup   _strdup
#define  stat     _stat
#define  fopen_S(f,n,a)  fopen_s(&f,n,a)
#endif
#else
////////////////////////////////////////
// Unix or Mac

#define  closesocket close

#include <unistd.h>
#include <strings.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define fopen_S(f,n,o) f=fopen(n,o)
#define WINAPI
typedef unsigned long DWORD ;

#define SOCKET_ERROR        -1
#define WSAEWOULDBLOCK  EINPROGRESS
#define WSAENOTCONN     EAGAIN

static int WSAGetLastError()
{
    return errno ;
}

static void Sleep(int millisecs)
{
    const struct timespec rqtp = { 0 , millisecs*1000000 } ;
    struct timespec   rmtp ;
    nanosleep(&rqtp,&rmtp);
}

#endif

////////////////////////////////////////
// code
const char* httpTemplate =
"%s %s HTTP/%s\r\n"            // $verb $page $version
"User-Agent: exiv2http/1.0.0\r\n"
"Accept: */*\r\n"
"Host: %s\r\n"                 // $servername
"%s"                           // $header
"\r\n"
;

#ifndef lengthof
#define lengthof(x) (sizeof(x)/sizeof((x)[0]))
#endif

#define white(c) ((c == ' ') || (c == '\t'))

#define FINISH          -999
#define OK(s)    (200 <= s  && s < 300)

const char*   blankLines[] =
{       "\r\n\r\n"             // this is the standard
,       "\n\n"                 // this is commonly sent by CGI scripts
}  ;

int             snooze    = SNOOZE    ;
int             sleep_    =  SLEEP    ;

static void report(const char* msg,std::string& errors)
{
    errors += std::string(msg) + '\n';
}

static int forgive(int n,int& err)
{
    err  = WSAGetLastError() ;
    if ( !n && !err ) return FINISH ;
#ifndef WIN32
    if ( n == 0 ) return FINISH ; // server hungup
#endif
    bool bForgive = err == WSAEWOULDBLOCK || err == WSAENOTCONN ;
    bool bError   = n == SOCKET_ERROR ;
    if ( bError && bForgive ) return 0 ;
    return n ;
}

static int error(std::string errors,const char* msg,const char* x=NULL,const char* y=NULL,int z=0);
static int error(std::string errors,const char* msg,const char* x     ,const char* y     ,int z )
{
    char buffer[512] ;
    snprintf(buffer,sizeof buffer,msg,x,y,z) ;
    if ( errno ) {
        perror(buffer) ;
    } else {
        fprintf(stderr,"%s\n",buffer);
    }
    report(buffer,errors) ;
    return -1 ;
}

static void flushBuffer(const char* buffer,size_t start,int& end,std::string& file)
{
    file += std::string(buffer+start,end-start) ;
    end = 0 ;
}

static Exiv2::dict_t stringToDict(const std::string& s)
{
    Exiv2::dict_t result;
    std::string   token;

    size_t i = 0;
    while (i < s.length() )
    {
        if ( s[i] != ',' ) {
            if ( s[i] != ' ' )
                token += s[i];
        } else {
            result[token]=token;
            token="";
        }
        i++;
    }
    result[token]=token;
    return result;
}

static int makeNonBlocking(int sockfd)
{
#ifdef WIN32
  ULONG  ioctl_opt = 1;
  return ioctlsocket(sockfd, FIONBIO, &ioctl_opt);
#else
  int    result  = fcntl(sockfd, F_SETFL, O_NONBLOCK);
  return result >= 0 ? result : SOCKET_ERROR ;
#endif
}

int Exiv2::http(dict_t& request,dict_t& response,std::string& errors)
{
    if ( !request.count("verb")   ) request["verb"   ] = "GET";
    if ( !request.count("header") ) request["header" ] = ""   ;
    if ( !request.count("version")) request["version"] = "1.0";
    if ( !request.count("port")   ) request["port"   ] = ""   ;
    
    std::string file;
    errors     = "";
    int result = 0;

    ////////////////////////////////////
    // Windows specific code
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
#endif

    const char* servername = request["server" ].c_str();
    const char* page       = request["page"   ].c_str();
    const char* verb       = request["verb"   ].c_str();
    const char* header     = request["header" ].c_str();
    const char* version    = request["version"].c_str();
    const char* port       = request["port"   ].c_str();
    
    const char* servername_p = servername;
    const char* port_p       = port      ;
    std::string url = std::string("http://") + request["server"] + request["page"];
    
    // parse and change server if using a proxy
    const char* PROXI  = "HTTP_PROXY";
    const char* proxi  = "http_proxy";
    const char* PROXY  = getenv(PROXI);
    const char* proxy  = getenv(proxi);
    bool        bProx  = PROXY || proxy;
    const char* prox   = bProx ? (proxy?proxy:PROXY):"";
    Exiv2::Uri  Proxy  =  Exiv2::Uri::Parse(prox);

    // find the dictionary of no_proxy servers
    const char* NO_PROXI = "NO_PROXY";
    const char* no_proxi = "no_proxy";
    const char* NO_PROXY = getenv(NO_PROXI);
    const char* no_proxy = getenv(no_proxi);
    bool        bNoProxy = NO_PROXY||no_proxy;
    std::string no_prox  = std::string(bNoProxy?(no_proxy?no_proxy:NO_PROXY):"");
    Exiv2::dict_t noProxy= stringToDict(no_prox + ",localhost,127.0.0.1");

    // if the server is on the no_proxy list ... ignore the proxy!
    if ( noProxy.count(servername) ) bProx = false;
    
	if (  bProx ) {
		servername_p = Proxy.Host.c_str();
		port_p       = Proxy.Port.c_str();
        page         = url.c_str();
        std::string  p(proxy?proxi:PROXI);
    //  std::cerr << p << '=' << prox << " page = " << page << std::endl;
	}
    if ( !port  [0] ) port   = "80";
    if ( !port_p[0] ) port_p = "80";
	
    ////////////////////////////////////
    // open the socket
    int     sockfd = (int) socket(AF_INET , SOCK_STREAM,IPPROTO_TCP) ;
    if (    sockfd < 0 ) return error("unable to create socket\n",NULL,NULL,0) ;

    // connect the socket to the server
    int     server  = -1 ;

    // fill in the address
    struct  sockaddr_in serv_addr   ;
    int                 serv_len = sizeof(serv_addr);
    memset((char *)&serv_addr,0,serv_len);

    serv_addr.sin_addr.s_addr   = inet_addr(servername_p);
    serv_addr.sin_family        = AF_INET    ;
    serv_addr.sin_port          = htons(atoi(port_p));

    // convert unknown servername into IP address
    // http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=/rzab6/rzab6uafinet.htm
    if (serv_addr.sin_addr.s_addr == (unsigned long)INADDR_NONE)
    {
        struct hostent* host = gethostbyname(servername_p);
        if ( !host )  return error("no such host",servername_p,NULL,0);
        memcpy(&serv_addr.sin_addr,host->h_addr,sizeof(serv_addr.sin_addr));
    }
    
    makeNonBlocking(sockfd) ;

    ////////////////////////////////////
    // and connect
    server = connect(sockfd, (const struct sockaddr *) &serv_addr, serv_len) ;
    if ( server == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK )
        return error(errors,"error - unable to connect to server = %s port = %s wsa_error = %d",servername_p,port_p,WSAGetLastError());

    char   buffer[32*1024+1];
    size_t buff_l= sizeof buffer - 1 ;

    ////////////////////////////////////
    // format the request
    int    n  = snprintf(buffer,buff_l,httpTemplate,verb,page,version,servername,header) ;
    buffer[n] = 0 ;
    response["requestheaders"]=std::string(buffer,n);


    ////////////////////////////////////
    // send the header (we'll have to wait for the connection by the non-blocking socket)
    while ( sleep_ >= 0 && send(sockfd,buffer,n,0) == SOCKET_ERROR /* && WSAGetLastError() == WSAENOTCONN */ ) {
        Sleep(snooze) ;
        sleep_ -= snooze ;
    }

    if ( sleep_ < 0 )
        return error(errors,"error - timeout connecting to server = %s port = %s wsa_error = %d",servername,port,WSAGetLastError());

    int    end   = 0         ; // write position in buffer
    bool   bSearching = true ; // looking for headers in the response
    int    status= 200       ; // assume happiness

    ////////////////////////////////////
    // read and process the response
    int err ;
    n=forgive(recv(sockfd,buffer,(int)buff_l,0),err) ;
    while ( n >= 0 && OK(status) ) {
        if ( n ) {
            end += n ;
            buffer[end] = 0 ;

            size_t body = 0         ; // start of body
            if ( bSearching ) {

                // search for the body
                for ( size_t b = 0 ; bSearching && b < lengthof(blankLines) ; b++ ) {
                    if ( strstr(buffer,blankLines[b]) ) {
                        bSearching = false ;
                        body   = (int) ( strstr(buffer,blankLines[b]) - buffer ) + strlen(blankLines[b]) ;
                        status = atoi(strchr(buffer,' ')) ;
                    }
                }

                // parse response headers
                char* h = buffer;
                char  C = ':' ;
                char  N = '\n';
                int   i = 0   ; // initial byte in buffer
                while(buffer[i] == N ) i++;
                h       = strchr(h+i,N)+1;
                response[""]=std::string(buffer+i).substr(0,h-buffer-2);
                result = atoi(strchr(buffer,' '));
                char* c = strchr(h,C);
                char* n = strchr(h,N);
                while ( c && n && c < n && h < buffer+body ) {
                    std::string key(h);
                    std::string value(c+1);
                    key   = key.substr(0,c-h);
                    value = value.substr(0,n-c-1);
                    response[key]=value;
                    h = n+1;
                    c = strchr(h,C);
                    n = strchr(h,N);
                }
            }

            // if the bufffer's full and we're still searching - give up!
            // this handles the possibility that there are no headers
            if ( bSearching && buff_l-end < 10 ) {
                bSearching = false ;
                body  = 0 ;
            }
            if ( !bSearching && OK(status) ) {
                flushBuffer(buffer,body,end,file);
            }
        }
        n=forgive(recv(sockfd,buffer+end,(int)(buff_l-end),0),err) ;
        if ( !n ) {
            Sleep(snooze) ;
            sleep_ -= snooze ;
            if ( sleep_ < 0 ) n = FINISH ;
        }
    }

    if ( n != FINISH || !OK(status) ) {
        snprintf(buffer,sizeof buffer,"wsa_error = %d,n = %d,sleep_ = %d status = %d"
                ,   WSAGetLastError()
                ,   n
                ,   sleep_
                ,   status
                ) ;
        error(errors,buffer,NULL,NULL,0) ;
    } else if ( bSearching && OK(status) ) {
        if ( end ) {
        //  we finished OK without finding headers, flush the buffer
            flushBuffer(buffer,0,end,file) ;
        } else {
            return error(errors,"error - no response from server = %s port = %s wsa_error = %d",servername,port,WSAGetLastError());
        }
    }

    ////////////////////////////////////
    // close sockets
    closesocket(server) ;
    closesocket(sockfd) ;
    response["body"]=file;
    return result;
}

////
#else

/**********************
 * Using LIBCURL
 **********************
 */
#include <curl/curl.h>

#ifdef    _MSC_VER
# pragma message("Using curl http support")
# ifdef   _WINDLL
#  pragma comment(lib, "curldll.lib")
# else
#  pragma comment(lib, "curl.lib")
# endif
#endif

// callback function for curl to write the header output
static int writerHeader(char *buffer, size_t size, size_t nmemb, Exiv2::dict_t *response) {
    if (buffer == NULL) return 0;

    // try to parse response headers like EXIV2_HTTP
    int length = strlen(buffer);
    while(length > 0 && ((buffer[length-1] == '\r') || (buffer[length-1] == '\n')) ) length--;
    if (length) {
        buffer[length] = '\0';
        char* h = buffer;
        char  C = ':' ;
        char* c = strchr(h,C);
        if (!c) {
            std::string value(buffer);
            (*response)[""] = value;
        } else {
            std::string key(h);
            std::string value(c+1);
            key   = key.substr(0,c-h);
            (*response)[key]=value;
        }
    }

    return size * nmemb;
}

// callback function for curl to write the body output
static int writerBody(char *data, size_t size, size_t nmemb, std::string *writerData) {
  if (writerData == NULL) return 0;
  writerData->append(data, size*nmemb);
  return size * nmemb;
}

int Exiv2::http(dict_t& request,dict_t& response,std::string& errors) {
    if ( !request.count("verb")   ) request["verb"   ] = "GET";
    if ( !request.count("header") ) request["header" ] = ""   ;
    if ( !request.count("version")) request["version"] = "1.0";
    if ( !request.count("port")   ) request["port"   ] = "80";

    errors     = "";
    int result = 0;
    std::string url = std::string("http://") + request["server"] + ":" + request["port"] + request["page"];

    // init the curl session
    CURL *curl = curl_easy_init();

    if(!curl) {
        errors = "there is an error when initing the lib curl!";
        result = -1;
    } else {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // set URL to get
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L); // no progress meter please
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, writerHeader);
        curl_easy_setopt(curl, CURLOPT_WRITEHEADER, &response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writerBody); // send all body data to this function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response["body"]);

        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); // for debug only

        // set HTTP headers
        if (request["header"] != "") {
            curl_slist *headers = NULL;
            headers = curl_slist_append(headers, request["header"].c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }

        // HTTP verbs
        if (request["verb"] == "HEAD") { // for HEAD
            curl_easy_setopt(curl, CURLOPT_NOBODY,1 );
        } else if (request["verb"] != "GET") { // the request instead of GET or HEAD
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request["verb"].c_str());
        }

        // HTTP version
        if (request["version"] == "1.0") {
            curl_easy_setopt (curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
        } else {
            curl_easy_setopt (curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
        }

        // send the request
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) { // error happends
            errors = curl_easy_strerror(res);
            result = -1;
        } else {
            long foo;
            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &foo); // get HTTP return code
            result = (int) foo;    
        }
    }

    return result;
}

#endif

// http://stackoverflow.com/questions/2616011/easy-way-to-parse-a-url-in-c-cross-platform
Exiv2::Uri Exiv2::Uri::Parse(const std::string &uri)
{
    Uri result;

    typedef std::string::const_iterator iterator_t;

    if ( !uri.length() )  return result;

    iterator_t uriEnd = uri.end();

    // get query start
    iterator_t queryStart = std::find(uri.begin(), uriEnd, '?');

    // protocol
    iterator_t protocolStart = uri.begin();
    iterator_t protocolEnd   = std::find(protocolStart, uriEnd, ':');            //"://");

    if (protocolEnd != uriEnd)
    {
        std::string prot = &*(protocolEnd);
        if ((prot.length() > 3) && (prot.substr(0, 3) == "://"))
        {
            result.Protocol = std::string(protocolStart, protocolEnd);
            protocolEnd += 3;   //      ://
        }
        else
            protocolEnd = uri.begin();  // no protocol
    }
    else
        protocolEnd = uri.begin();  // no protocol

    // host
    iterator_t hostStart = protocolEnd;
    iterator_t pathStart = std::find(hostStart, uriEnd, '/');  // get pathStart

    iterator_t hostEnd = std::find(protocolEnd,
        (pathStart != uriEnd) ? pathStart : queryStart,
        ':');  // check for port

    result.Host = std::string(hostStart, hostEnd);

    // port
    if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':'))  // we have a port
    {
        hostEnd++;
        iterator_t portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
        result.Port = std::string(hostEnd, portEnd);
    }
    if ( !result.Port.length() && result.Protocol == "http" ) result.Port = "80";

    // path
    if (pathStart != uriEnd)
        result.Path = std::string(pathStart, queryStart);

    // query
    if (queryStart != uriEnd)
        result.QueryString = std::string(queryStart, uri.end());

    return result;
}   // Parse

// That's all Folks
