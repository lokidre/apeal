/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
// usage: httpcli GET <uri>


#include <iostream>
#include <string>
#include <vector>

//#include <openssl/bio.h>
//#include <openssl/err.h>
//#include <openssl/rand.h>
//#include <openssl/ssl.h>
//#include <openssl/x509v3.h>

#include <apl/ssl.c++>

#include <apl/file.h>
#include <apl/http.h>
#include <apl/parse.h>
//#include <apl/socket.h>





tParseURI uri_parse ;
tObjectURI<> uri_obj ;

tRequestHTTP<> http_request ;
tResponseHTTP<> http_response ;
tClientHTTP http_client ;
tParseHTTP http_parse ;

std::string request_text, response_text, request_body ;


// static MUTEX_TYPE *mutex_buf ;
/*
tMutex *mutex_buf ;

static void locking_function ( int mode, int n, const char *file, int line )
{
  if ( mode & CRYPTO_LOCK )
    mutex_buf[n].Lock() ;
  else
    mutex_buf[n].Unlock() ;
}

static unsigned long id_function ( void )
{
  return (unsigned long)GetCurrentThreadId() ;
}
*/

/*
SSL_CTX *setup_client_ctx ( void )
{
  SSL_CTX *ctx ;

  ctx = SSL_CTX_new(SSLv23_method()) ;
  if ( SSL_CTX_use_certificate_chain_file(ctx,"client.pem") != 1 ) {
    std::cout << "error loading certificate file" << std::endl ;
    return false ;
  }

  if ( SSL_CTX_use_PrivateKey_file(ctx,"client.pem",SSL_FILETYPE_PEM) != 1 ) {
    std::cout << "error private key" << std::endl ;
    return false ;
  }

  return ctx ;
}


bool thread_setup ( void )
{
  mutex_buf = new tMutex[CRYPTO_num_locks()] ;  // (MUTEX_TYPE *)malloc(CRYPTO_num_locks()*sizeof(MUTEX_TYPE)) ;
  for ( int i = 0 ; i < CRYPTO_num_locks() ; i++ )
    mutex_buf[i].Create() ;

  CRYPTO_set_id_callback(id_function) ;
  CRYPTO_set_locking_callback(locking_function) ;

  return true ;
}

void thread_cleanup ( void )
{
  CRYPTO_set_id_callback(NULL) ;
  CRYPTO_set_locking_callback(NULL) ;
  for ( int i = 0 ; i < CRYPTO_num_locks() ; i++ )
    mutex_buf[i].Destroy() ;
  //delete mutex_buf ;   !!!! needed bug hangs !!!
}
*/

/*
bool openssl_init ( void )
{
  if ( !thread_setup() || !SSL_library_init() ) {
    std::cout << "OpenSSL init error " << std::endl ;
    return false ;
  }

  SSL_load_error_strings() ;

  return true ;
}

void openssl_destroy ( void )
{
  thread_cleanup() ;
}
*/

/*
bool openssl_ssl_send ( SSL *ssl, const void *buff, size_t len )
{
  size_t total_sent = 0 ;
  const char *buffer = (char *)buff ;

  while ( total_sent < len ) {
    int sent = SSL_write(ssl,buffer,len-total_sent) ;
    if ( sent <= 0 )
      return false ;

    buffer += sent ;
    total_sent += sent ;
  }

  return true ;
}

bool openssl_ssl_recv ( SSL *ssl, std::string &data, size_t length )
{
  char buf[1024] ;
  const size_t buf_siz = 1024 ;
  size_t total_recv = 0 ;

  int timeout = 60 ;  // 1 minute
  time_t start_time, current_time ;
  time(&start_time) ;

  while ( length > total_recv ) {
    size_t recv_size = min(buf_siz,length-total_recv) ;

    int received = SSL_read(ssl,buf,recv_size) ;

    if ( received < 0 )
      return false ;

    time(&current_time) ;

    if ( received == 0 ) {
      if ( current_time > start_time + timeout )
        return false ;
      Sleep(100) ;
      continue ;
    }

    if ( received > 0 ) {
      start_time = current_time ;

      data.append(buf,received) ;
      total_recv += received ;
    } 
  }

  return true ;
}
*/

