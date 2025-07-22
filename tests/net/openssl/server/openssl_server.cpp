/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
// configuration
const char *cert_file = "server.crt" ;
const char *key_file = "server.key" ;
const char *password_file = "server_psw" ;
int ssl_port = 443 ;


#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>


#include <exception>
#include <iostream>

#include <apeal/err.cxx>
#include <apeal/mutex.cxx>
#include <apeal/thread.cxx>
#include <apeal/file.cxx>


#if __CC == __CC_MICROSOFTC
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")

#pragma comment(lib,"ws2_32.lib")
#endif

#if __OS == __OS_WINDOWS
#include <io.h>

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



struct CRYPTO_dynlock_value {
  apl::AMutex<> mtx ;
} ;


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
  throw std::runtime_error(prefix + strerror(errno)) ;
}


static void ssl_lock_callback ( int mode, CRYPTO_dynlock_value *val, const char *file, int line )
{
  if ( mode & CRYPTO_LOCK )
    val->mtx.lock() ;
  else
    val->mtx.unlock() ;
}
/*
unsigned long id_callback()
{
  #if __OS == __OS_WINDOWS
  return (unsigned long)GetCurrentThreadId() ;
  #else
  return (unsigned long)pthread_self() ;
  #endif
}
*/

static CRYPTO_dynlock_value *ssl_create_callback ( const char *file, int line )
{
  CRYPTO_dynlock_value *val = new CRYPTO_dynlock_value ;
  if ( val ) {
    val->mtx.create() ;
  }
  return val ;
}

static void ssl_destroy_callback ( CRYPTO_dynlock_value *val, const char *file, int line )
{
  val->mtx.destroy() ;
  delete val ;
}


std::string certificate_password ;

static int password_callback ( char *buf, int size, int rwflag, void *password )
{
  strncpy(buf,certificate_password.c_str(),certificate_password.length()) ;
  return int(certificate_password.length()) ;
}



// global variables for all connections
typedef apl::ALock<apl::AMutex<> > lock_type ;
apl::AMutex<> console_mutex ;

struct Connection {
  struct sockaddr_in conn_addr ;
  int client_socket ;
  SSL *ssl ;


  bool finished ;
  Connection()
  {
    finished = false ;
  }

  apl::AThreadMeth<Connection> connection_thread ;

  void process()
  {
    connection_thread(this,&Connection::process_func) ;
  }


  void process_func()
  {
    using namespace std ;
    using namespace apl ;


    try {

      const int buffer_size=1024 ;
      char buf[buffer_size+1] ;

      int bytes = SSL_read(ssl, buf, buffer_size ) ;

      if ( bytes < 0 )
        raise_errno("read error:" ) ;

      buf[bytes] = 0 ;

      {
        lock_type lock(console_mutex) ;
        cout << buf << endl ;
      }

      const char *reply = 
        "HTTP/1.1 200 OK\r\n"
        "\r\n"
        "ssl server sends success!" ;

      // writing back cycle
      const char *pbuf = reply ;
      int total = strlen(reply) ;
      int current = 0 ;

      while ( current < total ) {

        int wrote = SSL_write ( ssl, pbuf, total-current ) ;

        if ( wrote < 0 )
          raise_errno("write error: ") ;

        if ( write == 0 )
          break ;

        current += wrote ;
        pbuf += wrote ;
      }



      SSL_shutdown(ssl) ;
      SSL_clear(ssl) ;
      SSL_free(ssl) ;
      ssl = 0 ;

      close(client_socket) ;
    }
    catch(exception &e) {
      lock_type lock(console_mutex) ;
      cerr << e.what() << endl ;
    }
    catch(...) {
      lock_type lock(console_mutex) ;
      cerr << "unhandled exception" << endl ;
    }
    finished = true ;
  }
} ;

