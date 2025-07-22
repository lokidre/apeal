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

#include <array>

namespace apl {


void generateUuid(void* data, std::size_t size);

inline std::array<uint8_t, 16> generateUuid() noexcept {
    std::array<uint8_t, 16> uuid{};
    generateUuid(uuid.data(), uuid.size());
    return uuid;
}


String generateUuidBase58() noexcept;

} // namespace apl
