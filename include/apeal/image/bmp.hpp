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
#include "../types.hpp"

namespace apl {

constexpr auto BmpFileHeaderSize = 14;
constexpr auto BmpInfoHeaderSize = 40;

#pragma pack(1)
struct BmpFileHeader {
    uint16_t header;   // 'BM'
    uint32_t size;     // file size in bytes
    uint32_t res;      // reserved, must be 0
    uint32_t off_bits; // offset from the beginning of the file to the bitmap data
};

struct BmpInfoHeader {
    uint32_t bih_size; // 40 - size of the structure in bytes
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_pels_per_meter;
    uint32_t y_pels_per_meter;
    uint32_t clrs_used;
    uint32_t clrs_important;
};

struct BmpRgbQuad {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t res;
};
#pragma pack()


} // namespace apl
