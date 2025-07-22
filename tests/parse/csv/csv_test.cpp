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

#include <apeal/parse/csv.hpp>


int main ( int argc, char *argv[] )
{
	using namespace apl ;
	using namespace std ;
  
	int retCode = 1 ;

	try {
		if ( argc <= 1 )
			throw runtime_error("Required parameter missing") ;

		string filename = argv[1] ;

		ACsvParser csvParser ;
		vector<vector<string>> data ;
		string text ;

		csvParser.load(filename,data) ;

		cout << "rows: " << data.size() << endl ;

		if ( !data.empty() )
			cout << "columns: " << data[0].size() << endl ;

		csvParser.generate(data,text) ;
		cout << text ;

		retCode = 0 ;
	}
	catch(exception &e) {
		cerr << e.what() << endl ;
	}
	catch(...) {
		cerr << "exception!" << endl ;
	}

	return retCode ;
}

