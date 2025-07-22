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
#include "../noncopyable.hpp"

namespace apl {

namespace impl {
struct LibTiffLoadContextImpl;

struct TiffLoadContext;

struct TiffDirectoryContext {
    bool valid;
    int index;
    Image::InitInfo ii;  // Image information
    Image::Dict dict;

    bool isTiled;
    Size<int> tileSize;
    int tilesCount;
    int tileUncopressedSize;
    int tileBytesPerRow;

    TiffLoadContext* loadContext;
};

struct TiffLoadContext {
    std::unique_ptr<LibTiffLoadContextImpl> pimpl;

    TiffLoadContext() noexcept;
    TiffLoadContext(TiffLoadContext&& other) noexcept;
    TiffLoadContext& operator=(TiffLoadContext&& other) noexcept;
    ~TiffLoadContext() noexcept;

    void swap(TiffLoadContext& other) noexcept {
        std::swap(pimpl, other.pimpl);
    }

    TiffDirectoryContext readDir();
};

struct TiffTilesInfo {
    int tilesCount;
    std::vector<int> tilesSizes;
    int jpegTableLength;
    ByteBuffer jpegTable;
};


}  // namespace impl

struct LibTiff {
    using LoadParams = ImageFile::LoadParams;

    static void load(StrView filename, ImageContainer* ic, LoadParams params={}) {
        loadCore(ic, nullptr, filename, params);
    }

    static Image loadImage(StrView filename, LoadParams params={}) {
        Image img;
        loadCore(nullptr, &img, filename, params);
        return img;
    }

    static ImageContainer loadInfo(StrView filename, LoadParams params={}) {
        ImageContainer ic;
        params.infoOnly = true;
        loadCore(&ic, nullptr, filename, params);
        ic.processImagesMaintenance();
        return ic;
    }

    static impl::TiffLoadContext startLoadProcess(StrView filename, LoadParams params={});
    static impl::TiffDirectoryContext loadDirectory(impl::TiffLoadContext& loadContext);
    static bool decodeTile(impl::TiffDirectoryContext& dir, Point<int> tilePos, Image& img, Point<int> imgPos);
    static impl::TiffTilesInfo getTilesInfo(impl::TiffDirectoryContext& dir);
    static Index readRawTile(impl::TiffDirectoryContext& dir, const impl::TiffTilesInfo& ti, Point<int> tileIndex, ByteBuffer& buffer);
    static void finishLoadProcess(impl::TiffLoadContext& loadContext);

private:
    // loads either container or an image
    static void loadCore(ImageContainer* ic, Image* img, StrView filename, const LoadParams& params);
};

}  // namespace apl


