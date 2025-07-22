/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/str/join.hpp>

#include <vector>


void body() {
    using namespace apl;

    std::vector<String> vecStr = {"a", "b", "c", "d", "e", "f"};

    auto s = str_join(vecStr, ',', [](auto& str) { return str; });
    con_(s);

    s = str_join(vecStr, ',');
    con_(s);

    std::vector vecInt = {1, 2, 3, 4, 5};

    s = str_join(vecInt, ',');
    con_(s);
}
