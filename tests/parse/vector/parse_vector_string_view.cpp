/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../../common/test_main.h"

#include <apeal/assert.hpp>
#include <apeal/parse/vector.hpp>

struct ParseTestData {
    std::string_view s;
    std::string_view f;
};


ParseTestData testData[] = {
    {"1,2", "1,2"},
    {"true,4,4", "true,4,4"},
};

void body() {
    using namespace apl;

    for (auto&& t: testData) {
        auto r = parse_std_vector_view_trim(t.s, ',');

        //verify(r == t.b, "Parse failed: %s != %s", t.s, (r ? "true" : "false"));

        auto&& f = fmt("%s", r);

        println("%s: %s", t.s, f);
    }
}
