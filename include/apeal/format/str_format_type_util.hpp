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
// Various utilies helpful for formatting the argument
//

namespace apl {

template <class C>
constexpr bool str_format_type_spec_char(C c) noexcept {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

template <class C>
constexpr bool is_str_format_digit(C c) noexcept {
    return (c >= '0' && c <= '9');
}


template <class F>
constexpr F str_format_collect_spec(F fmt, F fend, F *typespec = 0) noexcept
{
    // skip until the first alpha
    while (fmt != fend && !str_format_type_spec_char(*fmt))
        ++fmt;

    // Type type spec character
    if (typespec)
        *typespec = fmt;

    if (fmt != fend)
        ++fmt;

    return fmt;  // return the first iterator after the type spec
}

}  // namespace apl

