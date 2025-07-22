/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "rect.hpp"

namespace apl {

template <class T = int>
struct Margins {
    using Self = Margins;
    using Arg = const Margins&;

    T t, r, b, l;  // Top, right, bottom left
    T a;  // All margins
    T h, v;  // Horizontal, vertical


    [[nodiscard]] constexpr T top() const noexcept { return std::max({a, v, t}); }
    [[nodiscard]] constexpr T right() const noexcept { return std::max({a, h, r}); }
    [[nodiscard]] constexpr T bottom() const noexcept { return std::max({a, v, b}); }
    [[nodiscard]] constexpr T left() const noexcept { return std::max({a, h, l}); }

    [[nodiscard]] constexpr T horz2() const noexcept { return left() + right(); }
    [[nodiscard]] constexpr T vert2() const noexcept { return top() + bottom(); }

    void chop() noexcept {
        t += a + v;
        r += a + h;
        b += a + v;
        l += a + h;
        a = h = v = 0;
    }
    void setTop(T val) noexcept { chop(); t = val; }
    void setRight(T val) noexcept { chop(); r = val; }
    void setBottom(T val) noexcept { chop(); b = val; }
    void setLeft(T val) noexcept { chop(); l = val; }

    void addTop(T val) noexcept { setTop(top() + val); }
    void addRight(T val) noexcept { setRight(right() + val); }
    void addBottom(T val) noexcept { setBottom(bottom() + val); }
    void addLeft(T val) noexcept { setLeft(left() + val); }

    void subTop(T val) noexcept { setTop(top() - val); }
    void subRight(T val) noexcept { setRight(right() - val); }
    void subBottom(T val) noexcept { setBottom(bottom() - val); }
    void subLeft(T val) noexcept { setLeft(left() - val); }


    [[nodiscard]]
    constexpr Rect<T> outerRect(const Rect<T>& rect) const noexcept {
        return {
            rect.x - left(),
            rect.y - bottom(),
            rect.w + left() + right(),
            rect.h + top() + bottom(),
        };
    }

    // Self &scale(T s) noexcept {
    //     top *= s;
    //     right *= s;
    //     bottom *= s;
    //     left *= s;
    //     return *this;
    // }

    // Self &operator *= (T s) { scale(s);  return *this; }
    //
    // void operator=(T padding) { top = right = bottom = left = padding; }

    friend constexpr Self operator / (Arg ia, T ib) noexcept {
        return {
            .t = ia.t / ib,
            .r = ia.r / ib,
            .b = ia.b / ib,
            .l = ia.l / ib,
            .a = ia.a / ib,
            .h = ia.h / ib,
            .v = ia.v / ib,
        };
    };

    friend constexpr Self operator * (Arg ia, T ib) noexcept {
        return {
            .t = ia.t * ib,
            .r = ia.r * ib,
            .b = ia.b * ib,
            .l = ia.l * ib,
            .a = ia.a * ib,
            .h = ia.h * ib,
            .v = ia.v * ib,
        };
    };
};

}  // namespace apl
