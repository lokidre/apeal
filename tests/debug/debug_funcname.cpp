/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../common/main.hpp"

#include <apeal/assert.hpp>

using namespace apl ;

class C {
public :
	C() {
		println(__FUNCTION__) ;
	}

	int f(int a, int b) {
		println(__FUNCTION__) ;
		return 0 ;
	}
} ;

template <class T>
struct S {
	void f(T) {
		println(__FUNCTION__) ;
	}
} ;

void body() 
{
	println(__FUNCTION__) ;

	C c ;

	c.f(1,2) ;

	S<C> s ;
	s.f(c) ;

	//auto cond = false ;

	//debug_verify ( cond, "Verifiying conditition debug %z", cond ) ;


	//apl::verify ( cond, "Verifiying conditition runtime %z", cond ) ;
}

#include "../common/main.cpp"

