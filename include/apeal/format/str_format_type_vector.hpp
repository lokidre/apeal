/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <vector>

#include "str_format_components.hpp"
#include "str_format_append.hpp"

namespace std {

// TODO: move out of std:: and implement using concepts for all containers
// Formatting a vector of any values
template <class Str, class F, class T>
void str_format_type(Str &str, F *fmt, F fend, const std::vector<T> &v) noexcept {
    auto fcomp = ::apl::str_format_component(str, fmt, fend);

    auto first = true;
    if (fcomp.second.empty())  // Separator
        fcomp.second += ',';

    for (auto&& i: v) {
        first = first ? false : (str += fcomp.second, false);
        ::apl::str_format_append(str, fcomp.first, i);
    }
}

}  // namespace apl

