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

using ImageConvertRoutine = void (*)(const Image::ConvertContext& ctx);

template <typename SType, typename DType>
static void cvt_RGB_to_RGBA(const Image::ConvertContext& ctx) noexcept
{
    // Crude, inefficient. 
    // Use probably some libraries, or hardware
    //
    auto srow = ctx.sp[0].data;
    auto sbpr = ctx.sp[0].bpr;

    auto drow = ctx.dp[0].data;
    auto dbpr = ctx.dp[0].bpr;

    auto [width, height] = ctx.size;

    for (Index row{}; row < height; ++row) {
        auto src = (const SType*)srow;
        auto dst = (DType *)drow;

        // TODO: implement better
        //   implement with 32 bit operations *d32 = {rgb, 0xff}
        //   or maybe 64 bit version. 
        for (Index i{}; i < width; ++i) {
            *dst++ = *src++;
            *dst++ = *src++;
            *dst++ = *src++;
            *dst++ = 0xff;  // Alpha
        }

        srow += sbpr;
        drow += dbpr;
    }
}

template <typename SType, typename DType>
static void cvt_RGBA_to_BGRA(const Image::ConvertContext& ctx) noexcept
{
    // Crude, inefficient.
    // Use probably some libraries, or hardware
    //
    auto srow = ctx.sp[0].data;
    auto sbpr = ctx.sp[0].bpr;
    
    auto drow = ctx.dp[0].data;
    auto dbpr = ctx.dp[0].bpr;
    
    auto [width, height] = ctx.size;
    
    for (Index row{}; row < height; ++row) {
        auto src = (const SType*)srow;
        auto dst = (DType *)drow;
        
        // TODO: implement better
        //   implement with 32 bit operations *d32 = {rgb, 0xff}
        //   or maybe 64 bit version.
        for (Index i{}; i < width; ++i) {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            dst[3] = src[3];
            dst += 4;
            src += 4;
        }
        
        srow += sbpr;
        drow += dbpr;
    }
}


template <class SType, class DType>
static void cvt_RGB_to_planar(const Image::ConvertContext& ctx) noexcept
{
    auto [width, height] = ctx.size;

    auto srow = ctx.sp[0].data;
    auto sbpr = ctx.sp[0].bpr;

    auto drow1 = ctx.dp[0].data;
    auto drow2 = ctx.dp[1].data;
    auto drow3 = ctx.dp[2].data;

    auto dbpr1 = ctx.dp[0].bpr;
    auto dbpr2 = ctx.dp[1].bpr;
    auto dbpr3 = ctx.dp[2].bpr;


    for (decltype(height) row = 0; row != height; ++row) {
        auto s = (const SType*)srow;
        auto d1 = (DType*)drow1, d2 = (DType*)drow2, d3 = (DType*)drow3;

        for (decltype(width) col = 0; col != width; ++col) {
            *d1++ = (DType)*s++;
            *d2++ = (DType)*s++;
            *d3++ = (DType)*s++;
        }

        srow += sbpr;
        drow1 += dbpr1, drow2 += dbpr2, drow3 += dbpr3;
    }
}

template <class SType, class DType>
static void cvt_RGBA_to_planar(const Image::ConvertContext& ctx) noexcept
{
    auto srow = ctx.sp[0].data;
    auto sbpr = ctx.sp[0].bpr;

    auto drow1 = ctx.dp[0].data;
    auto drow2 = ctx.dp[1].data;
    auto drow3 = ctx.dp[2].data;
    auto drow4 = ctx.dp[3].data;

    auto dbpr1 = ctx.dp[0].bpr;
    auto dbpr2 = ctx.dp[1].bpr;
    auto dbpr3 = ctx.dp[2].bpr;
    auto dbpr4 = ctx.dp[3].bpr;

    auto [width, height] = ctx.size;

    for (decltype(height) row = 0; row != height; ++row) {
        auto s = (const SType*)srow;
        auto d1 = (DType*)drow1, d2 = (DType*)drow2, d3 = (DType*)drow3, d4 = (DType*)drow4;

        for (decltype(width) col = 0; col != width; ++col) {
            *d1++ = (DType)*s++;
            *d2++ = (DType)*s++;
            *d3++ = (DType)*s++;
            *d4++ = (DType)*s++;
        }

        srow += sbpr;
        drow1 += dbpr1, drow2 += dbpr2, drow3 += dbpr3, drow4 += dbpr4;
    }
}


