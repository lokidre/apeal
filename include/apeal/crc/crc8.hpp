/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "./crc.hpp"

namespace apl {

namespace impl {

class Crc8: public CrcBase<Crc8> {

public:
    static constexpr int ValueBits = 8;
    static constexpr int ValueBytes = 1;

    using Value = std::uint8_t;


    using iterator = std::uint8_t*;
    using const_iterator = const std::uint8_t*;

private:
    inline static Value table_[256];
    inline static bool initialized_{false};

    // Initialization
    static void init() noexcept {
        if (initialized_)
            return;

        std::uint8_t generator = 0x1D;

        for (unsigned int divident = 0; divident < 256; ++divident) {
            auto rem = (uint8_t)divident; // remainder
            for (auto bit = ValueBits; bit > 0; --bit)
                rem = (rem & 0x80) != 0 ? ((rem << 1) ^ generator) : (rem << 1);
            table_[divident] = rem;
        }

        initialized_ = true;
    }

protected:
    static Value onStart() noexcept {
        init();
        return 0x1d;
    }

    // Main CRC function
    static Value onUpdate(Value crc, const_iterator begin, const_iterator end) noexcept {
        for (auto ptr = begin; ptr != end; ++ptr) {
            auto val = *ptr ^ crc;
            crc = table_[val];
        }

        return crc;
    }

    static Value onFinish(Value crc) noexcept { return crc; }
};

} // namespace impl


using Crc8 = Crc<impl::Crc8>;


} // namespace apl
