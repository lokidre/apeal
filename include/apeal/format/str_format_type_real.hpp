/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <cstdio>  // std::snprintf

namespace apl {

//
// Real type
// TODO: std::to_char
//

template <class F, class S, class Arg>
void str_format_type_float(S &str, F *pfmt, F fend, Arg arg) noexcept
{
    // TODO: needed to implement floating point generation
    // for now use sprintf

    constexpr int buffSize = 1024;
    char format[buffSize+1];
    char buffer[buffSize+1];
    int ifmt = 0;

    format[ifmt++] = '%';

    auto& fmt = *pfmt;

    // skip until the first alpha
    while (fmt != fend && !isalpha(*fmt))
        format[ifmt++] = *fmt++;

    if (fmt != fend) {
        if (*fmt=='f' || *fmt=='F' || *fmt=='g' || *fmt=='G' || *fmt == 'e' || *fmt == 'E')
            format[ifmt++] = *fmt;
        else
            format[ifmt++] = 'f';  // default to 'f' format
        ++fmt;
    }

    format[ifmt] = 0;

#if __STDC_LIB_EXT1__
    std::snprintf_s(buffer, buffSize, format, arg);
#else
    std::snprintf(buffer, buffSize, format, arg);
#endif

    str += buffer;
}

template <class F, class S>
void str_format_type(S &str, F *fmt, F fend, float arg) noexcept {
    str_format_type_float(str, fmt, fend, arg);
}

template <class F, class S>
void str_format_type(S &str, F *fmt, F fend, double arg) noexcept {
    str_format_type_float(str, fmt, fend, arg);
}


}  // namespace apl

