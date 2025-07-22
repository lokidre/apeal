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

#include <apeal/mutex.hpp>
#include <apeal/thread.hpp>
#include <apeal/time.hpp>
#include <apeal/console.hpp>

#include <typeinfo>

//#include "console_out.hpp"

using namespace apl ;
using namespace std ;


//AMutex consoleMutex ;


class Class1 {
public :
	void show()
	{
		for(;;) {
			//console_ ( "Class1::show()" ) ;
			console_ ( "%s::%s()", typeid(*this).name(), __func__ ) ;
			sleep(1000) ;
		}
	}
} ;


void body()
{
//	consoleMutex.create() ;

	string s ;

	Class1 c ;
	//AThreadMeth<Class1> th ;

    console_ ( "Type 'quit' to quit..." ) ;

		Thread th ( &Class1::show, &c ) ;

		do { 
			cin >> s ;
		} while ( s != "quit" ) ;


}

