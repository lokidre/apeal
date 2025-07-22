/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../types.hpp"

namespace apl {

#pragma pack(1)
struct IcoFileHeader {
    uint16_t reserved_0;   // Reserved, must be 0
    uint16_t image_type;   // 1 - .ICO, 2 - .CUR
    uint16_t images_count; // Number of images in file
};

struct IcoDirEntry {
    uint8_t width;           // Image width in pixels. 0 value means 256
    uint8_t height;          // Image height in pixels. 0 value means 256
    uint8_t colors;          // Number of colors in color palette
    uint8_t reserved_4;      // should be 0
    uint16_t planes;         // Number of color planes for .ICO images and horz coordinates for .CUR
    uint16_t bits_per_pixel; // Bits per pixel for .ICO, vertical coords for .CUR
    uint32_t data_size;      // Image data size in bytes
    uint32_t data_offset;    // Image data offset from the beginning of the file
};
#pragma pack()

} // namespace apl
