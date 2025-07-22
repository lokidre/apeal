/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <vector>

#include <apeal/string.hpp>
#include <apeal/str/escape.hpp>

void body()
{
    using std::vector;

    using namespace apl;

    String sample = "escape %, asterisk *, excl !";
    auto escaped = sescape(sample, '%', "%*!", "%*!");
    auto unescaped = sunescape(escaped, '%', "%*!", "%*!");

    con_("|%s| => |%s| => |%s|", sample, escaped, unescaped);


    escaped = sescape(sample, '@', vector<char>{'%', '*', '!'},
                      vector<String>{"(percent)", "(asterisk)", "(exclamation)"});
    unescaped = sunescape(sample, '@', vector<char>{'%', '*', '!'},
                          vector<String>{"(percent)", "(asterisk)", "(exclamation)"});

    con_("|%s| => |%s| => |%s|", sample, escaped, unescaped);
}
