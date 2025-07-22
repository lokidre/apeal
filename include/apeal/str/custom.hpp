/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

// Custom string implementation

namespace apl {

struct CustomString: public std::basic_string<char> {
    using Char = char;
    using Base = std::basic_string<Char>;

    //using Base::Base;
    //using Base::empty;
    //using Base::operator=;
    //using Base::operator+=;

    String() noexcept {}
    String(const Char* ch) noexcept: Base{ch} {}
    String& operator=(const Char* ch) noexcept { Base::operator=(ch); return *this; }
    String(const Char* ch, std::size_t count) noexcept: Base{ch, count} {}

    String& operator=(Char ch) noexcept { Base::operator=(ch); return *this; }

    String& operator=(std::initializer_list<Char> ilist) noexcept { Base::operator=(ilist); return *this; }


    String(const String& s) noexcept: Base{s.data(), s.size()} {}

    template <class It>
    String(It first, It last) noexcept: Base{first, last} {}

    String(const Base& s) noexcept: Base{s} {}
    String& operator=(const Base& s) noexcept { Base::operator=(s); return *this; }

    String(const std::basic_string_view<Char>& s) noexcept: Base{s} {}
    String& operator=(const std::basic_string_view<Char>& s) noexcept { Base::operator=(s);  return *this; }


    explicit operator bool() const noexcept { return !empty(); }
};

}  // namespace apl;
