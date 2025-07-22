/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/console.hpp>
#include <apeal/assert.hpp>
#include <apeal/debug.hpp>
#include <test_main.h>


template <class T>
static void test(std::string_view fmt, const std::vector<T>& arg) noexcept
{
    auto&& s = apl::fmt(fmt, arg);

    apl::println("fmt(%s): %s", fmt, s);
}


void body()
{
    //test ( "%z", std::vector<int>({ 1, 3, 4, 55 }) ) ;

    //test ( "%10dz", std::vector<int>({ 1, 3, 4, 55 }) ) ;

    //test ( "%1.2z", std::vector<double>({ 1.2, 3., 4.545, 55.1 }) ) ;

    test(" \"%1.2|\" , \"|z\" ", std::vector<double>({1.2, 3., 4.545, 55.1}));

}



