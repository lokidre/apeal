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

#include <apeal/file.cxx>

int main()
{
	using namespace apl ;
	using namespace std ;

	std::string stemp ;
	AFileSystem fs ;

	try {
		/*
		fs.pwd(stemp) ;
		cout << "current directory: " << stemp << endl ;

		cout << "enter new directory: "  ;
		cin >> stemp ;

		fs.pwd(stemp) ;
		cout << "current directory: " << stemp << endl ;
		*/

		stemp = "aaaaaaa" ;
		fs.mkdir(stemp,fs.mkdir_parents) ;

    		// fs.mkdir("stemp") ;
		cout << "creating " << stemp << "..." << endl ;

	}
	catch(exception &e) {
		cerr << e.what() << endl ;
	}
	catch(...) {
		cerr << "exception!" << endl ;
	}

	return 0 ;
}
