/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "def.hpp"

namespace apl {

template <class _T>
struct ValRange {
    using T = _T;
    using value_type = T;

    T mn, mx;

    [[nodiscard]] constexpr T min() const noexcept { return mn; }
    [[nodiscard]] constexpr T max() const noexcept { return mx; }

    [[nodiscard]] constexpr bool empty() const noexcept { return mn == 0 && mx == 0; }
    [[nodiscard]] constexpr bool valid() const noexcept { return mn != 0 && mx != 0; }

    constexpr explicit operator bool() const noexcept { return !empty(); }
};



}  // namespace apl
