/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/parse/csv.hpp>

#include "dump_csv.hpp"

auto source = R"(
a11,a22,a3333,a44

"b11","b22",b33,b44


c11, c2,   c3 , "c4")";


void body() {

    auto csv = apl::parseCsv(source);

    dumpCsv(csv);

}
