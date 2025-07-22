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
	Signal<int> i = 0 ;

	//i = false ;


	C c ;

	i.connect ( &C::i, &c ) ;
	i(2) ;

	i.disconnect() ;

//	i.f = [](int i){  apl::println("[](%d)",i) ;  } ;
    i.func = [](int i){  apl::println("[](%d)",i) ;  } ;

	auto ii = std::move(i) ;

	std::swap ( i, ii ) ;

	i(3) ;

	

	//scope() ;
	//g("Sample string") ;

}




