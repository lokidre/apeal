/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/str/trim.hpp>

using namespace std ;
using namespace apl ;


void body() {
    string test = "  A sample string to trim.  ";

    if (cmdlineArgC_ > 1)
        test = cmdlineArgV_[1];

    cout << '|' << str_trim(test) << '|' << endl;
    cout << '|' << str_trim(test) << '|' << endl;
}
