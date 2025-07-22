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

using namespace std ;
using namespace apl ;


int main ( int argc, char *argv[] )
{

	int retcode = 1 ;

	try {

		if ( argc < 3 )
			throw runtime_error("usage: http_download <url> <file>") ;

		AHttp<ASocket> http ;

		//http.request.accept_encoding = "gzip" ;
		//http.request.accept_encoding = "deflate" ;
		http.get(argv[1]) ;

		saveFile ( argv[2], http.response.body ) ;

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

