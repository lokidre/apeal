/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <cctype>

#include "str_format_util.hpp"

namespace apl {

//
// Building formats for custom types
//

// Helper function to print the list of values
template <class Str, class F>
auto str_format_component(Str& str, F* pfmt, F fend) noexcept -> std::pair<Str, Str> {
    Str cf{'%'}; // component format
    Str sepf;    // separator format

    auto& fmt = *pfmt;

    while (fmt != fend) {

        if (*fmt == '|') { // separator
            ++fmt;
            while (fmt != fend && *fmt != '|') {
                sepf += *fmt++;
            }
            if (fmt != fend) // skip the separator separator
                ++fmt;
        }

        cf += *fmt;
        auto alpha = str_format_type_spec_char(*fmt);
        ++fmt;
        if (alpha)
            break;
    }

    return {cf, sepf};
}

template <class Str, class F>
auto str_format_list_components(Str& str, F* pfmt, F fend, char sep, int count) noexcept -> Str {
    Str cf{'%'}, ff;
    Str sepf;

    auto& fmt = *pfmt;

    while (fmt != fend) {
        if (*fmt == '|') { // separator
            ++fmt;
            while (fmt != fend && *fmt != '|') {
                sepf += *fmt++;
            }
            if (fmt != fend) // skip the separator separator
                ++fmt;
        }

        cf += *fmt;
        auto alpha = str_format_type_spec_char(*fmt);

        ++fmt;

        if (alpha)
            break;
    }

    if (sepf.empty())
        sepf.assign(1, sep);

    for (int i = 0; i < count; ++i) {
        if (!ff.empty())
            ff += sepf;
        ff += cf;
    }

    return ff;
}

} // namespace apl
