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
  
	int ret_code = 1 ;

	try {

		if ( argc <= 1 )
			throw runtime_error("usage: csv_torture <datafile>") ;

		ACsvParser parser ;
		vector<vector<string> > data ;
		string filename = argv[1] ;

		parser.load(filename,data) ;

		int rows(0), cols(0), total(0), value ;

		for ( auto &row: data ) {
			cols = 0 ;
			for ( auto &col: row ) {
				value = 0 ;
				if ( !parse_int(col,value) )
					throw runtime_error(string("cannot parse value: ") + col ) ;
				total += value ;
				cols++ ;
			}
			rows++ ;
		}

		cout << cols << " " << rows << " " << total << endl ;


		ret_code = 0 ;
	}
	catch(exception &e) {
		cerr << e.what() << endl ;
	}
	catch(...) {
		cerr << "exception!" << endl ;
	}

	return ret_code ;
}


