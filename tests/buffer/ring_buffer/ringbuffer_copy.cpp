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
    // non-trivially copyble
    auto bs = apl::RingBuffer<std::string, 5>{"3", "4", "5"};
    auto cs = bs;

    dumpBuffer(bs);
    dumpBuffer(cs);

    bs += "6", bs += "7", bs += "8";
    auto ds = bs;
    dumpBuffer(bs);
    dumpBuffer(ds);

    bs += "9", bs += "10", bs += "11";
    auto es = bs;
    dumpBuffer(bs);
    dumpBuffer(es);

    bs.pop();
    auto fs = bs;
    dumpBuffer(bs);
    dumpBuffer(fs);
}
