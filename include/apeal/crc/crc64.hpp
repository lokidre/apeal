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

class Crc64: public CrcBase<Crc64> {
public:
    static constexpr int ValueBits = 64;
    static constexpr int ValueBytes = 8;

    using Value = std::uint64_t;

    using iterator = std::uint8_t*;
    using const_iterator = const std::uint8_t*;


private:
    static constexpr int tableSize_{256};
    inline static Value table_[tableSize_];
    inline static bool initialized_{false};


private:
    static void init() noexcept {
        if (initialized_)
            return;

        auto t = table_;
        auto poly = 0xD800000000000000;

        for (int i = 0; i < tableSize_; ++i, ++t) {
            Value p = i;
            for (int j = 0; j < 8; ++j) {
                if (p & 1)
                    p = (p >> 1) ^ poly;
                else
                    p >>= 1;
            }
            *t = p;
        }

        initialized_ = true;
    }

protected:
    static Value onStart() noexcept {
        init();
        return 0xD800000000000000;
    }

    static Value onUpdate(Value crc, const_iterator begin, const_iterator end) noexcept {
        for (auto ptr = begin; ptr != end; ++ptr) {
            auto t1 = crc >> 8;
            auto t2 = table_[(crc ^ Value(*ptr)) & 0xFF];
            crc = t1 ^ t2;
        }

        return crc;
    }

    static Value onFinish(Value crc) noexcept { return crc; }
};

} // namespace impl


using Crc64 = Crc<impl::Crc64>;

} // namespace apl
