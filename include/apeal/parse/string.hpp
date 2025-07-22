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

#include <concepts>


namespace apl {

constexpr void parse(StrView s, String* val) noexcept {
    *val = s;
}

constexpr void parse(StrView s, String* val, const std::convertible_to<StrView> auto& def) noexcept {
    *val = s.empty() ? def : s;
}

// constexpr void parse(const std::convertible_to<String> auto& s, String* val) noexcept {
//     return *val = s;
// }
//
// constexpr void parse(const std::convertible_to<String> auto& s, String* val, StringA def) noexcept {
//     *val = s.empty() ? def : s;
// }

} // namespace apl
