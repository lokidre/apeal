/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "types.hpp"
#include "string.hpp"

namespace apl {

uint64_t generateUid64() noexcept;
String formatUid64Base68(uint64_t) noexcept;

inline String generateUid64Base58() noexcept { return formatUid64Base68(generateUid64()); }

} // namespace apl
