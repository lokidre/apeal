/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../image.hpp"

namespace apl {

struct LibPng {
    using SizeI = Image::SizeI;

    using TempBuffer = Buffer<uint8_t*>;

    struct LoadContext {
        //ImageContainer ic{};
        TempBuffer* tempBuffer{}; // to reuse the buffer for many images
    };

    struct SaveContext {
        const Image* image{};
        Index index{};
        TempBuffer* tempBuffer{}; // to reuse the buffer for many images
    };

    using SaveParams = ImageFile::SaveParams;
    using LoadParams = ImageFile::LoadParams;


  private:
    // Load image returns the final determined image format
    static void loadCore(StrView filename, bool infoOnly, LoadContext& ctx, ImageContainer* ic);
    static void saveCore(StrView filename, SaveContext& ctx);


  public:
    static void loadImageInfo(StrView filename, ImageContainer* ic, LoadParams parms = {}) {
        LoadContext ctx;
        loadCore(filename, true, ctx, ic);
    }

    static void loadImage(StrView filename, ImageContainer* ic, LoadParams parms = {}) {
        LoadContext ctx;
        loadCore(filename, parms.infoOnly, ctx, ic);
    }


    static void saveImage(StrView filename, const Image& image, SaveParams parms = {}) {
        SaveContext ctx;
        ctx.image = &image;
        saveCore(filename, ctx);
    }

    static void saveImage(StrView filename, const ImageContainer& ic, SaveParams parms = {}) {
        SaveContext ctx;
        auto& image = ic.getImage(parms.index);
        ctx.image = std::addressof(image);
        saveCore(filename, ctx);
    }
};

inline Image loadPng(StrView filename) {
    ImageContainer ic;
    LibPng::loadImage(filename, &ic);
    return std::move(ic.front());
}

inline void savePng(StrView filename, const Image& image) {
    LibPng::saveImage(filename, image);
}

} // namespace apl
