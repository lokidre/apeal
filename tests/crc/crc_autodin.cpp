/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>
#include <string>
#include <test_main.h>

#include <apeal/crc/autodin.hpp>
#include <apeal/str/format.hpp>

void body ()
{
	using namespace apl ;
	using namespace std ;
  
	//ACrcAutodin32 crc ;
	string v, s ;
	uint32_t crc ;

	for ( int i = 1 ; i < cmdlineArgC_ ; i++ ) {
		v = cmdlineArgV_[i] ;
		crc = crc_autodin32(v) ;
        cout << str_format(s,"%08x",crc) << " - " << crc << endl ;
		long acrc = crc ;
		if ( acrc < 0 )
			acrc = -acrc ;
		cout << acrc << endl ;
	}
  
}



/*
CRC32


echo hash('crc32', ''), "\n";
echo hash('crc32', 'a'), "\n";
echo hash('crc32', 'abc'), "\n";
echo hash('crc32', 'message digest'), "\n";
echo hash('crc32', 'abcdefghijklmnopqrstuvwxyz'), "\n";
echo hash('crc32', 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'), "\n";
echo hash('crc32', '12345678901234567890123456789012345678901234567890123456789012345678901234567890'), "\n";
echo hash('crc32b', ''), "\n";
echo hash('crc32b', 'a'), "\n";
echo hash('crc32b', 'abc'), "\n";
echo hash('crc32b', 'message digest'), "\n";
echo hash('crc32b', 'abcdefghijklmnopqrstuvwxyz'), "\n";
echo hash('crc32b', 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'), "\n";
echo hash('crc32b', '12345678901234567890123456789012345678901234567890123456789012345678901234567890'), "\n";

--EXPECT--
00000000
6b9b9319
73bb8c64
5703c9bf
9693bf77
882174a0
96790816
00000000
43beb7e8
c2412435
7f9d1520
bd50274c
d2e6c21f
724aa97c

*/
