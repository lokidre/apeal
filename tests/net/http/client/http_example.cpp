/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/file.hpp>
#include <apeal/parse.hpp>
#include <apeal/socket.hpp>
#include <apeal/http.hpp>

#include <iostream>


int main()
{
	using namespace std ;
	using namespace apl ;

	try {
		AHttp<ASocket> http ;

		http.get("http://www.example.com/") ;

		cout << http.response.body << endl ;
	}
	catch(exception &e) {
		cerr << e.what() << endl ;
	}
	catch(...) {
		cerr << "exception!" << endl ;
	}

	return 0 ;
}

