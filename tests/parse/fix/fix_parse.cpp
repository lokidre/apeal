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

std::string text ;
tParseFixSchema schema ;
tObjectFIX obj ;

tParseFIX parse_fix ;
tParseCSV parse_csv ;

int main ( void )
{
  AFile<> file ;

  schema.resize(schema.size()+1), schema.back().off = 2, schema.back().len = 3 ;
  schema.resize(schema.size()+1), schema.back().off = 7, schema.back().len = 5 ;


  try {
    file.load("test",text) ;
    parse_fix.SetSchema(schema) ;
    parse_fix.parse(text,obj) ;
    parse_csv.generate(obj,text) ;
    file.save("test.csv",text) ;
  }
  catch(...) {
    printf ( "Eception\n" ) ;
  }

  return 0 ;
}


