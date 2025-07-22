/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/crc/crc8.hpp>
#include <apeal/file.hpp>
#include <iostream>
#include <vector>


void body()
{
    using namespace apl;

    Crc8 crc8;
    cmdlineVector_.emplace_back("Hello world");
    verify(!cmdlineVector_.empty(), "Required parameter missing");

    for (auto& filename: cmdlineVector_) {
//        auto data = loadFile(filename);
auto data = cmdlineVector_;

        auto crc = crc8.compute_data(data);


        con_.println("%02x %s", crc, filename);
    }
}
