/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <string_view>
#include <cstring>

#include "str_format_type_util.hpp"


namespace apl {

template <class S, class F, class T>
void str_format_type_string_it(S &s, F *fmt, F fend, T begin, T end) noexcept
{
    // Ignore the format spec and the parameters (for now)
    // TODO: read the format parameters properly (alignment, maybe uppercase/lowercase)
    *fmt = str_format_collect_spec(*fmt, fend);

    if (begin == end)
        return;

    s.append(begin, end);
}



//
// Formattting the string types
//
template <class S, class F, class T, std::enable_if_t<std::is_convertible_v<T, std::string_view>, int> = 0>
void str_format_type(S &out, F* fmt, F fend, T arg) noexcept {
    str_format_type_string_it(out, fmt, fend, arg.begin(), arg.end());
}

//template <class S, class F>
//void str_format_type(S &out, F* fmt, F fend, std::string_view arg) noexcept {
//    str_format_type_string_it(out, fmt, fend, arg.begin(), arg.end());
//}

/*
template <class S, class F, std::enable_if_t<!std::is_same_v<StrView, std::string_view>, int> = 0>
void str_format_type(S &out, F* fmt, F fend, StrView arg) noexcept {
    str_format_type_string_it(out, fmt, fend, arg.begin(), arg.end());
}


template <class S, class F>
void str_format_type(S &out, F* fmt, F fend, const std::string& arg) noexcept {
    str_format_type_string_it(out, fmt, fend, arg.begin(), arg.end());
}

template <class S, class F, std::enable_if_t<!std::is_same_v<String, std::string>, int> = 0>
void str_format_type(S &out, F* fmt, F fend, const String& arg) noexcept {
    str_format_type_string_it(out, fmt, fend, arg.begin(), arg.end());
}
*/

template <class S, class F>
void str_format_type(S &s, F* fmt, F fend, char *arg) noexcept {
    str_format_type_string_it(s, fmt, fend, arg, arg + (arg ? std::strlen(arg) : 0));
}

template <class S, class F>
void str_format_type(S &s, F* fmt, F fend, const char *arg) noexcept
{
    str_format_type_string_it(s, fmt, fend, arg, arg + (arg ? std::strlen(arg) : 0));
}


}  // namespace apl

