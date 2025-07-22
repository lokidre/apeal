/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

//
// Primitive data types parsing
//
#include "def.hpp"
#include "assert.hpp"

// Basic types
#include "parse/string.hpp"

#include "parse/float.hpp"
#include "parse/int.hpp"
#include "parse/bool.hpp"


#include "parse/split.hpp"
#include "parse/vector.hpp"
#include "parse/csv.hpp"

#include "parse/uri.hpp"

// #include "parse/geo.hpp"

#include <concepts>

namespace apl {

// template <class T, std::enable_if_t<std::is_same_v<T, int>, int> = 0>
// int parse(StrView s) { return parseIntGeneric<int>(s); }
//
// template <class T, typename std::enable_if_t<std::is_same_v<T, int>, int> = 0>
// int parse(StrView s, T def) noexcept { return parseIntDefGeneric(s, def); }
//
// template <class T, typename std::enable_if_t<std::is_same_v<T, long>, int> = 0>
// long parse(StrView s) { return parseIntGeneric<long>(s); }
//
// template <class T, typename std::enable_if_t<std::is_same_v<T, long>, int> = 0>
// long parse(StrView s, T def) noexcept { return parseIntDefGeneric(s, def); }
//
// template <class T, typename std::enable_if_t<std::is_same_v<T, long long>, int> = 0>
// long long parse(StrView s) { return parseIntGeneric<long long>(s); }
//
// template <class T, typename std::enable_if_t<std::is_same_v<T, long long>, int> = 0>
// long long parse(StrView s, T def) noexcept { return parseIntDefGeneric(s, def); }
//
//
// template <class T>
// typename std::enable_if<std::is_same<T, unsigned int>::value, unsigned int>::type
// parse(StrView s) { return parseUIntGeneric<unsigned int>(s); }
//
// template <class T>
// typename std::enable_if<std::is_same<T, unsigned int>::value, unsigned int>::type
// parse(StrView s, T def) noexcept { return parseUIntDefGeneric(s, def); }
//
// template <class T>
// typename std::enable_if<std::is_same<T, unsigned long>::value, unsigned long>::type
// parse(StrView s) { return parseUIntGeneric<unsigned long>(s); }
//
// template <class T>
// typename std::enable_if<std::is_same<T, unsigned long>::value, unsigned long>::type
// parse(StrView s, T def) noexcept { return parseUIntDefGeneric(s, def); }
//
// template <class T>
// typename std::enable_if<std::is_same<T, unsigned long long>::value, unsigned long long>::type
// parse(StrView s) { return parseUIntGeneric<unsigned long long>(s); }
//
// template <class T>
// typename std::enable_if<std::is_same<T, unsigned long long>::value, unsigned long long>::type
// parse(StrView s, T def) noexcept { return parseUIntDefGeneric(s, def); }



// template <class T>
// typename std::enable_if<std::is_same<T, bool>::value, bool>::type
// parse(StrView s) { return parseBool(s); }
//
// template <class T>
// typename std::enable_if<std::is_same<T, bool>::value, bool>::type
// parse(StrView s, T def) noexcept { return parseBool(s, def); }



// template <class T>
// typename std::enable_if<std::is_same<T, float>::value, float>::type
// parse(StrView s) { return parseFloat(s); }
//
// template <class T>
// typename std::enable_if<std::is_same<T, float>::value, float>::type
// parse(StrView s, T def) noexcept { return parseReal(s, def); }


// template <class T>
// typename std::enable_if<std::is_same<T, double>::value, double>::type
// parse(StrView s) { return parseDouble(s); }
//
// template <class T>
// typename std::enable_if<std::is_same<T, double>::value, double>::type
// parse(StrView s, T def) noexcept { return parseReal(s, def); }
//
//
// template <class T>
// typename std::enable_if<std::is_same<T, long double>::value, long double>::type
// parse(StrView s) { return parseDouble(s); }
//
// template <class T>
// typename std::enable_if<std::is_same<T, long double>::value, long double>::type
// parse(StrView s, T def) noexcept { return parseReal(s, def); }



}  // namespace apl
