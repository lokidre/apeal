/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/debug.hpp>
#include <apeal/string.hpp>
#include <apeal/string.hpp>
#include <apeal/assert.hpp>
#include <apeal/str/trim.hpp>


#define TEST_VOID_BODY 1

using namespace apl ;

template <class S>
struct TestString {
	S s, d ;
} ; 


TestString<StrView> testStrings[] = {
	{ "", "" },
	{ "a", "a" },
	{ "   \t", "" },
	{ "   a", "a" },
	{ "   a b", "a b" },
	{ "a ", "a" },
	{ " a ", "a" },
} ;

void body()
{
	//AString u ;

	for ( auto &t: testStrings ) {
		//u = t.s ;
		//str_trim_update(u) ;
		auto u = str_trim(t.s);

		println("'%s' -> '%s'%s", t.s, u, (u == t.d ? "" : " --->Mismatch!!")) ;

		ADebugTrapAssert(u == t.d);
	}
}


