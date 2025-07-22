/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "str_format_type_util.hpp"

namespace apl {

//
// char type
//
template <class Str, class F>
void str_format_type_char(Str &str, F *fmt, F fend, char arg) noexcept {
    // Ignore the format spec and the parameters (for now)
    *fmt = str_format_collect_spec(*fmt, fend);

    str.append(1, arg ? arg : '.');
}

template <class Str, class F>
void str_format_type_char(Str &str, F *fmt, F fend, unsigned char arg) noexcept {
    // Ignore the format spec and the parameters (for now)
    *fmt = str_format_collect_spec(*fmt, fend);

    str.append(1, arg ? arg : '.');
}



template <class Str, class F>
void str_format_type(Str &s, F *fmt, F fend, char arg) noexcept {
    str_format_type_char(s, fmt, fend, arg);
}

template <class Str, class F>
void str_format_type(Str &s, F *fmt, F fend, unsigned char arg) noexcept {
    str_format_type_char(s, fmt, fend, arg);
}


}  // namespace apl
