/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/file.cxx>
#include <apeal/parse.cxx>
#include <apeal/socket.cxx>
#include <apeal/thread.cxx>
#include <apeal/mutex.cxx>

#include <iostream>
#include <string>


int server_port = 80 ;
std::string home_dir = "public_html" ;


typedef apl::ALock<apl::AMutex<> > lock_type ;
apl::AMutex<> console_mutex ;


void process_request ( apl::AHttpRequest<> &http_request, apl::AHttpResponse<> &http_response )
{
  using namespace apl ;
  using namespace std ;

  bool res = true ;

  AUriParser<> uri_parser ;
  AUri<> uri ;
  AFile<> file ;

  try {
    uri_parser.parse(http_request.uri,uri) ;

    if ( stricmp(http_request.method,"GET") == 0 ) {

      std::string resource_file_path ;

      if ( uri.abs_path == "/" ) {
        MakeFilePath(home_dir,"index.html",resource_file_path) ;
      } else {
        MakeFilePath(home_dir,uri.abs_path,resource_file_path) ;
      }

      file.load(resource_file_path,http_response.body) ;
      http_response.content_type = "text/html" ;

    }
  }
  catch(exception &e) {
    lock_type lock(console_mutex) ;
    cerr << e.what() << endl ;
    res = false ;
  }
  catch(...) {
    lock_type lock(console_mutex) ;
    cerr << "exception!" << endl ;
    res = false ;
  }

  if ( !res ) {
    http_response.status_code = "500" ;
    http_response.reason_phrase = "Internal Server Error" ;
  } 
}


struct Connection {
	apl::ASocket<> sock ;
	apl::ASocket<>::Address adr ;
	apl::AThread<> conn_thread ;
	bool finished = false ;
	void start() ;
	static void *start_func ( void *p ) ;
	void conn_thread_func() ;
} ;

void Connection::start()
{
  // create new thread
  conn_thread.parm = this ;
  conn_thread.func = start_func ;
  conn_thread.create() ;
}

void *Connection::start_func ( void *param )
{
  Connection *c = (Connection *)param ;
  c->conn_thread_func() ;
  return 0 ;
}

void Connection::conn_thread_func()
{
  using namespace apl ;
  using namespace std ;

  AHttpParser<> http_parser ;
  AHttpRequest<> http_request ;
  AHttpResponse<> http_response ;
  string request_text, response_text ;
  int content_length = 0 ;

  try {
    sock.recv_packet(request_text,http_parser.eor) ;
    http_parser.parse_request(request_text,http_request) ;

    if ( http_request.content_length.size() && parse_int(http_request.content_length,content_length) && content_length > 0 )
      sock.recv(content_length,http_request.body) ;

    http_response.status_code = "200" ;
    http_response.reason_phrase = "OK" ;
    http_response.connection = "close" ;
    http_response.cache_control = "no-cache" ;
    http_response.server = "APL Test HTTP Server 0.1" ;

    process_request(http_request,http_response) ;

    http_parser.generate_response(http_response,response_text) ;
    sock.send(response_text) ;
  }
  catch(exception &e) {
    lock_type lock(console_mutex) ;
    // TODO cerr << e.what() << " with " << inet_ntoa(adr.adr.sin_addr) << endl ;
  }
  catch(...) {
    lock_type lock(console_mutex) ;
    // TODO cerr << "unknown exception with " << inet_ntoa(adr.addr.sin_addr) << endl ;
  }

  {
    lock_type lock(console_mutex) ;
    // TODO cout << "disconnected from " << inet_ntoa(adr.adr.sin_addr) << endl ;
  }

  finished = true ;
}




void server_listen()
{
  using namespace apl ;
  using namespace std ;

  ASocket<> listen_socket ;
  ASocket<>::Address listen_adr ;

  vector<Connection *> connection_list ;

  listen_socket.create() ;
  listen_adr.set_port(server_port) ;


  listen_socket.bind(listen_adr) ;

  for (;;) {
    try {

      // create new connection object
      Connection *c = new Connection ;

      {
        lock_type lock(console_mutex) ;
        cout << "listening on " << server_port << " ..." << endl ;
      }

      listen_socket.listen() ;
      listen_socket.accept(c->sock,&c->adr) ;

      {
        lock_type lock(console_mutex) ;
        // TODO cout << "connection established with " << inet_ntoa(c->adr.adr.sin_addr) << " on " << c->adr.adr.sin_port << endl ;
      }


      c->start() ;
      connection_list.push_back(c) ;

      // cleanup
      for ( vector<Connection *>::iterator cit = connection_list.begin() ; cit != connection_list.end() ; ) {
        if ( (*cit)->finished ) {
          (*cit)->conn_thread.wait(1000*2) ;
          delete *cit ;
          cit = connection_list.erase(cit) ;
        } else {
          ++cit ;
        }
      }
    }
    catch(exception &e) {
      lock_type lock(console_mutex) ;
      cerr << e.what() << endl ;
    }
    catch(...) {
      lock_type lock(console_mutex) ;
      cerr << "unknown exception" << endl ;
    }

  }  // main cycle
}



int main ( int argc, char *argv[] )
{
  try {
    for ( int i = 1 ; i < argc ; i++ ) {
      if ( strcmp(argv[i],"-p") == 0 )
        apl::parse_int(argv[i+1],server_port) ;
    }

    console_mutex.create() ;

    server_listen() ;
  } 
  catch ( std::exception &e ) {
    std::cerr << e.what() << std::endl ;
  }
  catch(...) {
    std::cerr << "unhandled exception!" << std::endl ;
  }

  return 0 ;
}

