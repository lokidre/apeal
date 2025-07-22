/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <stdio.h>

#include <apl/parse.h>
#include <apl/file.h>


void generate_http_desc ( tRequestHTTP &req, std::string &desc )
{
  desc.clear() ;

  std::string s ;
}



int main ( void )
{
  AFile<> file ;
  std::string http_request_text, http_response_text, req_desc, resp_desc ;
  tRequestHTTP<> http_request ;
  tResponseHTTP<> http_response ;
  tParseHTTP http_parse ;

  try {
    file.LoadText ( "test_request.http", http_request_text ) ;
    http_parse.parse_request ( http_request_text, http_request ) ;
    generate_http_desc ( http_request, req_desc ) ;
    file.SaveText ( "test_request.desc", req_desc ) ;
  }
  catch ( t_err_except *err ) {
    printf ( "Error: %s (code '%d')\n", err->error_string.c_str(), err->error_code ) ;
  }
  catch(...) {
    printf ( "Unhandled Error\n" ) ;
  }

  return 0 ;
}

