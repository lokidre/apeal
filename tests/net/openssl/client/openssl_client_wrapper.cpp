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

#include <apeal/socket.cxx>
#include <apeal/openssl.cxx>

#include <iostream>


int main()
{
  using namespace std ;
  using namespace apl ;

  int ret_code = 1 ;

  try {
    ASocket<> socket ;
    AOpenSSL<> ssl ;
    string request("GET / HTTP/1.1\r\n""\r\n"), response ;

    socket.create() ;
    socket.connect(ssl_host,ssl_port) ;

    ssl.connect(socket.handle()) ;

    ssl.send(request) ;
    ssl.recv(response) ;

    cout << response << endl ;


    ssl.close() ;
    socket.close() ;
  }
  catch(exception &e) {
    cerr << e.what() << endl ;
  }
  catch(...) {
    cerr << "unhandled exception" << endl ;
  }


  return ret_code ;
}
