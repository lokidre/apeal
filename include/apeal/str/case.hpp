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

namespace apl {

//
// TODO: update properly

template <class C>
C char_lower(C c) noexcept {
    return (C)std::tolower(c);
}

template <class C>
C char_upper(C c) noexcept {
    return (C)std::toupper(c);
}


inline
String str_lower(StrView s) noexcept
{
    String d;
    d.reserve(s.length());

    //size_t index = 0;

    for (auto&&c: s)
        d += char_lower(c);

    return d;
}

template <class I>
void str_lower_update_it(I c, I end) noexcept
{
    for (; c != end; ++c)
        if (std::isalpha(*c))
            *c = static_cast<std::remove_reference_t<decltype(*c)>>(std::tolower(*c));
}


template <class S>
void str_lower_update(S &s)  noexcept {
    str_lower_update_it(s.begin(), s.end());
}


template <class SI, class DI>
void str_lower_it(SI s, SI send, DI d) noexcept
{
    for (; s != send; ++s, ++d)
        *d = std::isalpha(*s) ? std::tolower(*s) : *s;
}


template <class S, class D>
void str_lower(const S &s, D &d) noexcept
{
    d.resize(s.size());
    str_lower_it(s.begin(), s.end(), d.begin());
}


inline
auto str_upper(StrView s) noexcept
{
    String d;
    d.reserve(s.size());

    for (auto &c: s)
        d.append(1, char_upper(c));
    return d;
}

template <class string_1_type, class string_2_type>
string_2_type &str_upper(const string_1_type &s, string_2_type &d)
{
    d.clear();

    for (auto c = s.begin(), c_end = s.end(); c != c_end; ++c)
        d.append(1, toupper(*c));

    return d;
}

}  // namespace apl
