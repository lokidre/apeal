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


namespace apl {

template <class Str, class F>
void str_format_remainder(Str& text, F* pfmt, F fend) noexcept {
    std::size_t count{};
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
            if (fmt == fend) {
                ADetailDebugPrintCore("Warning::str_format: Expecting format or percent sign!");
                text.append(1, '%'); // add it back, just in case but issue a warning
                break;
            }
            if (*fmt != '%') {
                ADetailDebugPrintCore("Warning::str_format: Extra format but argument is missing!");
            }
            // just continue with this same situation
            begin = fmt;
        }

        ++count, ++fmt;
    }
}

} // namespace apl
