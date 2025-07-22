/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <string>

#include <apeal/string.hpp>
#include <apeal/str/format.hpp>


using namespace apl ;
using namespace std ;

class CustomType {
public :
    int intValue ;
    double floatValue ;
    string stringValue ;
} ;

template <class S, class F>
void str_format_type ( S &str, F &fmt, const F &fmtEnd, const CustomType &ct )
{
    ++fmt ;  // skip the type character
    apl::str_format_append ( str, "i:%d, f:%g, s:%s", ct.intValue, ct.floatValue, ct.stringValue ) ;
}



void body()
{
    int retcode = 1 ;

    auto ct = CustomType { 5, 0.4, "foo" } ;

    cout << apl::fmt ( "ct: |%c|", ct ) ;
}
