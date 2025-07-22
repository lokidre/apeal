/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../concepts.hpp"

namespace apl {


bool parse_bool_it(std::input_iterator auto c, std::input_iterator auto end, bool* result) noexcept {
    bool& res = *result;

    // skip spaces
    while (c != end && isspace(*c))
        ++c;

    if (c == end)
        return false;

    if (*c == '1') {
        res = true;
        ++c;
    } else if (*c == '0') {
        res = false;
        ++c;
    } else {
        int lowc = tolower(*c++);

        if (lowc == 't') { // true
            res = true;
            if (c == end || tolower(*c++) != 'r' || c == end || tolower(*c++) != 'u' || c == end ||
                tolower(*c++) != 'e') {
                return false;
            }
        } else if (lowc == 'f') { // false
            res = false;
            if (c == end || tolower(*c++) != 'a' || c == end || tolower(*c++) != 'l' || c == end ||
                tolower(*c++) != 's' || c == end || tolower(*c++) != 'e') {
                return false;
            }
        } else if (lowc == 'y') { // yes or y
            res = true;
            if (c != end && (tolower(*c++) != 'e' || c == end || tolower(*c++) != 's')) {
                return false;
            }
        } else if (lowc == 'n') { // no
            res = false;
            if (c != end && (tolower(*c++) != 'o'))
                return false;
        } else if (lowc == 'e') { // enable
            res = true;
            if (c == end || tolower(*c++) != 'n' || c == end || tolower(*c++) != 'a' || c == end ||
                tolower(*c++) != 'b' || c == end || tolower(*c++) != 'l' || c == end ||
                tolower(*c++) != 'e') {
                return false;
            }
        } else if (lowc == 'd') { // disable
            res = false;
            if (c == end || tolower(*c++) != 'i' || c == end || tolower(*c++) != 's' || c == end ||
                tolower(*c++) != 'a' || c == end || tolower(*c++) != 'b' || c == end ||
                tolower(*c++) != 'l' || c == end || tolower(*c++) != 'e') {
                return false;
            }
        } else {
            return false;
        }
    }

    while (c != end && isspace(*c))
        ++c;

    if (c != end)
        return false;

    return true;
}


bool parse_bool(TextParamT auto s, bool* b) {
    return parse_bool_it(s.begin(), s.end(), b);
}


bool parseBool(TextParamT auto s) {
    bool b{};
    verify(parse_bool(s, &b), "Invalid bool value: %s", &s);
    return b;
}

bool parseBool(TextParamT auto s, bool def) {
    bool b{};
    if (!parse_bool(s, &b))
        return def;
    return b;
}

void parse(TextParamT auto s, bool* b) {
    verify(parse_bool(s, b), "Invalid bool value: %s", s);
}

void parse(TextParamT auto s, bool* b, bool def) {
    if (!parse_bool(s, b))
        *b = def;
}


} // namespace apl
