/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/image/libpng.hpp"

#if HAVE_LIBPNG

#include "../include/apeal/file.hpp"

#include "../include/apeal/debug.hpp"
#include "../include/apeal/assert.hpp"


//#include <png.h>
#include "../3rdparty/libpng/png.h"

#include <cstdio>

namespace apl {

static void on_png_read_data(png_structp png, png_bytep data, png_size_t length)
{
    auto& file = *static_cast<File *>(png_get_io_ptr(png));
    file.read(data, length);
}

static void on_png_write_data(png_structp png, png_bytep data, png_size_t length)
{
    auto& file = *static_cast<File *>(png_get_io_ptr(png));
    file.write(data, length);
}

static void on_png_flush_data(png_structp png)
{
    auto& file = *static_cast<File *>(png_get_io_ptr(png));
    file.flush();
}

static void on_png_error(png_structp, png_const_charp serror)
{
    throwError("%s", serror);
}

static void on_png_warn(png_structp, png_const_charp swarn)
{
    // TODO: add to dictionary
    ADebugPrintFunc("libpng::warning: %s", swarn);
}




void LibPng::loadCore(StrView filename, bool infoOnly, LoadContext& ctx, ImageContainer* ic) {
    ic->clear();

    png_struct *png{};
    png_info *info{};

    auto cleanup = [&] {
        if (png) {
            png_destroy_read_struct(&png, &info, nullptr);
            png = nullptr;
        }
        info = nullptr;
    };

    TempBuffer localBuffer;

    try {
        File file;
        file.open(filename, FileAccess::Read);

        //int a = PNG_SIG_BYTES ;

        constexpr int PngSignatureLength = 8;
        png_byte sig[PngSignatureLength];
        file.read(sig, PngSignatureLength);

        verify(png_check_sig(sig, PngSignatureLength), "Invalid signature");
        //throw std::runtime_error("not png: " + filename) ;


        png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, &on_png_error, on_png_warn);
        verify(png, "png_create_read_struct");

        info = png_create_info_struct(png);
        verify(info, "png_create_info_struct");

        png_set_read_fn(png, &file, on_png_read_data);

        png_set_sig_bytes(png, PngSignatureLength);
        png_read_info(png, info);


        png_uint_32 pngWidth{}, pngHeight{};
        int pngBitDepth{}, pngFormat{};

        png_get_IHDR(png, info, &pngWidth, &pngHeight, &pngBitDepth, &pngFormat, nullptr, nullptr, nullptr);

        png_read_update_info(png, info);

        auto& image = ic->emplaceImage();

        image.size = Image::SizeI::make(pngWidth, pngHeight);
        //colorType = color_type ;


        /*
        int transform = PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND ;
        png_read_png ( png, info, transform, NULL ) ;

        size.w = png_get_image_width(png,info) ;
        size.h = png_get_image_height(png,info) ;
        auto pngFormat = png_get_color_type(png,info) ;


        png_bytepp rows = png_get_rows(png,info) ;
        */

        switch (pngFormat) {
        case PNG_COLOR_TYPE_GRAY: image.cm = ColorModel::Gray;  break;
        case PNG_COLOR_TYPE_RGB:  image.cm = ColorModel::RGB;   break;
        case PNG_COLOR_TYPE_RGBA: image.cm = ColorModel::RGBA;  break;
        default: throwError("Unsupported png image format: %d", pngFormat);
        }

        auto pngBytesPerRow = png_get_rowbytes(png, info);

        image.bytesPerRow = static_cast<ImageIndex>(pngBytesPerRow);

        image.processMaintenance();

        if (infoOnly) {
            cleanup();
            return;
        }

        auto rowPointers = ctx.tempBuffer ? ctx.tempBuffer : &localBuffer;

        rowPointers->alloc(pngHeight);

        image.buffer.alloc(image.bytesPerRow * image.size.h);

        uint8_t *ppix = image.data();
        auto bpr = image.bpr();

        auto prow = rowPointers->data();

        for (Index j{}; j < image.size.h; ++j, ++prow, ppix += bpr) {
            *prow = ppix;
        }

        png_read_image(png, rowPointers->data());
    }
    catch (const std::exception& e) {
        cleanup();
        throwError("%s: %s", filename, e.what());
    }
    catch (...) {
        cleanup();
        throwError("%s: %s", filename, "Unhandled exception!");
    }

    cleanup();
}


void LibPng::saveCore(StrView filename, SaveContext& ctx)
{
    png_structp png{};
    png_infop pinfo{};

    auto cleanup = [&] {
        if (png) {
            png_destroy_write_struct(&png, &pinfo);
            png = nullptr;
            pinfo = nullptr;
        }
    };

    auto& image = *ctx.image;

    auto pngBitDepth = (png_uint_32)image.depth;
    //auto pngBytesPerPixel = (png_uint_32)image.bpp();

    png_uint_32 pngColorType{};

    switch (image.cm) {
    case ColorModel::RGB:  pngColorType = PNG_COLOR_TYPE_RGB;  break;
    case ColorModel::RGBA: pngColorType = PNG_COLOR_TYPE_RGBA; break;
    case ColorModel::Gray: pngColorType = PNG_COLOR_TYPE_GRAY; break;
    default: throwError("PNG:Unsupported pixel format");
    }

    TempBuffer localBuffer;
    auto rowsBuffer = ctx.tempBuffer ? ctx.tempBuffer : &localBuffer;

    rowsBuffer->alloc(image.size.h);
    

    //bool wasException = false;
    //std::string errorMessage;

    try {
        File file;
        file.create(filename, FileCreate::CreateAlways, FileAccess::Write);


        png = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        verify(png, "PNG png_create_write_struct");

        pinfo = png_create_info_struct(png);
        verify(pinfo, "PNG png_create_info_struct");

        png_set_write_fn(png, &file, on_png_write_data, on_png_flush_data);

        png_set_IHDR(png, pinfo,
            static_cast<png_uint_32>(image.size.w), // width,
            static_cast<png_uint_32>(image.size.h),  // height,
            pngBitDepth, //bit_depth,
            pngColorType, // color_type,
            PNG_INTERLACE_NONE, //interlace_type,
            PNG_COMPRESSION_TYPE_DEFAULT, //compression_type,
            PNG_FILTER_TYPE_DEFAULT // filter_method
        );


        //auto pngBytesPerRow = png_get_rowbytes(png, pinfo);

        auto srow = (uint8_t *)image.data();
        auto sbpr = image.bpr();
        auto prow = rowsBuffer->data();

        for (Index j{}; j < image.size.h; ++j) {
            *prow = srow;
            ++prow;
            srow += sbpr;
        }


        png_write_info(png, pinfo);
        png_write_image(png, rowsBuffer->data());
        png_write_end(png, 0);
    }
    catch (const std::exception& e) {
        cleanup();
        throwError("%s: %s", filename, e.what());
    }
    catch (...) {
        cleanup();
        throwError("%s: %s", filename, "Unhandled exception!");
    }

    cleanup();
}


}  // namespace apl

//#else
//
//void LibPng::loadCore(StrView filename, bool infoOnly, LoadContext& ctx)
//{
//    throwError("File is png but HAVE_LIBPNG is not defined: %s", extractBaseName(filename));
//}
//
//void LibPng::saveCore(StrView filename, SaveContext& ctx)
//{
//    throwError("File is png but HAVE_LIBPNG is not defined: %s", extractBaseName(filename));
//}

#endif // if have libpng
