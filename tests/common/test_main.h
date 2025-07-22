/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <exception>
#include <vector>
#include <string_view>


#include <apeal/console.hpp>
#include <apeal/time.hpp>
#include <apeal/verify.hpp>

#include "./test_console.hpp"


extern int cmdlineArgC_;
extern char **cmdlineArgV_;


extern std::vector<std::string_view> cmdlineVector_;

extern int retCode_;


void body();


// Timing functions
inline apl::String dateTimeMsecText() {
    return apl::formatDateTimeMsec(apl::getLocalTime());
}


// Command line functions
inline
std::string_view requireCmdlineArg() {
    apl::verify(std::ssize(cmdlineVector_) >= 1, "Cmdline argument missing");

    return cmdlineVector_[0];
}

inline
std::vector<std::string_view> requireCmdlineArgs(apl::Index minArgs) {
    apl::verify(std::ssize(cmdlineVector_) >= minArgs, "Cmdline argument missing");

    return cmdlineVector_;
}

