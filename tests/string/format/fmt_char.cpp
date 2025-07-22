/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/assert.hpp>
#include <apeal/debug.hpp>


void test(std::string_view fmt, char arg) {
    auto&& s = apl::fmt(fmt, arg);

    apl::println(R"(fmt("%s", '%c'): "%s")", fmt, arg, s);
}

void body() {
    test("%c", '$');

    test("123456%c", '$');

    test("1%%23456%c", '$');

    test("%c1%%23456", '$');

    test("1%%23456 %c 9886%%40", '$');
}
