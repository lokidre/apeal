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
// Main formatting functionality
//

#include "str_format_arg.hpp"

namespace apl {

//
// Terminator
//
template <class Str, class F> constexpr void str_format_append_body_arg(Str&, F*, F) noexcept {}

//
// Recursive argument formatting until terminiator ( no more arguments)
//
template <class Str, class F, class T, class... Args>
void str_format_append_body_arg(Str& str, F* fmt, F fend, T&& arg, Args&&... args) noexcept {
    // Format the current argument
    str_format_arg(str, fmt, fend, std::forward<T>(arg));

    // Recursion for each supplied argument
    str_format_append_body_arg(str, fmt, fend, std::forward<Args>(args)...);
}

} // namespace apl