template <class SType, class DType>
static void cvt_YCCI420_to_planar(const Image::ConvertContext& ctx) noexcept
{
    auto [width, height] = ctx.size;

    auto srow = ctx.sp[0].data;
    auto sbpr = ctx.sp[0].bpr;

    auto ybpr = width * sizeof(DType);
    auto cbpr1 = width * sizeof(DType) / 2;
    auto cbpr2 = width * sizeof(DType) / 2;


    auto yrow1 = ctx.dp[0].data;
    auto yrow2 = yrow1 + ybpr;

    auto crow1 = ctx.dp[1].data;
    auto crow2 = ctx.dp[2].data;

    auto processRows = height/2;
    auto processCols = width/2;

    for (decltype(height) j = 0; j < processRows; ++j) {
        auto s = (const SType*)srow;
        auto y1 = (DType*)yrow1, y2 = (DType*)yrow2;
        auto c1 = (DType*)crow1, c2 = (DType*)crow2;

        for (decltype(width) i = 0; i < processCols; ++i) {
            // 4 bytes of luma
            *y1++ = (DType)*s++;
            *y1++ = (DType)*s++;
            *y2++ = (DType)*s++;
            *y2++ = (DType)*s++;

            // 2 bytes of chroma
            *c1++ = (DType)*s++;
            *c2++ = (DType)*s++;
        }

        srow += sbpr;
        yrow1 += ybpr*2, yrow2 += ybpr*2, crow1 += cbpr1, crow2 += cbpr2;
    }
}


// Very slow implementation. Just for testing purposes
template <class SType, class DType>
static void cvt_YCCI420_to_RGBA_fp(const Image::ConvertContext& ctx) noexcept
{
    auto [width, height] = ctx.size;
    using F = float;

    auto srow = ctx.sp[0].data;
    auto sbpr = ctx.sp[0].bpr;

    auto drow = ctx.dp[0].data;
    auto dbpr = ctx.dp[0].bpr;
    auto dbpr2 = dbpr*2;
    auto dbpp = 4*sizeof(DType);

    struct PixelQuad {
        F y;
        DType* p;
    };

    std::array<PixelQuad, 4> quad;

    constexpr auto dMaxVal = std::numeric_limits<DType>::max();

    auto processRows = height/2;
    auto processCols = width/2;

    for (decltype(height) j = 0; j < processRows; ++j) {
        auto sptr = (const SType*)srow;

        quad[0].p = (DType*)(drow);
        quad[1].p = (DType*)(drow + dbpp);
        quad[2].p = (DType*)(drow + dbpr);
        quad[3].p = (DType*)(drow + dbpr + dbpp);

        for (decltype(width) i = 0; i < processCols; ++i) {

            // 4 bytes of luma
            quad[0].y = (F)*sptr++;
            quad[1].y = (F)*sptr++;
            quad[2].y = (F)*sptr++;
            quad[3].y = (F)*sptr++;

            // 2 bytes of chroma
            auto cb = (F)*sptr++;
            auto cr = (F)*sptr++;

            auto cr128 = cr - 128.f;
            auto cb128 = cb - 128.f;
            auto rK = 1.402f * cr128;
            auto gK = - 0.344136286f * cb128 - 0.714136286f * cr128;
            auto bK = 1.772f * cb128;


            for (auto&& q: quad) {

                auto rf = q.y + rK; //                              + 1.402f * (cr - 128.f);
                auto gf = q.y + gK; // - 0.344136f * (cb - 128.f) - 0.714136f * (cr - 128.f);
                auto bf = q.y + bK; // +    1.772f * (cb - 128.f);

                auto r = (DType)rf;
                auto g = (DType)gf;
                auto b = (DType)bf;
                //auto r = (DType)std::clamp(rf, 0.f, 255.f);
                //auto g = (DType)std::clamp(gf, 0.f, 255.f);
                //auto b = (DType)std::clamp(bf, 0.f, 255.f);

                *q.p++ = r;
                *q.p++ = g;
                *q.p++ = b;
                *q.p++ = dMaxVal;  // alpha

                q.p += 4; // skip next
            }
        }

        srow += sbpr;
        drow += dbpr2;
    }
}

