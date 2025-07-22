/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/assert.hpp>
#include <apeal/parse/bool.hpp>

struct ParseTestData {
    std::string_view s;
    bool b;
};


ParseTestData testData[] = {
    {"1",       true},
    {"0",       false},
    {"true",    true},
    {"TRUE",    true},
    {"false",   false},
    {"False",   false},
    {"Yes",     true},
    {"no",      false},
};

void body()
{
    using namespace apl;

    for (auto&& t: testData) {
        auto r = parseBool(t.s);

        verify(r == t.b, "Parse failed: %s != %s", t.s, (r ? "true" : "false"));

        Console::log("%s: %s", t.s, t.b);
    }
}

