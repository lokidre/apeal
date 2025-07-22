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

template <class Str, class F, class... Args>
Str& str_format_append_it(Str& str, F fmt, F fend, Args&&... args) noexcept {
    return str_format_append_it_body(str, &fmt, fend, std::forward<Args>(args)...);
}

template <class Str, class... Args>
Str& str_format_append(Str& str, StrView fmt, Args&&... args) noexcept {
    return str_format_append_it(str, fmt.begin(), fmt.end(), std::forward<Args>(args)...);
}

} // namespace apl
