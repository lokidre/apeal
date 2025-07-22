/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>
#include <chrono>
#include <ctime>

#include <test_main.h>

#include <apeal/hash/sha256.hpp>

using namespace apl;

void body()
{
        if (cmdlineArgC_ <= 1)
            throw std::runtime_error("usage: sha256gen <file>...");

        auto bufferSize = 9048;
        auto buffer = std::make_unique<uint8_t[]>(bufferSize);

        for (int iarg = 1; iarg < cmdlineArgC_; iarg++) {
            std::string filename{cmdlineArgV_[iarg]};

            HashSHA256 sha256;
            uint8_t hash[HashSHA256::hashSize];
            File file;
            file.open(filename, File::AccessMode::Read);

            sha256.init();

            for (;;) {
                if (auto count = file.read(buffer.get(), bufferSize); count > 0)
                    sha256.update(buffer.get(), count);
                else
                    break;
            }

            sha256.final(hash);

            std::cout << format_SHA256(hash) << " *" << filename << std::endl;
        }


}

