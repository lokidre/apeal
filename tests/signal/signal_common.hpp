/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <apeal/console.hpp>

struct C {
	void v() {  apl::println("C::v()") ;  }
	void i(int i) {  apl::println("C::i(%d)", i) ;  }
	void s(std::string s) {  apl::println("C::s(%d)", s) ;  }

	void iref(int &i) {  apl::println("C::iref(%d)", i) ;  i = i + 1 ;  }
	void sref(int &s) {  apl::println("C::iref(%s)", s) ;  s += '.' ;  }
} ;

