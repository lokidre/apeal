/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>
#include <cstdlib>

#include <apeal/def.hpp>


#if __OS == __OS_UNIX
#include <unistd.h>
#endif


int main ( int argc, char *argv[] ) 
{
	for ( char **penv = environ ; penv && *penv ; ++penv )
		std::cout << *penv << std::endl ;

	return 4 ;
}

