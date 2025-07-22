/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../common/test_main.h"

#include <apeal/random.hpp>

void body() {
    using namespace apl;

    Random rand;

    con_("min: %d", rand.min());
    con_("max: %d", rand.max());

    std::pair<int, int> range(-20, 300);
    int intVal{};
    rand.range(range.first, range.second, &intVal);

    con_("range(%d, %d) -> %d", range.first, range.second, intVal);

    std::pair<double, double> ranged(-0.0001, 0.0003);
    double doubleVal{};
    rand.range(ranged.first, ranged.second, &doubleVal);

    con_("ranged(%g, %g) -> %g", ranged.first, ranged.second, doubleVal);
}
