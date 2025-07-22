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



void body()
{
	//ASignal<int> i = 0 ;
	Signal<int &> iref ;
	Signal<std::string &> sref ;

	//i = false ;


	C c ;

	iref.connect ( &C::iref, &c ) ;

	auto i = 2 ;

	iref(i) ;

	iref.disconnect() ;

	iref = [](int &i){  apl::println("[](%d)",i) ; i+=2 ;  } ;

	iref(i) ;

	//auto ii = std::move(i) ;

	//std::swap ( i, ii ) ;

	iref.connect ( &C::iref, &c ) ;

	iref(i) ;

	

	//scope() ;
	//g("Sample string") ;

}




