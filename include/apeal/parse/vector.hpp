/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <algorithm>
#include <cstring>
#include <vector>

#include "../str/trim.hpp"

namespace apl {

// TODO: implement parsing right with trimming for efficiency
// TODO: implement parsing fully for string_view, explore constexpr possibility
// TODO: implement for both: string and string_view separately

namespace detail {
//template <class S, class I, class V, typename std::enable_if_t<std::is_same_v<S, std::string>, int> = 0>
//template <class S, class I, class V, std::enable_if_t<std::is_convertible_v<S, std::string>, int> = 0>
//void parse_vector_push_back_string(V &v, I begin, std::size_t count) noexcept {
//    v.push_back({begin, begin + count});
//}

//template <class S, class I, class V, typename std::enable_if_t<std::is_same_v<S, std::string_view>, int> = 0>
template <class Val, class Ch, class I, class V, std::enable_if_t<std::is_convertible_v<Val, std::basic_string_view<Ch>>, int> = 0>
void parse_vector_push_back_string(V &v, I begin, std::size_t count) noexcept {
    v.push_back({std::addressof(*begin), count});
}


}  // namespace detail


template <class V, class I, class Char>
void parse_vector_it(I c, I end, Char sep, V &v) noexcept
{
    using Value = typename V::value_type;

    auto fieldsCount = std::count(c, end, sep);

    v.clear();
    v.reserve(fieldsCount + 1);

    for (;;) {
        auto b = c;
        std::size_t count{};

        while (c != end && *c != sep)
            ++c, ++count;

        if (count)
            detail::parse_vector_push_back_string<Value, Char>(v, b, count);
        else
            v.emplace_back();

        if (c == end)
            break;

        ++c;  // skip the delimiter
    }
}

template <class V, class I, class Char>
V parse_vector_it(I c, I end, Char sep) noexcept {
    V v;
    parse_vector_it(c, end, sep, v);
    return v;
}

template <class V, class Char>
void parse_vector(StrView str, Char delim, V &v) noexcept { 
    parse_vector_it(str.begin(), str.end(), delim, v); 
}


template <class V, class Char>
auto parseVector(StrView str, Char delim) noexcept { 
    V v;
    parse_vector_it(str.begin(), str.end(), delim, v);
    return v;
}

template <class ValT, class Char>
auto parseStdVector(StrView str, Char delim) noexcept { 
    std::vector<ValT> v;
    parse_vector_it(str.begin(), str.end(), delim, v);
    return v;
}

// TODO: more efficient implementation
template <class ValT, class Char>
auto parseStdVectorTrim(StrView str, Char delim) noexcept { 
    std::vector<ValT> v;
    parse_vector_it(str.begin(), str.end(), delim, v);

    for (auto& s: v)
        s = str_trim(s);
    return v;
}

template <class Char>
auto parseStdVectorStringTrim(StrView str, Char delim) noexcept {
    return parseStdVectorTrim<String>(str, delim);
}



/*
//
// Returing std::vector
//

template <class V, class I, class Char>
auto parse_std_vector_it(I begin, I end, Char sep) noexcept {
    //std::vector<std::string_view> v;
    parse_vector_it(begin, end, sep, v);
    return v;
}

template <class V, class Char>
auto parse_std_vector_string(StrView s, Char sep) noexcept {
    std::vector<std::string> v;
    parse_vector_it(s.begin(), s.end(), sep, v);
    return v;
}

template <class Char>
auto parse_std_vector_view(StrView s, Char sep) noexcept {
    std::vector<std::string_view> v;
    //V v;
    parse_vector_it(s.begin(), s.end(), sep, v);
    return v;
}

template <class Char>
auto parse_std_vector(StrView s, Char sep) noexcept {
    return parse_std_vector_view(s, sep);
}
*/


// TODO: needed more efficient implementation
template <class I, class C, class V>
void parse_vector_trim_it(I begin, I end, C sep, V &v) noexcept {
    parse_vector_it(begin, end, sep, v);
    for (auto& s: v)
        s = str_trim(s);
}


template <class Char>
auto parse_std_vector_view_trim(StrView s, Char sep) noexcept {
    std::vector<std::string_view> v;
    parse_vector_trim_it(s.begin(), s.end(), sep, v);
    return v;
}


template <class Char>
auto parse_std_vector_string_trim(StrView s, Char sep) noexcept {
    std::vector<std::string> v;
    parse_vector_trim_it(s.begin(), s.end(), sep, v);
    return v;
}

template <class Char>
auto parse_std_vector_trim(StrView s, Char sep) noexcept {
    return parse_std_vector_view_trim(s, sep);
}



}  // namespace apl

