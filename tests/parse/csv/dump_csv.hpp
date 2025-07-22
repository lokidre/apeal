/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <test_main.h>

#include <string>

template<class C>
void dumpCsv(const C& csv) {
    apl::String s;

    for (auto& row: csv) {
        s.clear();

        for (auto& col: row) {
            if (!s.empty())
                s += ",";
            s += col;
        }
        con_(s);
    }
}
