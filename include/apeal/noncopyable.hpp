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

struct NonCopyable {

    constexpr NonCopyable() = default;
    virtual ~NonCopyable() = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    // Making the class copyable
    // NonCopyable ( NonCopyable && ) noexcept {}
    // NonCopyable& operator=(NonCopyable &&other) noexcept {}
};

}  // namespace apl
