/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../common/test_main.h"

#include <apeal/signal.hpp>

#include "signal_common.hpp"

using namespace apl ;
using namespace std ;


struct A: NonCopyable {
	int a{} ;
} ;

struct S {
	void a(A &a) { apl::println("C::a(%d)", a.a) ; }
} ;

Signal<A &> aref ;


void body()
{
	S s ;

	A a ;

	a.a = 5 ;

	aref.connect(&S::a, &s) ;

	aref(a) ;

}