typedef std::vector<Connection *> connection_list_type ;



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
    console_mutex.create() ;

    // initialization
    ::SSL_load_error_strings();     // load all error messages
    ::SSL_library_init() ;


    ::CRYPTO_set_dynlock_create_callback(ssl_create_callback) ;
    ::CRYPTO_set_dynlock_lock_callback(ssl_lock_callback) ;
    ::CRYPTO_set_dynlock_destroy_callback(ssl_destroy_callback) ;


    SSL_METHOD *method = SSLv23_server_method();   // create server instance

    if ( method == 0 )
      openssl_raise_error() ;


    SSL_CTX *ctx = SSL_CTX_new(method);         // create context

    if ( ctx == 0 )
      openssl_raise_error() ;

    SSL_CTX_set_mode ( ctx, SSL_MODE_AUTO_RETRY ) ;



    // loading certificate
    ::SSL_CTX_set_default_passwd_cb(ctx,password_callback) ;

    AFile<> file ;
    file.load(password_file,certificate_password) ;

    if ( SSL_CTX_use_certificate_chain_file ( ctx, cert_file ) != 1 )
      openssl_raise_error() ;

    if ( SSL_CTX_use_PrivateKey_file ( ctx, key_file, SSL_FILETYPE_PEM ) != 1 )
      openssl_raise_error() ;

    if ( !SSL_CTX_check_private_key(ctx) )
      throw runtime_error("private key doesn't match certificate") ;



    // creating server socket
    struct sockaddr_in addr;
    memset ( &addr, 0, sizeof(addr) ) ;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ssl_port) ;
    addr.sin_addr.s_addr = INADDR_ANY ;

    int server_socket = ::socket ( PF_INET, SOCK_STREAM, 0 ) ;

    if ( server_socket == -1 )
      raise_errno("socket error: ") ;
    
    if ( ::bind ( server_socket, (struct sockaddr*)&addr, sizeof(addr) ) == -1 )
      raise_errno("bind error: ") ;

    if ( ::listen ( server_socket, SOMAXCONN ) == -1 )
      raise_errno("listen error: ") ;

    // connection list
    connection_list_type connection_list ;

    // main listening cycle
    for(;;) {

      try {

        // first do the maintenance
        for ( connection_list_type::iterator c_it = connection_list.begin() ; c_it != connection_list.end() ; ) {
          if ( (*c_it)->finished ) {
            delete *c_it ;
            c_it = connection_list.erase(c_it) ;
            continue ;
          }
          ++c_it ;
        }

        // create new connection
        Connection *c = new Connection ;

        // connecting party address
        // struct sockaddr_in conn_addr ;
        socklen_t addr_len = sizeof(c->conn_addr) ;

        memset(&c->conn_addr,0,sizeof(c->conn_addr)) ;

        c->client_socket = ::accept ( server_socket, (sockaddr *)&c->conn_addr, &addr_len ) ;

        if ( c->client_socket == -1 )
          raise_errno("accept error: " ) ;

        // show information about connection
        {
          lock_type lock(console_mutex) ;
          cout << "Connection established: " << inet_ntoa(c->conn_addr.sin_addr) << endl ;
        }


        // pass socket to ssl
        c->ssl = SSL_new(ctx) ;
        if ( c->ssl == 0 )
          openssl_raise_error() ;

        SSL_set_fd(c->ssl, c->client_socket);    // set connection to SSL state
        if ( SSL_accept(c->ssl) <= 0 )           // start the handshaking
          openssl_raise_error() ;

        connection_list.push_back(c) ;
        c->process() ;
      }
      catch(exception &e) {
        lock_type lock(console_mutex) ;
        cerr << e.what() << endl ;
      }
      catch(...) {
        lock_type lock(console_mutex) ;
        cerr << "unhandled exception" << endl ;
      }
      
    }  // main connection cycle


    SSL_CTX_free(ctx) ;
    ctx = 0 ;


    // 
    // Server cleanup
    //
    close(server_socket) ;

    CRYPTO_set_dynlock_create_callback(0) ;
    CRYPTO_set_dynlock_lock_callback(0) ;
    CRYPTO_set_dynlock_destroy_callback(0) ;

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

