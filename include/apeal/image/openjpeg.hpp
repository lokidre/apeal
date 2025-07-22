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

namespace impl {
struct OpenJpegOpContextImpl;

struct OpContext {
    Image::InitInfo ii{};
    std::unique_ptr<OpenJpegOpContextImpl> pimpl;

    OpContext() noexcept;
    OpContext(OpContext&& other) noexcept;
    OpContext& operator=(OpContext&& other) noexcept;
    ~OpContext() noexcept;

    void swap(OpContext& other) noexcept {
        std::swap(ii, other.ii);
        std::swap(pimpl, other.pimpl);
    }
};

struct JpLoadContext {
    ImageFile::LoadParams parms{};
};

struct JpSaveContext {
    ImageFile::SaveParams parms{};
    const Image* image{};
    int index{};
};



}   // namespace impl

struct OpenJpeg {
    using LoadParams = ImageFile::LoadParams;
    using SaveParams = ImageFile::SaveParams;

    static ImageContainer loadImageInfo(StrView filename, LoadParams lp={}) {
        lp.infoOnly = true;
        impl::JpLoadContext ctx{.parms = lp};
        ImageContainer ic;
        loadCore(filename, ctx, &ic);
        return ic;
    }

    static void loadImage(StrView filename, ImageContainer *ic, LoadParams lp={}) {
        impl::JpLoadContext ctx{.parms = lp};
        loadCore(filename, ctx, ic);
    }

    static void saveImage(StrView filename, const Image& image, SaveParams parms={}) {
        impl::JpSaveContext ctx{.parms=parms, .image=&image};
        saveCore(filename, ctx);
    }

    static void saveImage(StrView filename, const ImageContainer& ic, SaveParams parms={}) {
        impl::JpSaveContext ctx{.parms = parms};
        auto& image = ic.getImage(parms.index);
        ctx.image = std::addressof(image);
        saveCore(filename, ctx);
    }

    static impl::OpContext startLoadProcess(StrView filename, LoadParams params);
    static void decodeImage(impl::OpContext& opctx, Image& image);
    static void decodeTile(impl::OpContext& opctx, Point<int> tilePos, Image& img, Point<int> imgPos);
    static void finishLoadProcess(impl::OpContext& opCtx);


    static impl::OpContext startSaveProcess(StrView filename, ImageA image, SaveParams params);
    static void encodeImage(impl::OpContext& opctx, ImageA image);
    static void encodeTile(impl::OpContext& opctx, Point<int> tilePos, ImageA img, Point<int> imgPos);
    static void finishSaveProcess(impl::OpContext& opCtx);


private:
    static void loadCore(StrView filename, impl::JpLoadContext& ctx, ImageContainer* ic);
    static void saveCore(StrView filename, impl::JpSaveContext& ctx);
};

}  // namespace apl
