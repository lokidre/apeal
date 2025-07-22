/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../common/main.hpp"

#include <apeal/debug.hpp>

void body()
{
	debug_print("Printing debugging information...") ;

	debug_print("Prining formatted %s", "debugging information" ) ;

	debug_print("Prining formatted plain") ;


	debug_print_path("With file line") ;
	debug_print_file("With file line formatted: %s", "some info") ;

	debug_funcv() ;
	debug_func("%i, %s", 5, "sfdsfd") ;
}


#include "../common/main.cpp"


