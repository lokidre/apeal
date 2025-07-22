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
    apl::RingBuffer<int> buffer;
    buffer.reserve(4);

    buffer += 2;
    buffer += 3;

    buffer.pop();
    dumpBuffer(buffer);

    buffer += 4;
    buffer += 5;
    buffer.pop();
    buffer += 6;
    buffer += 7;
    buffer.pop();
    dumpBuffer(buffer);

    buffer.reserve(6);

    dumpBuffer(buffer);
}
