/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <stdio.h>
#include <test_main.h>

#include <iostream>
#include <string>
#include <vector>


#include <apeal/hash/md5.hpp>

using namespace apl;

struct MD5TestTable {
    std::string_view str;
    std::string_view md5;
};

static MD5TestTable testTable[] ={
    { "", "d41d8cd98f00b204e9800998ecf8427e" },
    { "a", "0cc175b9c0f1b6a831c399e269772661" },
    { "abc", "900150983cd24fb0d6963f7d28e17f72" },
    { "message digest", "f96b697d7cb7938d525a2f31aaf161d0" },
    { "abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b" },
    { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "d174ab98d277d9f5a5611c2c9f419d9f" },
    { "12345678901234567890123456789012345678901234567890123456789012345678901234567890", "57edf4a22be3c955ac49da2e2107b67a" },
};


void body()
{

    uint8_t digest[16];

    for (auto& t: testTable) {
        hash_MD5(t.str, digest);

        auto str = format_MD5(digest);

        std::cout << str << std::flush;

        if (str != t.md5) {
            std::cout << "  <--- FAIL !!!!" << std::endl;
            retCode_ = 1;
            break;
        }

        std::cout << std::endl;
    }

    if (!retCode_)
        std::cout << std::endl << "+++ SUCCESS +++" << std::endl;

}


