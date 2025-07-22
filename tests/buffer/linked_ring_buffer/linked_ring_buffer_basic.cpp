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

#include <apeal/buffer/linked_ring_buffer.hpp>

void body()
{
    apl::LinkedRingBuffer<int> buffer;

    buffer.push(2);

    // auto begin = buffer.begin();
    // auto end = buffer.end();
    //
    // con_("%d", *begin);
    //
    // ++begin;

    dumpBuffer(buffer, "2");

    buffer.push(3);
    dumpBuffer(buffer, "2, 3");

    buffer.pop();

    dumpBuffer(buffer, "3");

    buffer.push(4);
    buffer.push(5);
    buffer.push(6);
    dumpBuffer(buffer, "3, 4, 5, 6");

}
