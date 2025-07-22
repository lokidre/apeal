/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>
#include <stdexcept>
#include <string>

#include <apeal/parse/csv.hpp>

#include "dump_csv.hpp"


int main ( int argc, char *argv[] )
{
	using namespace apl ;
	using namespace std ;

	int retcode = 1 ;

	try {
		if ( argc <= 1 )
			throw runtime_error("Required parameter missing") ;

		auto csv = loadCsv ( argv[1] ) ;

		dumpCsv ( csv ) ;

		retcode = 0 ;
	}
	catch(exception &e) {
		cerr << e.what() << endl ;
	}
	catch(...) {
		cerr << "exception!" << endl ;
	}

	return retcode ;
}