/*
bool openssl_ssl_recv_packet ( SSL *ssl, std::string &data, std::string &end_of_packet )
{
  char buf[1024] ;
  const size_t buf_siz = 1024 ;
  size_t total_recv = 0 ;

  int timeout = 60 ;  // 1 minute
  time_t start_time, current_time ;
  time(&start_time) ;

  for(;;) {
    int received = SSL_read(ssl,buf,1) ;

    if ( received < 0 )
      return false ;

    time(&current_time) ;

    if ( received == 0 ) {
      if ( current_time > start_time + timeout )
        return false ;
      Sleep(100) ;
      continue ;
    }

    data += buf[0] ;

    if ( data.size() >= end_of_packet.size() && strnicmp(data.data()+data.size()-end_of_packet.size(),end_of_packet.data(),end_of_packet.size()) == 0 )
      break ;

    time(&start_time) ;
  }

  return true ;
}
*/

int http_client_request ( void )
{
  //tSocket sock ;
  tSSL<> ssl ;

  uri_parse.except() ;
  ssl.except() ;
  //sock.except() ;

  try {
    // parse uri
    if ( strnicmp(http_request.uri.c_str(),"https://", 7 ) != 0 )
      http_request.uri.insert(0,"https://") ;
    uri_parse.parse(http_request.uri,uri_obj) ;

    // request headers
    http_request.host = uri_obj.host ;
    http_request.user_agent = "APL HTTP Client Test 0.01" ;
    //http_request.connection = "close" ;
    http_request.body = request_body ;

    if ( http_request.body.length() )
      strfmt ( http_request.content_length, "%d", http_request.body.length() ) ;

    http_parse.generate_request(http_request,request_text) ;


    //if ( !openssl_init() )
    //  return false ;

    //BIO *conn = NULL ;
    //SSL *ssl = NULL ;
    //SSL_CTX *ctx = NULL ;

    //seed_prng() ;

    //ctx = setup_client_ctx() ;

    std::string bio_conn_str = uri_obj.host + ":" ;
    bio_conn_str += uri_obj.port.length()? uri_obj.port : "443" ;

    ssl.Connect(bio_conn_str) ;

    /*
    if ( (conn=BIO_new_connect((char *)bio_conn_str.c_str())) == NULL )
      throw "Error creating BIO connection" ;

    if ( BIO_do_connect(conn) <= 0 )
      throw "Error connecting to remote machine" ;

    if ( (ssl=SSL_new(ctx)) == NULL )
      throw "Error creating SSL context" ;

    SSL_set_bio(ssl,conn,conn) ;
    if ( SSL_connect(ssl) <= 0 )
      throw "SSL connect failed" ;
      */

    std::cout << "Connection established" << std::endl ;


    ssl.Send(request_text) ;

    
    //if ( !openssl_ssl_send(ssl,request_text.data(),request_text.size()) )
    //  throw "send error" ;

    ssl.RecvPacket(std::string(http_parse.eor),response_text) ;

    //if ( !openssl_ssl_recv_packet ( ssl, response_text, http_parse.eor ) )
    //  throw "receive timeout" ;

    http_parse.parse_response ( response_text, http_response ) ;
    if ( http_response.content_length.length() ) {
      ssl.Recv(atoi(http_response.content_length.c_str()),http_response.body) ;
      //if ( !openssl_ssl_recv ( ssl, http_response.body, atoi(http_response.content_length.c_str()) ) )
      //  throw "receive error" ;
    }


    std::cout << "Disconnect" << std::endl ;

    //BIO_free(conn) ;
    //SSL_free(ssl) ;
    //SSL_CTX_free(ctx) ;
    //openssl_destroy() ;


  }
  catch(t_err_except *err) {
    std::cout << "Error:" << err->error_code << ":" << err->error_string << std::endl ;
    return 2 ;
  }
  catch(const char *s) {
    std::cout << s << std::endl ;
    return 1 ;
  }
  catch(...) {
    std::cout << "Exception!" << std::endl ;
    return 1 ;
  }

  return 0 ;
}




int main ( int argc, char *argv[] )
{
  if ( argc < 3 ) {
    printf ( "command line\n" ) ;
    return 0 ;
  }

  http_request.method = argv[1] ;
  http_request.uri = argv[2] ;

  if ( argc >= 4 ) {
    AFile<> f ;
    f.LoadText ( argv[3], request_body ) ;
  }


  if ( http_request.uri[0] == '@' ) {
    AFile<> f ;
    f.LoadText ( http_request.uri.c_str()+1, http_request.uri ) ;
  }

  int res = http_client_request() ;

  if ( res == 0 ) {
    AFile<> f ;

    f.SaveText ( "request_header", request_text ) ;
    f.SaveText ( "request_body", http_request.body ) ;

    f.SaveText ( "response_header", response_text ) ;
    f.SaveText ( "response_body", http_response.body ) ;
  }

  return res ;
}



