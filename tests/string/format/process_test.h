/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <array>
#include <apeal/str/format.hpp>

template <class T>
struct FormatElement {
	int index ;
	const char *fmt ;
	T val ;
} ;


template <class T>
int processTest ( const FormatElement<T> *fmt, int size )
{
	using namespace apl ;

	char buffer[256] ;
	std::string str ;

	// integer tests
	auto t = fmt ;
	for ( int idx = 0 ; idx < size ; idx++, t++ ) {

		str_format ( str, t->fmt, t->val ) ;
		snprintf  ( buffer, sizeof(buffer)-1, t->fmt, t->val ) ;


		std::cout << "(\"" << t->fmt << "\", " << t->val << ") --> \t'" << str << "'" ;

		// bring the sprintf equivalent
		std::cout << ", \t sprintf --> '" << buffer << "'" ;

		if ( str != buffer ) {
            std::cerr << "error:" << t->index << std::endl ;
			str_format ( str, t->fmt, t->val ) ;
			return 1 ;
		}

        std::cout << std::endl ;
  }

  return 0 ;
}

