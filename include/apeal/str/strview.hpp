/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
// Custom string view implementation

namespace apl {

#if 0
struct StrView: public std::basic_string_view<Char> {
    //using Char = char;
    using Base = std::basic_string_view<Char>;

    using Base::Base;
    //using Base::empty;
    //using Base::operator=;

    constexpr StrView(const std::basic_string<Char>& s) noexcept: Base{s.data(), s.size()} {}
    constexpr StrView(const String& s) noexcept: Base{s.data(), s.size()} {}
    StrView& operator=(const String& s) noexcept { Base::operator=(s);  return *this; }

    constexpr StrView(const std::basic_string_view<Char>& s) noexcept: Base{s} {}
    constexpr StrView& operator=(const Base& s) noexcept { Base::operator=(s); return *this; }

    constexpr explicit operator bool() const noexcept { return !empty(); }
};
#endif

}  // namespace apl

