/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/def.hpp>
#include <apeal/mutex.hpp>
#include <apeal/thread.hpp>
#include <apeal/time.hpp>

#include <apeal/console.hpp>

using namespace std ;
using namespace apl ;


class ThreadClass {
public :
	void thv()
	{
		for(;;) {
            console_ ( "%s::%s()", typeid(*this).name(), __func__ ) ;
			sleep(2000) ;
		}
	}

	void ths ( const string &s)
	{
		for(;;) {
            console_ ( "%s::%s(%s)", typeid(*this).name(), __func__, s ) ;
			sleep(2500) ;
		}
	}

	int thi ( int p1 )
	{
		for(;;) {
            console_ ( "%s::%s(%i)", typeid(*this).name(), __func__, p1 ) ;
			sleep(2700) ;
		}
		return 1 ;
	}
} ;

void body()
{
	console_ ( "Type 'quit' to quit..." ) ;
	string s ;

	ThreadClass cls ;

	Thread thv ( &ThreadClass::thv, &cls ) ;
	Thread ths ( &ThreadClass::ths, &cls, "string param" ) ;
	Thread thi ( &ThreadClass::thi, &cls, 5 ) ;

	do {
		std::cin >> s ;
	} while ( s != "quit" ) ;

}

