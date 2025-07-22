/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/buffer.hpp>

struct C {
    C() { con_.println("C()"); }
    ~C() { con_.println("~C()"); }

    C(const C&) { con_.println("C(&)"); }

    C& operator=(const C&)
    {
        con_.println("C=(&)");
        return *this;
    }

    C(C&& o) { con_.println("C(&&)"); }
};


void body()
{
    apl::FixedBuffer<C, 5> b;

    b.resize(1);

    auto c = b;

    auto d = std::move(b);

    c.clear();
}
