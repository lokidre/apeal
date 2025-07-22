/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../str/format.hpp"

#include "../geo/point.hpp"
#include "../geo/point3d.hpp"
#include "../geo/size.hpp"
#include "../geo/rect.hpp"


namespace apl {

// Point
template <class Str, class F, class T>
void str_format_type(Str &str, F* fmt, F fend, const Point<T>& u) noexcept {
    auto lfmt = str_format_list_components(str, fmt, fend, ',', 2);
    str_format_append(str, lfmt, u.x, u.y);
}

// Point3D
template <class Str, class F, class T>
void str_format_type(Str &str, F* fmt, F fend, const Point3D<T>& u) noexcept {
    auto lfmt = str_format_list_components(str, fmt, fend, ',', 3);
    str_format_append(str, lfmt, u.x, u.y, u.z);
}

// Size
template <class Str, class F, class T>
void str_format_type(Str &str, F* fmt, F fend, const Size<T>& u) noexcept {
    auto lfmt = str_format_list_components(str, fmt, fend, ',', 2);
    str_format_append(str, lfmt, u.w, u.h);
}


// Rect
template <class Str, class F, class T>
void str_format_type(Str &str, F* fmt, F fend, const Rect<T>& u) noexcept {
    auto lfmt = str_format_list_components(str, fmt, fend, ',', 4);
    str_format_append(str, lfmt, u.x, u.y, u.w, u.h);
}


}  // namespace apl

