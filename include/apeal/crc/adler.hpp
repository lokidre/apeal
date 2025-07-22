/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../def.hpp"
#include "../file.hpp"
#include "../types.hpp"
#include "../buffer.hpp"


namespace apl {


inline
uint32_t crc_adler32(const void *data, std::size_t len, uint32_t ad = 1)
{
    uint32_t s1 = ad%0xFFFF, s2 = ad/0xFFFF;

    auto p = (const uint8_t *)data;

    for (size_t i = 0; i < len; i++, ++p) {
        s1 = (s1 + *p) % 65521;
        s2 = (s2 + s1) % 65521;
    }

    return (s2<<16) + s1;
}


class CrcAdler32 {
public:
    CrcAdler32(): bufferSize_(8192) {}

public:
    unsigned long load(StrView filename)
    {
        File f;
        f.open(filename, AFileAccess::Read);

        buffer_.alloc(bufferSize_) ;

        uint32_t crc = 1;
        int count = 0;

        while ((count = f.read(buffer_.get(), buffer_.size())) > 0)
            crc = crc_adler32(buffer_.get(), count, crc);

        f.close();

        return crc;
    }

private:
    std::size_t bufferSize_;
    Buffer<uint8_t> buffer_;
};

}  // namespace apl
