/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>
#include <test_main.h>

#include <apeal/crc/adler.hpp>

void body ()
{
	using namespace apl ;
	using namespace std ;
  
	CrcAdler32 crc ;
  
	for ( int i = 1 ; i < cmdlineArgC_ ; i++ ) {
		cout.width(8) ;
		cout.fill('0') ;
		cout << hex << crc.load(cmdlineArgV_[i]) << ' ' << cmdlineArgV_[i] << endl ;
	}

}


