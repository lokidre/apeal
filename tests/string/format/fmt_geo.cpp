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

#include <apeal/geo.hpp>

#include <typeinfo>


template<class T>
static void test(std::string_view fmt, const T& arg) {
    auto&& text = apl::fmt(fmt, arg);

    apl::println("fmt('%s'): %s", fmt, text);
}


void body() {
    using namespace apl;

    //auto b = std::is_reference_v<ARect<double>::P>;
    //auto a = std::is_reference_v<ARect<int>::PS>;

    //println("Point<int>: %s", typeid(APoint<int>::P).name());
    //println("Point<double>: %s", typeid(APoint<double>::P).name());
    //println("Point<double>: %s", typeid(APoint<long double>::P).name());

    //auto a = sizeof(double);
    //auto b = sizeof(void *);

    test("%z", Point<int>{23, 43});
    test("%|x|z", Size<int>{45, 82});

    test("Rectangle: %.2| >X< |z", Rect<double>{5.33, 82.34, 300, 440.34});
}
