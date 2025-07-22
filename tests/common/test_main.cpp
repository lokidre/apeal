/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "./test_main.h"

#include <iostream>

int cmdlineArgC_{};
char **cmdlineArgV_{};

std::vector<std::string_view> cmdlineVector_{};

int retCode_{};

apl::Console con_{};



int main(int argc, char* argv[])
{
    try {
        cmdlineArgC_ = argc;
        cmdlineArgV_ = argv;

        for (auto i = 1; i < argc; ++i)
            cmdlineVector_.emplace_back(argv[i]);

        body();
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        retCode_ = 1;
    }
    catch (...) {
        std::cout << "Exception!" << std::endl;
        retCode_ = 2;
    }

    return retCode_;
}
