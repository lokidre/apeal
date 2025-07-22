/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../string.hpp"
#include "../parse/parse_util.hpp"

#include <memory>

namespace apl {

template <class I>
constexpr std::pair<I, I> str_trim_it(I c, I end) noexcept
{
    std::size_t ccount{}, rcount{};

    for (; c != end; ++c, ++ccount)
        if (!parse_is_space(*c))
            break;

    if (c == end)
        return {c, end};

    auto r = end - 1, rbegin = r, rend = c;
    for (; r != rend; --r, ++rcount)
        if (!parse_is_space(*r))
            break;

    if (r == rbegin)
        return {c, end};

    //return {c, end - ccount - rcount};
    return {c, r+1};
}

/*
template <class I, class S>
void str_trim_it(I c, I end, S &s)
{
    std::size_t ccount = 0;

    for (; c != end; ++c, ++ccount)
        if (!parse_is_space(*c))
            break;

    if (c == end) {
        s.clear();
        return;
    }

    std::size_t rcount = 0;
    auto r = s.rbegin(), rbegin = r, rend = s.rend();
    for (; r != rend; ++r, ++rcount)
        if (!parse_is_space(*r))
            break;

    if (r == rbegin) {
        s.assign(c, end);
        return;
    }

    s.assign(c, end - ccount - rcount);
}

template <class S>
void str_trim_update(S &s)
{
    std::size_t ccount = 0;

    auto c = s.begin(), end = s.end();

    for (; c != end; ++c, ++ccount)
        if (!parse_is_space(*c))
            break;

    if (c == end) {  // Empty string or all spaces
        s.clear();
        return;
    }

    // Go backwards too
    std::size_t rcount = 0;
    auto r = s.rbegin(), rend = s.rend();
    for (; r != rend; ++r, ++rcount)
        if (!parse_is_space(*r))
            break;


    if (ccount) {  // the most effective would be just shifting the characters left
        auto cd = s.data(), cs = cd + ccount;
        for (std::size_t i = 0, total = s.size() - ccount; i < total; ++i)
            *cd++ = *cs++;
        s.resize(s.size() - ccount);
    }

    if (rcount)
        s.resize(s.size() - rcount);
}
*/

template <class S>
void str_trim(StrView str, S &d) noexcept {
    auto&& t = str_trim_it(str.begin(), str.end());
    d.assign(t.first, t.second);
    //str_trim_it(s.begin(), s.end(), d);
}


constexpr auto str_trim(StrView str) noexcept
{
    if (str.empty() || (!parse_is_space(str.front()) && !parse_is_space(str.back())))
        return str;

    auto&& t = str_trim_it(str.begin(), str.end());
    return StrView{std::to_address(t.first), std::size_t(t.second - t.first)};  // TODO: remove when c++20 is implemented
}

constexpr auto str_trim(const std::pair<StrView, StrView> &p) noexcept {
    return std::pair{str_trim(p.first), str_trim(p.second)};
}


}  // namespace apl
