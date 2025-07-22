/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>
#include <test_buffer.hpp>

#include <apeal/noncopyable.hpp>

#include <apeal/buffer.hpp>


struct D: apl::NonCopyable {};

struct C {
    // Non copyable
    constexpr C() = default;
    ~C() = default;

    C(const C&) = delete;
    C& operator=(const C&) = delete;

    // Movable
    C(C&& other) noexcept {
        con_.println("C(&&)");
    }

    C& operator=(C&& other) noexcept {
        con_.println("=C(&&)");
        return *this;
    }

};



void body()
{
    apl::Buffer<C> buffer;
    apl::Buffer<D> duffer;

    auto& a = buffer.emplace_back();

    buffer.emplace_back();

    dumpBufferInfo(buffer);

    auto& dd = buffer.emplace_back();

    dumpBufferInfo(buffer);
}

