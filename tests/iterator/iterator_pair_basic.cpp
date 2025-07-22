/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/iterator.hpp>

#include <vector>

void body()
{
    std::vector<int> v{3, 4, 5, 6, 7};

    for (auto it = apl::beginRangeIterator(v); it; ++it) {
        con_.print(" %d", *it);
    }

    con_.n();
}
