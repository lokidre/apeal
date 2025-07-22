/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/console.hpp>
#include <apeal/assert.hpp>
#include <apeal/debug.hpp>


static void test(std::string_view fmt, double arg)
{
    auto&& text = apl::sformat(fmt, arg);

    con_("fmt(%s, %f): %s", fmt, arg, text);
}

void body()
{
    test("%f", 1.234);
}






