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

#include <apeal/thread.hpp>
#include <apeal/time.hpp>


[[noreturn]] void thread_func_1() {
    for (;;) {
        con_("%s(): %s", __func__, dateTimeMsecText());
        apl::sleep_sec(1);
    }
}

[[noreturn]] void thread_func_2() {
    for (;;) {
        con_("%s(): %s", __func__, dateTimeMsecText());
        apl::sleep_sec(2.5);
    }
}

void body() {
    con_.setAtomic(true);

    con_("Type 'quit' to quit...");

    apl::Thread th1{thread_func_1}, th2{thread_func_2};

    std::string s;
    do {
        std::cin >> s;
    } while (s != "quit");
}
