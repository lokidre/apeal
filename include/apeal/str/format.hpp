/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../string.hpp"

#include <utility>

#include "../format/str_format_util.hpp"

// Here all the formatting of all the basic types are defined
#include "../format/str_format_append_body.hpp"
#include "../format/str_format_append.hpp"

// Composite types
#include "../format/str_format_type_vector.hpp"
#include "../format/str_format_type_geo.hpp"
// #include "../format/str_format_type_geo.hpp"


//
// TODO: better do with accepting Fmt as std::string or const char
//

namespace apl {

//
// str_format
//

template <class S, class ...Args>
S& str_format(S& text, StrView fmt, Args&&... args) noexcept {
    text.clear();
    return str_format_append(text, fmt, std::forward<Args>(args)...);
}


//
// string format
//
template <class ...Args>
String sformat(StrView fmt, Args&&... args) noexcept {
    String text;
    return str_format(text, fmt, std::forward<Args>(args)...);
}


// TODO: explore constexpr
inline
String sformat(StrView text) noexcept { return String{text}; }

inline
String sformat() noexcept { return String{}; }


//
// Shortened for formatting things
//
template <class ...Args>
String fmt(StrView fmt, Args&&... args) noexcept {
    String s;
    return str_format(s, fmt, std::forward<Args>(args)...);
}

//#if __cpp_lib_constexpr_string
// constexpr
//#else
inline
//#endif
String fmt(StrView s) noexcept { return String{s}; }

inline
String fmt() noexcept { return String{}; }


}  // namespace apl
