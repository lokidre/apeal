/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/logger.hpp>

#include <string>
#include <iostream>
#include <stdexcept>

struct CustomType {
    int intValue ;
    double floatValue ;
    std::string stringValue ;
} ;

template <class Str, class F>
void str_format_type(Str &str, F *fmt, F fend, const CustomType &ct)
{
    ++*fmt;  // skip the type character
    apl::str_format_append(str, "i:%d, f:%g, s:%s", ct.intValue, ct.floatValue, ct.stringValue);
}

void body()
{
    apl::Logger log{"test.log"};

    std::string message = "First message" ;
    log.write(message) ;

    log.write("second message");
    log << 2 ;
    log << CustomType{5, 0.4, "foo"};
    log.format("Formatted i:%d, d:%g", 5, 0.4);
}
