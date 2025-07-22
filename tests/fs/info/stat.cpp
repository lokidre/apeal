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

#include <apeal/file.hpp>
#include <apeal/time.hpp>


int main ( int argc, char *argv[] )
{
	using namespace apl ;
	using namespace std ;

	try {

		AFileSystem fs ;
		AFileInfo fi ;
		string s ;

		string filename = "stat_test.cpp" ;

		if ( argc > 1 )
			filename = argv[1] ;


		fs.stat(filename,fi) ;

		cout << "ctime " << format_datetime(fi.creation_time,s) << endl ;
		cout << "mtime " << format_datetime(fi.write_time,s) << endl ;
		cout << "atime " << format_datetime(fi.access_time,s) << endl ;
		cout << "size  " << fi.size << endl ;

	}
	catch(exception &e) {
		cerr << e.what() << endl ;
	}
	catch(...) {
		cerr << "exception!" << endl ;
	}

	return 0 ;
}


