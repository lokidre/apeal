/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/parse/xml.cxx>


#include <iostream>
#include <string>
#include <stdexcept>


int main()
{
  using namespace std ;

  int ret_code = 1 ;

  try {



    ret_code = 0 ;
  }
  catch(exception &e) {
    cerr << e.what() << endl ;
  }
  catch(...) {
    cerr << "exception" << endl ;
  }

  return ret_code ;
}
