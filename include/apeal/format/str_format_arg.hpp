/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../debug/debug_print_core.hpp"

#include "str_format_type.hpp"

namespace apl {

//
// Arguments
//

template <class Str, class F, class T>
void str_format_arg(Str& text, F* pfmt, F fend, T&& arg) noexcept {
    // First just push all the string befor the format
    Index count{};
    auto& fmt = *pfmt;
    auto begin = fmt;

    for (;;) {
        if (fmt == fend || *fmt == '%') { // beginning of the
            if (count) {                  // copy watever was before
                text.append(begin, fmt);
                count = 0;
                begin = fmt;
            }
            if (fmt == fend)
                break; // done

            ++fmt; // skip the '%' sign
            // check if this is just '%' symbol
            if (fmt == fend) {
                ADetailDebugPrintCore("Warning::str_format: Expecting format or percent sign!");
                break;
            }
            if (*fmt == '%') { // then it's just a '%' symbol
                begin = fmt++;
                count = 1;
                continue; // continue
            }
            break; // now we have to do the argument formatting
        }
        ++count, ++fmt;
    }

    // Check the situation with the formatting
    // If there are more arguments but no formats are given, maybe it's mistake, issue warning
    // and keep generating the string with some separator
    if (fmt == fend) {
        ADetailDebugPrintCore("Warning:str_format: No format for extra arguments!");
        // auto f = "|%z";  // default format
        // str_format_arg(s, f, f + strlen(f), std::forward<T>(arg));  // call ourselves recursively
        auto d = std::string_view{"|%z"}; // default format
        auto dbegin = d.begin(), dend = d.end();
        str_format_arg(text, &dbegin, dend, std::forward<T>(arg)); // call ourselves recursively
        return;
    }

    // go until end of format string
    str_format_type(text, pfmt, fend, std::forward<T>(arg));
}

} // namespace apl
