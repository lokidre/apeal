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
#include <ctime>


#include <apl/parse/xml.c++>




//template <class stream_type>
//bool process_stream


//template <



int main()
{
  using namespace std ;

  cout << "Loading complete XML..." << flush ;

  tParseXML parser ;
  tObjectXML<string> xml ;

  if ( !parser.load("huge.xml",xml) ) {
    cerr << "Error:" << parser.error_code << ":" << parser.error_string << endl ;
    return 1 ;
  }

  cout << " done" << endl ;




  // walk through the document and parse the content
  {
    typedef tObjectXML<string>::element_list_type element_list_type ;

    for ( element_list_type::iterator 
  }



  return 0 ;
}

