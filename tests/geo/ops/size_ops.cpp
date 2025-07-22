/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "common_ops.hpp"


void body()
{
    using T = double;

    using Size = apl::Size<T>;

    auto a = Size{1.4, 2.7};
    auto b = Size{4.1, 5.3};

    printBinary(a, b, a + b, "+");
    printBinary(a, b, a - b, "-");
    printBinary(a, b, a * b, "*");
    printBinary(a, b, a / b, "/");

    auto s = 2.5;
    printBinary(a, s, a + s, "+");
    printBinary(a, s, a - s, "-");
    printBinary(a, s, a * s, "*");
    printBinary(a, s, a / s, "/");


    auto c = a;
    c += b;
    printBinary(a, b, c, "+=");

    c = a;
    c -= b;
    printBinary(a, b, c, "-=");

    c = a;
    c *= b;
    printBinary(a, b, c, "*=");

    c = a;
    c /= b;
    printBinary(a, b, c, "/=");



    c = a;
    c += s;
    printBinary(a, s, c, "+=");

    c = a;
    c -= s;
    printBinary(a, s, c, "-=");

    c = a;
    c *= s;
    printBinary(a, s, c, "*=");

    c = a;
    c /= s;
    printBinary(a, s, c, "/=");

}

