/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/parse/xml.hpp>
#include <apeal/file.hpp>

#include <string>

int main ( int argc, char *argv[] )
{
	using namespace std ;
	using namespace apl ;

	int ret = 1 ;

	try {
		if ( argc <= 1 )
			throw runtime_error("Argument missing") ;

		AXmlParser parser ;
		AXml xml ;
		parser.load(argv[1],xml) ;

		ret = 0 ;
	}
	catch(exception &e) {
		cerr << e.what() << endl ;
	}
	catch(...) {
		cerr << "exception!" << endl ;
	}

	return ret ;

}

