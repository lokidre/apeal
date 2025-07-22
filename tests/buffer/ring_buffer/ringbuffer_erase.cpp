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
    auto b = apl::RingBuffer<int, 5>{3, 4, 5};
    dumpBuffer(b);

    auto c = b;
    c.erase(c.beginIndex());
    dumpBuffer(c, "erase(beginIndex())");

    c = b;
    c.erase(c.begin());
    dumpBuffer(c, "erase(begin())");


    c = b;
    c.erase(c.nextIndex(c.beginIndex()));
    dumpBuffer(c, "erase(c.nextIndex(c.beginIndex()))");

    c = b;
    auto it = c.begin();
    c.erase(++it);
    dumpBuffer(c, "erase(begin() + 1)");


    c = b;
    c.cut(c.nextIndex(c.beginIndex()));
    dumpBuffer(c, "cut(c.nextIndex(c.beginIndex()))");

    c = b;
    it = c.begin();
    //++it;
    c.cut(++it);
    dumpBuffer(c, "cut(begin() + 1)");
}