// TODO: have a bug at some points (file 0_93_79 at index 245465)
template <class SType, class DType>
static void cvt_YCCI420_to_RGBA(const Image::ConvertContext& ctx) noexcept
{
    auto [width, height] = ctx.size;

    auto srow = ctx.sp[0].data;
    auto sbpr = ctx.sp[0].bpr;

    auto drow = ctx.dp[0].data;
    auto dbpr = ctx.dp[0].bpr;
    auto dbpr2 = dbpr*2;
    auto dbpp = 4*sizeof(DType);

    struct PixelQuad {
        int32_t y;
        DType* p;
    };

    std::array<PixelQuad, 4> quad;

    constexpr int shift = 14; // 65536 / 4 = 16384
    //constexpr int32_t maxVal = 4177920; // 16384 * 255
    //constexpr int32_t minVal = 0;
    constexpr int32_t half = 128;// << shift;
    constexpr int32_t rcrk = 22970; // 1.402 * 16384
    constexpr int32_t gcrk = 11700; // 16384 * 0.714136
    constexpr int32_t gcbk = 5638; // 16384 * 0.344136
    constexpr int32_t bcbk = 29032;  // 16384 * 1.772



    constexpr auto dMaxVal = std::numeric_limits<DType>::max();

    auto processRows = height/2;
    auto processCols = width/2;

    for (decltype(height) j = 0; j < processRows; ++j) {
        auto sptr = (const SType*)srow;

        quad[0].p = (DType*)(drow);
        quad[1].p = (DType*)(drow + dbpp);
        quad[2].p = (DType*)(drow + dbpr);
        quad[3].p = (DType*)(drow + dbpr + dbpp);

        for (decltype(width) i = 0; i < processCols; ++i) {

            // 4 bytes of luma
            quad[0].y = (int32_t)(((uint32_t)*sptr++) << shift);
            quad[1].y = (int32_t)(((uint32_t)*sptr++) << shift);
            quad[2].y = (int32_t)(((uint32_t)*sptr++) << shift);
            quad[3].y = (int32_t)(((uint32_t)*sptr++) << shift);

            // 2 bytes of chroma
            //auto cb = (int32_t)(((uint32_t)*sptr++) << shift);
            //auto cr = (int32_t)(((uint32_t)*sptr++) << shift);
            auto cb = (int32_t)(uint32_t)*sptr++;
            auto cr = (int32_t)(uint32_t)*sptr++;
            auto cbh = cb - half;
            auto crh = cr - half;

            for (auto&& q: quad) {
                int32_t rf = q.y              + rcrk * crh;
                int32_t gf = q.y - gcbk * cbh - gcrk * crh;
                int32_t bf = q.y + bcbk * cbh;

                //auto rf = q.y                               + 1.402f * (cr - 128.f);
                //auto gf = q.y - 0.344136f * (cb - 128.f) - 0.714136f * (cr - 128.f);
                //auto bf = q.y +    1.772f * (cb - 128.f);

                //auto r = (uint32_t)std::clamp(rf, minVal, maxVal);
                //auto g = (uint32_t)std::clamp(gf, minVal, maxVal);
                //auto b = (uint32_t)std::clamp(bf, minVal, maxVal);
                auto r = (uint32_t)rf;
                auto g = (uint32_t)gf;
                auto b = (uint32_t)bf;

                *q.p++ = (DType)(r >> shift);
                *q.p++ = (DType)(g >> shift);
                *q.p++ = (DType)(b >> shift);
                *q.p++ = dMaxVal;  // alpha

                q.p += 4; // skip next
            }
        }

        srow += sbpr;
        drow += dbpr2;
    }
}




