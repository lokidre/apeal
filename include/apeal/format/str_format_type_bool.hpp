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
void str_format_type_bool(Str& s, F* fmt, F fend, bool arg) noexcept {
    // Ignore the format spec and the parameters (for now)
    auto typespec = fend;
    *fmt = str_format_collect_spec(*fmt, fend, &typespec);

    // auto trueChar = '1', falseChar = '0' ;

    if (typespec != fend) {
        switch (*typespec) {
        case 'c': s.append(1, arg ? 't' : 'f'); break;
        case 'C': s.append(1, arg ? 'T' : 'F'); break;
        case 'd': s.append(1, arg ? '1' : '0'); break;
        case 's': s.append(arg ? "true" : "false"); break;
        case 'S': s.append(arg ? "TRUE" : "FALSE"); break;
        case 'o': s.append(arg ? "yes" : "no"); break; // Like in objective C
        case 'O': s.append(arg ? "YES" : "NO"); break;
        case 'x': s.append(1, arg ? 'y' : 'n'); break; // like in "hex"
        case 'X': s.append(1, arg ? 'Y' : 'N'); break;
        default: s.append(1, arg ? '1' : '0'); break;
        }

    } else {
        s.append(1, arg ? '1' : '0');
    }
}


template <class Str, class F>
void str_format_type(Str& str, F* fmt, F fend, bool arg) noexcept {
    str_format_type_bool(str, fmt, fend, arg);
}

} // namespace apl
