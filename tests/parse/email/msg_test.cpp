/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>
#include <string>
#include <vector>

#include <apeal/file.hpp>
#include <apeal/parse/txtmsg.hpp>


int main ( int argc, char *argv[] )
{
  using namespace apl ;
  using namespace std ;

  if ( argc <= 1 ) {
    cout << "msg_test <file>" << endl ;
    return 1 ;
  }

  AFile file ;
  ATextMessageParser msg_parse ;
  ATextMessage msg ;
  string msg_text ;

  try {
    //file.load(argv[1],msg_text) ;
    msg_parse.load(argv[1],msg) ;

    // print out the infomation
    {
      for ( auto f = msg.header.fields.begin() ; f != msg.header.fields.end() ; ++f ) {
        std::cout << f->first << ": " << f->second << std::endl ;
      }

      cout << endl ;
      cout << msg.body << endl ;
    }
  }
  catch(exception &e) {
    cout << "Error:" << e.what() << endl ;
  }
  catch(...) {
    cout << "Exception"  << endl ;
  }
  
  return 0 ;
}

