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

#include <test_buffer.hpp>

struct C {
    C() noexcept {  con_("C()") ; }
    ~C() noexcept {  con_("~C()") ; }

    C(const C&) noexcept {  con_("C(&)") ;  }

    auto &operator=(const C&) noexcept {
        con_("C=(&)");
        return *this;
    }


    C(C&&o) noexcept { con_("C(&&)") ; }
} ;

void body()
{
    apl::Buffer<C> b ;

    b.resize(1) ;

    // dumpBuffer(b, "b");

    auto c = b;

    // dumpBuffer(c, "c");
    // dumpBuffer(b, "b");

    auto d = std::move(b);

    c.clear();

    // dumpBuffer(d);
}
