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
    dumpBuffer([] {
        apl::RingBuffer<int, 5> bb = {8, 7, 78};
        return bb;
    }());
}
