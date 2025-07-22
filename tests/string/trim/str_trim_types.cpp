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

#include <apeal/string.hpp>
#include <apeal/str/trim.hpp>

using namespace std ;
using namespace apl ;




void body ()
{
		cout << '|' << str_trim(" Abc\t") << '|' << endl ;
		cout << '|' << str_trim(string(" Bc d ")) << '|' << endl ;
		cout << '|' << str_trim("") << '|' << endl ;

}



