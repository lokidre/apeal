/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>

int main ( int argc, char *argv[] ) 
{
	std::cout << std::endl << "args[" << argc << "]: " << std::endl ;

	for ( int i = 0 ; i < argc ; i++ )
		std::cout << "    argv[" << i << "] = " << argv[i] << std::endl ;

	return argc ;
}

