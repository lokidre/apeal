/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

namespace apl {

template <class C>
constexpr bool parse_is_space(C c) noexcept { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }

template <class C>
constexpr bool parse_is_eol(C c) noexcept { return c == '\r' || c == '\n'; }


//
// parses single character digit into integer value
// returns -1 if the digit could not be converted
//
template <class C>
constexpr int parse_int_dig_hex(C c) noexcept
{
    if (c >= 'a' && c <= 'f')
        return int(c - 'a' + 10);

    if (c >= 'A' && c <= 'F')
        return int(c - 'A' + 10);

    if (c >= '0' && c <= '9')
        return int(c - '0');

    return -1;
}


}  // namespace apl

