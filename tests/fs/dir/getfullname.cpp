/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/

#include "../../common/test_main.h"

#include <apeal/file.hpp>

void body() {
    using namespace apl;
    using namespace std ;

    using FS = apl::AFileSystem;

    FS fs;

//    auto currentdir = FS::getFullName(cmdlineArgC_);
//    println("Current Dir: %s", currentdir);

    for ( int iarg = 1 ; iarg < cmdlineArgC_ ; iarg++ ) {
        String fullname = fs.getFullName(cmdlineArgV_[iarg]) ;
        cout << "get full name " << fullname << endl ;
    }
}


