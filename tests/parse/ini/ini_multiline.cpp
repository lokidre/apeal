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

using namespace apl ;


void dump ( const apl::AIni &ini )
{
	for ( auto &el : ini.el ) {
		std::cout << el.name << " = " << el.value << std::endl ;
	}
}

int main ( int argc, char *argv[] )
{
	int retval = 1 ;

	std::string string_value = "ini value" ;
	int int_value = -1 ;
	bool bool_value = false ;
	int list_value = 0 ;


	try {
		AFileSystem fs ;
		AIniParser parser ;
		AIni ini ;
		std::string filename ;

		if ( argc > 1 )
			filename = argv[1] ;
		else
			filename = "multiline.ini" ;

		parser.setMultilineEnabled() ;

		parser.load(filename,ini) ;
		dump(ini) ;

		retval = 0 ;
	}
	catch (const std::exception &e ) {
		std::cerr << e.what() << std::endl ;
	}
	catch(...) {
		std::cerr << "Exception!" << std::endl ;
	}

	return retval ;
}
