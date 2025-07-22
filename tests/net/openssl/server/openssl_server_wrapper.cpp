/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
// configuration
const char *crt_file = "server.crt" ;
const char *key_file = "server.key" ;
const char *psw_file = "server_psw" ;
int ssl_port = 443 ;


#include <apeal/socket.cxx>
#include <apeal/openssl.cxx>
#include <apeal/thread.cxx>
#include <apeal/file.cxx>

#include <iostream>


// global variables for all connections
typedef apl::ALock<apl::AMutex<> > lock_type ;
apl::AMutex<> console_mutex ;


struct Connection {
	apl::ASocket<>::Address client_address ;
	apl::ASocket<> socket ;
	apl::AOpenSSL<> ssl ;


  bool finished ;
  Connection()
  {
    finished = false ;
  }

  apl::AThreadMeth<Connection> connection_thread ;

  void start()
  {
    connection_thread(this,&Connection::process_func) ;
  }


  void process_func()
  {
    using namespace std ;
    using namespace apl ;


    try {

      {
        lock_type lock(console_mutex) ;
        // TODO address string info cout << "Connection established: " << inet_ntoa(client_address.addr.sin_addr) << endl ;
      }

      const int buffer_size=1024 ;
      char buf[buffer_size+1] ;

      int bytes = ssl.read(buf,buffer_size) ;

      buf[bytes] = 0 ;

      {
        lock_type lock(console_mutex) ;
        cout << buf << endl ;
      }

      ssl.send("HTTP/1.1 200 OK\r\n" "\r\n" "ssl server sends success!") ;

      ssl.close() ;
      socket.close() ;
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

  try {
    ASocket<> server_socket ;

    // setup ssl server
    AOpenSSLServer<> ssl_server ;
    AFile<> file ;
    string password ;

    ssl_server.set_crt_file(crt_file) ;
    ssl_server.set_key_file(key_file) ;

    file.load(psw_file,password) ;
    ssl_server.set_password(password) ;


    // setup socket server
    server_socket.create() ;
    server_socket.bind(ssl_port) ;
    server_socket.listen() ;


    // connection list
    connection_list_type connection_list ;

    // main listening cycle
    for(;;) {

      try {

        // first do the maintenance
        for ( connection_list_type::iterator c_it = connection_list.begin() ; c_it != connection_list.end() ; ) {
          if ( (*c_it)->finished ) {
            // (*c_it)->conn_thread.wait(1000) ;
            delete *c_it ;
            c_it = connection_list.erase(c_it) ;
            continue ;
          }
          ++c_it ;
        }

        // create new connection
        Connection *c = new Connection ;

        server_socket.accept(c->socket,&c->client_address) ;
        ssl_server.accept(c->socket.handle(),c->ssl) ;

        // maybe it's important to push into vector after starting the thread
        // in this case if thread throws error, the connection won't stay in the list
        c->start() ;

        connection_list.push_back(c) ;
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

    ssl_server.close() ;
    server_socket.close() ;

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

#include <apeal/openssl.cpp>