template <class SType, class DType>
static void cvt_YCCI420_from_planar(const Image::ConvertContext& ctx) noexcept
{
    auto [width, height] = ctx.size;

    auto ybpr = width * sizeof(SType);
    auto cbpr = width * sizeof(SType) / 2;

    auto yrow = ctx.sp[0].data;
    auto brow = ctx.sp[1].data;
    auto rrow = ctx.sp[2].data;


    auto drow = ctx.dp[0].data;
    auto dbpr = ctx.dp[0].bpr;

    auto processRows = height / 2;
    auto processCols = width / 2;


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




template <class SType, class DType>
static void cvt_Gray_to_Gray(const Image::ConvertContext& ctx) noexcept
{
    auto srow = ctx.sp[0].data;
    auto sbpr = ctx.sp[0].bpr;

    auto drow = ctx.dp[0].data;
    auto dbpr = ctx.dp[0].bpr;

    auto [width, height] = ctx.size;

    for (decltype(height) j = 0; j != height; ++j) {
        auto s = (const SType*)srow;
        auto d = (DType*)drow;
        for (decltype(width) i = 0; i != width; ++i) {
            *d++ = (DType)*s++;
        }
        srow += sbpr;
        drow += dbpr;
    }
}




struct ImageConvertItem {
    ColorModel c{};
    ImageValueType t{ImageValueType::U8};
    Index p{1};
};

struct ImageConvertTable {
    ImageConvertItem s{}, d{};
    ImageConvertRoutine func{};
};

using CM = ColorModel;
using VT = ImageValueType;

constexpr static ImageConvertTable imageConvertTable[] = {
    {
        .s = {.c=CM::RGB},
        .d = {.c=CM::RGBA},
        .func = cvt_RGB_to_RGBA<uint8_t, uint8_t>
    },
    {
        .s = {.c=CM::RGB}, 
        .d = {.c=CM::RGB, .t=VT::U32, .p=3}, 
        .func = cvt_RGB_to_planar<uint8_t, uint32_t>
    },
    {
        .s = {.c = CM::RGBA},
        .d = {.c = CM::RGBA, .t=VT::U32, .p=4}, 
        .func = cvt_RGBA_to_planar<uint8_t, uint32_t>
    },

    {
        .s = {.c = CM::RGBA},
        .d = {.c = CM::BGRA},
        .func = cvt_RGBA_to_BGRA<uint8_t, uint8_t>
    },


    {
        .s = {.c=CM::YCC}, 
        .d = {.c=CM::YCC, .p=3}, 
        .func = cvt_YCCI420_to_planar<uint8_t, uint8_t>
    },
    {
        .s = {.c = CM::YCC, .p = 3},
        .d = {.c = CM::YCC}, 
        .func = cvt_YCCI420_from_planar<uint8_t, uint8_t>
    },

    {
        .s = {.c = CM::YCC}, 
        .d = {.c = CM::YCC, .t = VT::U32, .p = 3},
        .func = cvt_YCCI420_to_planar<uint8_t, uint32_t>
    },
    {
        .s = {.c=CM::YCC, .t=VT::U32, .p=3}, 
        .d = {.c=CM::YCC}, 
        .func = cvt_YCCI420_from_planar<uint32_t, uint8_t>
    },

    {
        .s = {.c=CM::YCC}, 
        .d = {.c=CM::RGBA}, 
        .func = cvt_YCCI420_to_RGBA_fp<uint8_t, uint8_t>
    },



    {
        .s = {.c=CM::Gray}, 
        .d = {.c=CM::Gray, .t=VT::U32},
        .func = cvt_Gray_to_Gray<uint8_t, uint32_t>
    },
    {
        .s = {.c=CM::Gray, .t=VT::U32},
        .d = {.c=CM::Gray}, 
        .func = cvt_Gray_to_Gray<uint32_t, uint8_t>
    },
};


void Image::convertCore(const ConvertContext& ctx)
{
    auto& src = ctx.sp[0];
    auto& dst = ctx.dp[0];

    bool found{};
    for (auto& tab: imageConvertTable) {
        if (tab.s.c != src.cm || tab.d.c != dst.cm)
            continue;

        if (tab.s.t != src.val || tab.d.t != dst.val)
            continue;

        // Planes count
        if (tab.s.p != ctx.spc || tab.d.p != ctx.dpc)
            continue;

        tab.func(ctx);
        found = true;
        break;
    }

    if (!found) {
        auto stag = Image::colorModelTag(ctx.sp[0].cm);
        auto dtag = Image::colorModelTag(ctx.dp[0].cm);
        throwError("Image Convert not supported: %s -> %s", stag, dtag);
    }
}


void Image::convert(Image& toImage, ConvertParams parms) const
{
    //if (tocm == ColorModel::None)
    //    tocm = toImage.cm;

    //verify(tocm != ColorModel::None, "Unknown target Color Model");

    //toImage.create({.cm = tocm, .size = size});

    // Check if no conversion is necessary
    // TODO: implement for planar
    if (cm == toImage.cm && !isPlanar() && !toImage.isPlanar() && bytesPerRow == toImage.bytesPerRow) {
        std::memcpy(toImage.buffer.get(), buffer.data(), toImage.dataSize());
        return;
    }

    // Prepare conversion parameters
    Image::ConvertContext cvt{};

    cvt.size = size;

    auto preparePlanes = [](auto& img, Index* count, auto& planes) {
        *count = img.numPlanes ? img.numPlanes : 1;
        for (Index i = 0; i < *count; ++i) {
            auto& pl = planes[i];
            pl.cm = img.cm;
            pl.bpr = img.bpr();
            pl.data = img.numPlanes > 1 ? img.planes[i].buffer.data() : img.data();
            pl.val = img.val;
        }
    };

    preparePlanes(*this, &cvt.spc, cvt.sp);
    preparePlanes(toImage, &cvt.dpc, cvt.dp);

    convertCore(cvt);
}

}  // namespace apl


