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
    apl::FixedBuffer<int, 5> buffer;

    buffer.push_back(2);

    dumpBuffer(buffer, "push_back(2)");

    buffer.push_back(3);

    buffer.push_back(4);

    dumpBuffer(buffer, "push_back({3, 4})");

    buffer.pop_back();
    dumpBuffer(buffer, "pop_back()");

    buffer.pop_back();
    dumpBuffer(buffer, "pop_back()");

    buffer.pop_back();
    dumpBuffer(buffer, "pop_back()");
}
