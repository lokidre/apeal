/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "str_format_append_body_arg.hpp"
#include "str_format_remainder.hpp"

//
// Main formatting function body
//

namespace apl {

template <class Str, class F, class... Args>
Str &str_format_append_it_body(Str &str, F *fmt, F fend, Args &&... args) noexcept {
    str_format_append_body_arg(str, fmt, fend, std::forward<Args>(args)...);
    str_format_remainder(str, fmt, fend);
    return str;
}

} // namespace apl
