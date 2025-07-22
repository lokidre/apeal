/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/image/libjpeg.hpp"


#if HAVE_LIBJPEG

//#include <jinclude.h>
#include <jpeglib.h>
#include <jerror.h>

#include <turbojpeg.h>

#include "../include/apeal/file.hpp"

#include "../include/apeal/on_exit.hpp"

#include "../include/apeal/debug.hpp"
#include "../include/apeal/assert.hpp"


namespace apl {

void LibJpeg::loadCore(StrView filename, bool infoOnly, LoadContext& ctx)
{
    auto data = loadFile(filename);
    auto jpegData = (const unsigned char *)data.data();
    auto jpegDataSize = (unsigned long)data.size();


    //throwError("LibJpeg::loadCore not implemented");

    /*
    struct jpeg_decompress_struct dinfo;
    struct jpeg_error_mgr jerr;

    dinfo.err = jpeg_std_error(&jerr);


    jpeg_create_decompress(&dinfo);


    jpeg_mem_src(&dinfo, jpegData, jpegDataSize);

    jpeg_read_header(&dinfo, TRUE);

    dinfo.out_color_space = JCS_YCbCr;


    auto& image = ctx.ic.emplaceImage();

    auto width = dinfo.image_width;
    auto height = dinfo.image_width;

    image.dim = Image::SizeI::make(width, height);


    switch (dinfo.jpeg_color_space) {
    case JCS_YCbCr: image.cm = ColorModel::YCC; break;
    default:
        ADebugTrap();
        break;
    }
    
    image.processMaintenance();

    ctx.ic.fileFormat = ImageFileFormat::Jpeg;

    if (infoOnly)
        return;

    jpeg_start_decompress(&dinfo);

    image.bytesPerRow = width * 3 / 2;

    //image.bytesPerRow = cinfo.output_width * cinfo.output_components;

    image.alloc();


    auto numLines = JDIMENSION(image.dim.h);

    Buffer<unsigned char *> lines{numLines};
    for (auto dest = (unsigned char*)image.data(); auto &l: lines) {
        l = dest;
        dest += image.bytesPerRow;
    }

    auto remaining = numLines;
    auto linePtr = lines.data();
    while (dinfo.output_scanline < height/2) {
        auto count = jpeg_read_scanlines(&dinfo, linePtr, remaining);
        linePtr += count;
        remaining -= count;
    }

    jpeg_finish_decompress(&dinfo);
    jpeg_destroy_decompress(&dinfo);
    */


    tjhandle handle{nullptr};

    OnExit onExit([&] {
        if (handle)
            tjDestroy(handle);
    });


    //jpeg_read_header(


    handle = tjInitDecompress();

    if (!handle)
        throwError("%s: %s", filename, tjGetErrorStr2(handle));

    int width{}, height{};
    int subsamp{}, colorSpace{};

    auto ret = tjDecompressHeader3(handle, jpegData, jpegDataSize,
        &width, &height, &subsamp, &colorSpace
    );

    auto& image = ctx.ic.emplaceImage();

    image.size = Image::SizeI::make(width, height);

    //TJSAMP_420 

    switch (colorSpace) {
    case TJCS_GRAY: image.cm = ColorModel::Gray; break;
    case TJCS_YCbCr: image.cm = ColorModel::YCC; break;
    default:
        ADebugTrap();
        break;
    }

    int pad = 4;

    auto bufSize = (Index)tjBufSizeYUV2(width, pad, height, subsamp);

    //image.bytesPerRow = bufSize / height;

    //image.bytesPerRow = image.dim.w * 3 / 2;
    image.processMaintenance();

    ctx.ic.fileFormat = ImageFileFormat::Jpeg;

    if (infoOnly)
        return;

    image.alloc();


    ByteBuffer yuv{bufSize};

    ret = tjDecompressToYUV2(handle, jpegData, jpegDataSize, 
        yuv.data(), width, pad, height, 0
    );


    using SType = unsigned char;
    using DType = unsigned char;

    auto processRows = height / 2;
    auto processCols = width / 2;

    auto ybpr = width * sizeof(SType);
    auto cbpr = width * sizeof(SType) / 2;

    auto yrow = yuv.data();
    auto brow = yrow + width * height;
    auto rrow = brow + width * height / 4;


    auto drow = image.data();
    auto dbpr = image.bytesPerRow;

    for (decltype(height) j = 0; j < processRows; ++j) {
        auto y1 = (SType*)yrow;
        auto y2 = (SType*)(yrow + ybpr);
        auto b = (SType*)brow;
        auto r = (SType*)rrow;

        auto d = (DType*)drow;

        for (decltype(width) i = 0; i < processCols; ++i) {

            // 4 bytes of luma
            *d++ = (DType)*y1++;
            *d++ = (DType)*y1++;
            *d++ = (DType)*y2++;
            *d++ = (DType)*y2++;

            // 2 bytes of chroma
            *d++ = (DType)*b++;
            *d++ = (DType)*r++;
        }

        yrow += ybpr*2;
        brow += cbpr, rrow += cbpr;

        drow += dbpr;
    }

}

void LibJpeg::saveCore(StrView filename, SaveContext& ctx)
{
    throwError("LibJpeg::saveCore not implemented");

    //tjhandle tj{};

    //auto cleanup = [&] {
    //    if (tj) {
    //        tjDestroy(tj);
    //        tj = nullptr;
    //    }
    //};


    //tj = tjInitCompress();
    //verify(tj, "Error initializing tjInitCompress()");

    //cleanup();
}


}  // namespace apl

#endif
