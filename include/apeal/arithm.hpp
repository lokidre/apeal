/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

// TODO: cleanup

#include "def.hpp"

#include <cstdint>

namespace apl {

template <class T, class U>
constexpr T inc(const T& num, const U& rng) {
    return (num < rng - 1) ? num + 1 : 0;
}

template <class T, class U>
T dec(const T& num, const U& rng) {
    return (num) ? num - 1 : rng - 1;
}


template <class T>
T rdiv(const T& divident, const T& divisor) {
    return divident / divisor + (divident % divisor >= 5 ? 1 : 0);
}

template <class T>
T adiv(const T& divident, const T& divisor) {
    return divident / divisor + (divident % divisor ? 1 : 0);
}


// Absolute value
// needed some better realization
// template <class T>
// inline T abs ( const T &x ) {  return (T)::fabs(x) ; }

// specializations
// template <>
// inline char abs ( const char &x ) {  return x < 0 ? -x : x ;  }

// template <>
// inline int abs ( const int &x ) {  return x < 0 ? -x : x ;  }


// Rounding
template <typename type>
inline type Round(type x, int n = 0) {
    // modify a bit...
    if (n < 0)
        x *= -n * 10;
    else if (n > 0)
        x /= n * 10;

    // get first digit of mantissa
    long int m = (long int)(x * 10) % 10;

    // make result
    if (m >= 5)
        x += 1;

    // zero mantissa
    x = (long int)x;

    if (n < 0)
        x /= -n * 10;
    else if (n > 0)
        x *= n * 10;

    return x;
}


// Alignment
inline uint32_t align2(const uint32_t n) { return (n & 0x1) ? (n + 2) & ~0x1 : n; }

inline uint32_t align4(const uint32_t n) { return (n & 0x3) ? (n + 4) & ~0x3 : n; }

inline uint32_t align8(const uint32_t n) { return (n & 0x7) ? (n + 8) & ~0x7 : n; }

template <class T>
T align16(const T& n) {
    return (n & 0xF) ? (n + 16) & ~0xF : n;
}

template <class T>
T align128(const T& n) noexcept {
    return (n + 128 - 1) & ~0x7F;
}


// inline uint32_t align ( const uint32_t n, const int m )
//{
//  return ( n & (~uint32_t(0)<<m) )? (n + m) & ~(~uint32_t(0)<<m) : n ;
//}


} // namespace apl
