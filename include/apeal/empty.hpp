/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#if 0

Definition of empty() test for strings
---


#endif

#include <string>


namespace apl {

constexpr bool empty(const char* s) noexcept { return !s || !*s; }

constexpr bool empty(const std::string& s) { return s.empty(); }
constexpr bool empty(const std::wstring& s) { return s.empty(); }


}  // namespace apl

