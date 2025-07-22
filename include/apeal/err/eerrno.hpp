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

#include <string.h>
#include <errno.h>

#include <array>

namespace apl {

inline
String getErrorStringErrno(int code)
{
    String str;

#ifdef __STDC_LIB_EXT1__
    const auto bufferSize = strerrorlen_s(code);
    std::array<char, bufferSize+1> buffer;

    auto ret = strerror_s(buffer.data(), bufferSize, code);

    if (ret)
        str = ret;
#else

#ifdef _MSC_VER
    std::array<char, 1024> buffer;
    auto ret = strerror_s(buffer.data(), buffer.size()-1, code);
    if (ret == 0) {
        str = buffer.data();
    }
    if (ret == ERANGE)
        str = "Buffer too small for strerror";
#else
    auto ret = strerror(code);
    if (ret)
        str = ret;
#endif  // streror_s

#endif  // EXT1

    if (str.empty())
        str = sformat("Unknown errno: %d (%X)", code, code);

    return str;
}

template <class Condition>
void verifyErrno(const Condition &cond) {
    verify(cond, getErrorStringErrno(errno));
}

template <class Condition, class S>
void verifyErrno(const Condition &cond, const S &msg) {
    verify(cond, "%s: %s", msg, getErrorStringErrno(errno));
}

}  // namespace apl
