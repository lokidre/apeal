/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <string>
#include <iostream>
#include "../../common/test_main.h"

#include <apeal/file.hpp>

void body ()
{
	using namespace apl ;
	using namespace std ;

	string stemp ;
	AFileSystem fs ;


    for ( int iarg = 1 ; iarg < cmdlineArgC_ ; iarg++ ) {
        cout << "creating " << cmdlineArgV_[iarg] << endl ;
        fs.mkdir(cmdlineArgV_[iarg],fs.mkdir_parents) ;
    }


}
