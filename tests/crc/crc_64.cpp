/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>
#include "vector"
#include <apeal/crc/crc64.hpp>
#include <apeal/file.hpp>





void body() {
    using namespace apl;
    // cmdlineVector_.emplace_back("a");
    std::vector<unsigned char> aVector;
    aVector.emplace_back('a');
    aVector.emplace_back('a');
    aVector.emplace_back('a');
    aVector.emplace_back('a');
    aVector.emplace_back('a');
    aVector.emplace_back('a');
    aVector.emplace_back('a');
    aVector.emplace_back('a');
 //   verify(!cmdlineVector_.empty(), "Required parameter missing");
  //  verify(!cmdlineVector_.empty(), "Required parameter missing");

    //    for (auto& filename: cmdlineVector_) {
    //   auto data = loadFile(filename);
    auto data = aVector;

    auto crc = Crc64::compute_data(data);


    con_.println("%08x %s", crc, aVector);

//}
}
