/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/file.hpp>

using namespace apl ;
using namespace std ;


int main ( int argc, char *argv[] )
{
	int retcode = 1 ;

	try {
		AFileWriteFormat f ( "test.md" ) ;

		f.line ( "Formatted output test" ) ;
		f.ch ( '-', 20 ).n() ;
		f.write ( "Begin. int=%d, double=%g\n", 4, 4.5 ) ;
		f.t().line("Tab") ;
		f.t(2).line("Two Tabs") ;
		f.line ( "Done." ) ;
  
		retcode = 0 ;
	}
	catch ( exception &e ) {
		cerr << e.what() << endl ;
	}
	catch(...) {
		cerr << "exception!" << endl ;
	}

	return retcode ;
}

