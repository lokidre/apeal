/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
// configuration
const char *ssl_host = "127.0.0.1" ;
int ssl_port = 443 ;


#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>


#include <exception>
#include <iostream>
#include <io.h>

#include <apeal/err.cxx>
#include <apeal/mutex.cxx>
#include <apeal/thread.cxx>
#include <apeal/file.cxx>

#include <apeal/err/eerrno.cxx>


#if __CC == __CC_MICROSOFTC
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")

#pragma comment(lib,"ws2_32.lib")
#endif

#if __OS == __OS_WINDOWS
#define close closesocket
#define socklen_t int
#endif

#if __OS == __OS_UNIX
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#endif



void openssl_get_errors(std::string &error_string)
{
  int code ;
  char buf[1024] ;
  while((code=::ERR_get_error()) != 0 ) {
    ::ERR_error_string(code,buf) ;
    if ( error_string.size() )
      error_string.append(1,'\n') ;
    error_string.append(buf) ;
  }
}

void openssl_raise_error()
{
	std::string s ;
	openssl_get_errors(s) ;
	throw std::runtime_error(s.c_str()) ;
}

void raise_errno(const std::string &prefix)
{
	throw std::runtime_error(prefix + apl::get_error_string_errno(errno)) ;
}



int main()
{
  using namespace std ;
  using namespace apl ;

  int ret_code = 1 ;

  #if __OS == __OS_WINDOWS
  WSADATA _ws_wsdata ;
  WORD wVersionRequested = MAKEWORD(2,2) ;
  ::WSAStartup ( wVersionRequested, &_ws_wsdata ) ;
  #endif


  try {

    struct hostent *host = gethostbyname(ssl_host) ;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ssl_port) ;
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
    
    int sd = socket ( PF_INET, SOCK_STREAM, 0 ) ;
    if ( connect ( sd, (struct sockaddr*)&addr, sizeof(addr) ) != 0 )
      raise_errno("connect error: ") ;


    // now pass the connection to SSL
    ::SSL_load_error_strings();     // load all error messages
    ::SSL_library_init() ;


    SSL_METHOD *method = SSLv23_client_method() ;
    if ( method == 0 )
      openssl_raise_error() ;

    SSL_CTX *ctx = SSL_CTX_new(method) ;
    if ( ctx == 0 )
      openssl_raise_error() ;

    SSL_CTX_set_mode ( ctx, SSL_MODE_AUTO_RETRY ) ;


    SSL *ssl = SSL_new(ctx) ;
    if ( ssl == 0 )
      openssl_raise_error() ;

    SSL_set_fd(ssl, sd) ;
    if ( SSL_connect(ssl) != 1 )   // establish ssl connection
      openssl_raise_error() ;

    //
    // send to the server
    //
    {
      const char *request =
        "GET / HTTP/1.1\r\n"
        "\r\n" ;

      int total = strlen(request) ;
      const char *pbuf = request ;
      int current = 0 ;

      while ( current < total ) {
        int wrote = SSL_write ( ssl, pbuf, total-current ) ;

        if ( wrote < 0 )
          openssl_raise_error() ;

        if ( wrote == 0 )
          break ;

        current += wrote ;
        pbuf += wrote ;
      }

    }



    //
    // receive from the server
    //
    {
      const int buffer_size=1024 ;
      char buf[buffer_size+1] ;

      for(;;) {
        int bytes = SSL_read(ssl, buf, buffer_size ) ;

        if ( bytes == 0 )
          break ;

        if ( bytes < 0 )
          openssl_raise_error() ;

        buf[bytes] = 0 ;
        cout << buf << flush ;
      }
    }


    SSL_shutdown(ssl) ;
    SSL_clear(ssl) ;
    SSL_free(ssl) ;
    ssl = 0 ;

    close(sd) ;


    SSL_CTX_free(ctx) ;
    ctx = 0 ;


    ret_code = 0 ;
  }
  catch(exception &e) {
    cerr << e.what() << endl ;
  }
  catch(...) {
    cerr << "unhandled exception" << endl ;
  }

  return ret_code ;
}

