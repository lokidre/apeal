/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <iostream>
#include <string>

#include <apeal/file.hpp>
#include <apeal/hash/md5.hpp>


using namespace apl;


void body()
{

        if (cmdlineArgC_ <= 1)
            throw std::runtime_error("usage: md5gen <file>...");

        auto bufferSize = 9048;
        auto buffer = std::make_unique<uint8_t[]>(bufferSize);

        for (int iarg = 1; iarg < cmdlineArgC_; iarg++) {
            std::string filename{cmdlineArgV_[iarg]};

            HashMD5 md5;
            uint8_t hash[HashMD5::hashSize];
            File file;
            file.open(filename, File::AccessMode::Read);

            md5.init();

            for (;;) {
                if (auto count = file.read(buffer.get(), bufferSize); count > 0)
                    md5.update(buffer.get(), count);
                else
                    break;
            }

            md5.final(hash);

            std::cout << format_MD5(hash) << " *" << filename << std::endl;
        }



}

