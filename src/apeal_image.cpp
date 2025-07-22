/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/image.hpp"

namespace apl {

struct ImageFormatStaticInfo {
    ColorModel cm;
    StrView sign;  // signature
    int bps;  // bits per sample
    int bpp;  // bytes per pixel
    int ncmp;  // number of components
    int npl;  // Number of planes
    Image::CompsLayout layout;
    ImageSubSampling sub;
};

constexpr static ImageFormatStaticInfo imageFormatStaticInfo[] = {
    {
        .cm = ColorModel::Gray,   
        .sign = "G",
        .bps = 8, 
        .bpp = 1, 
        .ncmp = 1,
        .layout = {},
        .sub = {},
    },

    {
        .cm = ColorModel::RGB,   
        .sign = "RGB",
        .bps = 8, 
        .bpp = 3, 
        .ncmp = 3,
        .layout = {.r = 0, .g = 1, .b = 2},
        .sub = {.j = 4, .a = 4, .b = 4},
    },
    {
        .cm = ColorModel::RGBA,
        .sign = "RGBA",
        .bps = 8,
        .bpp = 4,
        .ncmp = 4,
        .layout = {.r = 0, .g = 1, .b = 2, .a = 3},
        .sub = {.j = 4, .a = 4, .b = 4, .alpha = 4},
    },
    {
        .cm = ColorModel::BGRA,
        .sign = "BGRA",
        .bps = 8,
        .bpp = 4,
        .ncmp = 4,
        .layout = {.r = 2, .g = 1, .b = 0, .a = 3},
        .sub = {.j = 4, .a = 4, .b = 4, .alpha = 4},
    },

    // YCC Packed
    {
        .cm = ColorModel::YCC,
        .sign = "YCC",
        .bps = 8, 
        .bpp = 2,
        .ncmp = 3,
        .layout = {.y = 0, .u = 1, .v = 2},
        .sub = {.j = 4, .a = 2, .b = 0},
    },

    // YCC Planar
    {
        .cm = ColorModel::YCC,
        .sign = "YCCP",
        .bps = 8, 
        .bpp = 2,
        .ncmp = 1,
        .npl = 3,
        .layout = {.y = 0, .u = 1, .v = 2},
        .sub = {.j = 4, .a = 2, .b = 0},
    },

};

void Image::processMaintenance()
{
    // fix all the missing information
    bool found{};

    for (auto& info: imageFormatStaticInfo) {
        if (info.cm != cm)
            continue;

        if (numPlanes && info.npl != numPlanes)
            continue;

        compsLayout = info.layout;

        if (ImageValueType::Unknown == val)
            val = ImageValueType::U8;

        switch (val) {
        case ImageValueType::U8:    depth = 8;  break;
        case ImageValueType::U16:   depth = 16; break;
        case ImageValueType::U32:   depth = 32; break;
        default: 
            ADebugTrap();
            break;
        }

        if (!prec)
            prec = depth;

        if (!sub.j)
            sub = info.sub;

        // Handle the planar situation
        if (numPlanes) {
            // TODO: assuming YCC
            planes.resize(info.npl);

            auto plIt = planes.begin();
            {
                auto& pl = *plIt++;
                pl.size.w = size.w; //sub.bytesPerRow(size.w, depth); //size.w;
                //pl.dim.h = sub.rowsPerHeight(size.h); //b ? size.h : size.h / 2;
                pl.size.h = size.h; //sub.b ? size.h : size.h / 2;
            }

            while (plIt != planes.end()) {
                auto& pl = *plIt++;
                pl.size.w = size.w / 2;
                pl.size.h = size.h / 2;
            }

            for (auto& pl: planes) {
                pl.val = val;
                pl.prec = prec;
                pl.bpr = pl.size.w;
            }

            //pl.dim.w = sub.
        }

        bytesPerSample = calcBytesPerSample();


        if (!bytesPerRow) {
            bytesPerRow = calcBytesPerRow(); //info.bpp * dim.w;
            //bpr = alignBytesPerRow32(bytesPerRow);
        }

        if (!numComps)
            numComps = info.ncmp;

        found = true;
        break;
    }

    ADebugTrapAssert(found);
}


void ImageContainer::processImagesMaintenance()
{
    // recreating ordered vector is easy
    byHeight.clear();

    bool byHeightUnordered{};

    for (ImageIndex idx{}; auto& image: images) {

        if (!byHeight.empty() && image.size.h >= byHeight.back().first)
            byHeightUnordered = true;

        byHeight.emplace_back(image.size.h, idx);

        ++idx;
    }

    if (!byHeightUnordered)
        std::ranges::sort(byHeight, std::greater<>());
}


StrView Image::colorModelTag(ColorModel c) noexcept {
    for (auto& info: imageFormatStaticInfo) {
        if (info.cm == c)
            return info.sign;
    }

    ADebugTrap();
    return {};
}

StrView ImageFile::fileFormatExt(ImageFileFormat fmt)
{
    switch (fmt) {
    using enum ImageFileFormat;
    case Png:    return "png";
    case Jpeg:   return "jpeg";
    case Tiff:   return "tiff";

    case Ico:    return "ico";
    case Icns:   return "icns";
    case Bmp:    return "bmp";

    default:
        ADebugTrap();
        break;
    }
    return {};
}

}  // namespace apl

