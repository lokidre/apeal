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

#if HAVE_LIBJPEG

struct LibJpeg {
    using SaveParams = ImageFile::SaveParams;
    using LoadParams = ImageFile::LoadParams;


    static ImageContainer loadImageInfo(StrView filename, LoadParams parms={}) {
        LoadContext ctx;
        loadCore(filename, true, ctx);
        return ctx.ic;
    }

    static ImageContainer loadImage(StrView filename, LoadParams parms={}) {
        LoadContext ctx;
        loadCore(filename, parms.infoOnly, ctx);
        return ctx.ic;
    }

    static void saveImage(StrView filename, const Image& image, SaveParams parms={}) {
        SaveContext ctx;
        ctx.image = &image;
        saveCore(filename, ctx);
    }

    static void saveImage(StrView filename, const ImageContainer& ic, SaveParams parms={}) {
        SaveContext ctx;
        auto& image = ic.getImage(parms.index);
        ctx.image = std::addressof(image);
        saveCore(filename, ctx);
    }

private:
    struct LoadContext {
        ImageContainer ic{};
        //TempBuffer* tempBuffer{};  // to reuse the buffer for many images
    };

    struct SaveContext {
        //const ImageContainer *ic{};
        const Image *image{};
        Index index{};
        //TempBuffer* tempBuffer{};  // to reuse the buffer for many images
    };


    static void loadCore(StrView filename, bool infoOnly, LoadContext& ctx);
    static void saveCore(StrView filename, SaveContext& ctx);
};


inline
Image loadJpeg(StrView filename) {
    return LibJpeg::loadImage(filename).front();
}

inline
void saveJpeg(StrView filename, const Image& image) {
    LibJpeg::saveImage(filename, image);
}

#endif

} // namespace apl
