/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../types.hpp"
#include "../assert.hpp"
#include "../string.hpp"

#include <cstdlib>
#include <cmath>
#include <charconv>
#include <concepts>

namespace apl {

// Parses and checks floating point value for zero
// returns pair <is_zero, is_empty>
template <class I>
constexpr std::pair<bool, bool> parse_real_check_zero_basic(I it, I end) noexcept {

    for (; it != end; ++it) {
        if (*it != '\t' && *it != ' ')
            break;
    }

    if (it == end)
        return {true, true};

    for (; it != end; ++it)
        if (*it != '0' && *it != '.' && *it != ','
                && *it != 'e' && *it != 'E' && *it != ' ' && *it != '\t'
                && *it != '+' && *it != '-')
            break;

    return {it == end, false};
}


// Seems like only Microsoft implemented to/from_chars
// wait until clang/gcc implement it
// #ifdef _MSC_VER
//
// template <class I, class T>
// bool parse_real_it_body(I begin, I end, T &v) noexcept
// {
//     if (begin == end)
//         return false;
//
//     auto beginptr = &*begin;
//     auto endptr = beginptr + (end-begin);
//
//     //auto endptr = &*(end-1) + 1;  // because we cannot dereference end itself
//
//     auto [ptr, err] = std::from_chars(beginptr, endptr, v, std::chars_format::general);
//
//     if (ptr == &*begin || err != std::errc())
//         return false;
//
//     return true;
// }
//
// #else

template <class I>
bool parse_real_it_body(I it, I end, std::floating_point auto& v) noexcept
{
    auto zero = parse_real_check_zero_basic(it, end);

    if (zero.first) { // if zero, check for empty
        v = 0;
        return !zero.second;
    }


    // Waiting for android support of std::from_chars for floating point
#if defined(__ANDROID__) || defined(__APPLE__) || defined(_MSC_VER)
     // Inefficient, but it's remporary anyway until from_chars gets implemented properly
     String zs{it, end};

     auto dblVal = std::strtod(zs.c_str(), nullptr);
     v = static_cast<std::remove_reference_t<decltype(v)>>(dblVal);

     if (dblVal == 0 || dblVal == HUGE_VAL)
         return false;

#else
    auto result = std::from_chars(it, end, v, std::chars_format::general);
    if (result.ec != std::errc{})
        return false;
#endif

    return true;
}

// template <class I>
// bool parse_real_it_body(I it, I end, float &v) noexcept
// {
//     auto zero = parse_real_check_zero_basic(it, end);
//
//     if (zero.first) {   // if zero, check for empty
//         v = 0;
//         return !zero.second;
//     }
//
//     // Inefficient, but it's remporary anyway until from_chars gets implemented properly
//     String zs{it, end};
//
//     v = std::strtof(zs.c_str(), 0);
//
//     if (v == 0 || v == HUGE_VALF)
//         return false;
//
//     return true;
// }

// #endif



template <class I, class T>
bool parse_real_it(I begin, I end, T &v) noexcept {
    return parse_real_it_body(begin, end, v);
}

template <class T>
bool parse_real(StrView str, T &v) noexcept {
    //return parse_real_it(s.data(), s.data() + s.size(), v);
    return parse_real_it(str.begin(), str.end(), v);
}

//template <class T>
//bool parse_real(const char *s, T &v) {
//    if (!s)
//        return false;
//    return parse_real_it(s, s + std::strlen(s), v);
//}


//
// parseFloat intentionally requires template parameter, because "double" is also float
// the client code should explicitly define their function to call parseFloat without supplying parameter
//

template <class T>
T parseReal(StrView str, T def) noexcept {
    T v;
    return parse_real(str, v) ? v : def;
}

template <class T>
T parseReal(StrView str) {
    T v;
    verify(parse_real(str, v), "Invalid floating point: %s", str);
    return v;
}


//
// Obsolete, rename FP to Real
//
// template <class T>
// [[deprecated]] T parseFP(StrView s, T def) {
//     T v{};
//     return parse_real(s, v) ? v : def;
// }
//
// template <class T>
// [[deprecated]] T parseFP(StrView s) {
//     T v{};
//     verify(parse_real(s, v), "Invalid floating point: %s", s);
//     return v;
// }


inline float parseFloat(StrView s) { return parseReal<float>(s); }
inline float parseFloat(StrView s, float def) { return parseReal<float>(s, def); }


inline double parseDouble(StrView s) { return parseReal<double>(s); }
inline double parseDouble(StrView s, double def) { return parseReal<double>(s, def); }


void parse(StrView s, std::floating_point auto* val) {
    if (!parse_real(s, *val))
        throw std::invalid_argument("Invalid floating point value");
}

void parse(StrView s, std::floating_point auto* val, std::floating_point auto def) {
    if (!parse_real(s, *val))
        *val = def;
}



}  // namespace mangl
