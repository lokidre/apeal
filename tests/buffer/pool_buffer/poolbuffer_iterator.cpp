/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>
#include <test_buffer.hpp>

#include <apeal/buffer.hpp>

void body()
{
    auto b = apl::PoolBuffer<int, 5>{3, 4, 5, 6, 7, 8, 9};

    dumpBuffer(b);

    for (auto&& d: b)
        con_("%d ", d);
    con_.nl();

    for (auto it = b.begin(), end = b.end(); it != end; ++it)
        con_.print("%d ", *it);
    con_.nl();
}
