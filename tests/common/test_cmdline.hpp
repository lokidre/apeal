/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "./test_main.h"

#include <apeal/cmdline.hpp>


inline void dumpCmdline(const apl::CommandLine& cmdline)
{
    // Print original command line
    con_.print("cmdline:");
    for (auto& arg: cmdline.args) {
        con_.print(" %s", arg.orig);
    }
    con_.n();

    // Print command line info
    for (auto& arg: cmdline.args) {
        if (arg.op) {
            auto dash = arg.sh ? ' ' : '-';
            auto equ = arg.value.empty() ? ' ' : '=';
            con_("o: %c-%s%c%s", dash, arg.name, equ, arg.value);
        } else {
            con_("arg: %s", arg.value);
        }
    }

}
