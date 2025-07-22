/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <string>
#include <iostream>

#include <apeal/parse/ini.hpp>
#include <apeal/file.hpp>


int main ( int argc, char *argv[] )
{
	using namespace apl ;
	using namespace std ;

	string string_value = "ini value" ;
	int int_value = -1 ;
	bool bool_value = false ;
	int list_value = 0 ;


	try {
		AFileSystem fs ;
		AIniParser parser ;
		AIniSchema schema ;
		string filename ;

		if ( argc > 1 )
			filename = argv[1] ;
		else
			filename = "schema.ini" ;


		schema.add ( "sample_string", &string_value ) ;
		schema.add ( "sample_bool", &bool_value ) ;
		schema.add ( "sample_int", &int_value ) ;

		if ( fs.exist(filename) )
			parser.load_schema(filename,schema) ;
		else
			parser.save_schema(filename,schema) ;


		cout << "values:" << endl ;
		cout << "string_value: " << string_value ;

	}
	catch(...) {
		cerr << "exception" << endl ;
		return 1 ;
	}

	return 0 ;
}
