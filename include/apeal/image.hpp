/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#if DESC_Image

Simple Image Manipulation
---------


Class is designed for simple image formats mostly used for user interface and
simple import/export of images.

#endif


#include "geo.hpp"
#include "buffer.hpp"

#include <array>
#include <map>
#include <unordered_map>
#include <concepts>

namespace apl {

using ImageIndex = int;


enum class ColorModel {
    None = 0,
    Mono,   // Monochrome: bit mask

    // Grayscale formats
    Grey,
    Gray = Grey,   // Floating point values

    // RGB family of formats
    RGB, RGBA, BGR, BGRA,  // Various RGB formats - 8 bits per channel
    RGB565, RGB555, RGB4,

    YCC,

    YB12,
};

enum class ImageValueType {
    Unknown = 0,
    Bit,  // 1/0
    U8, U16, U32,  // unsigned integer
    S8, S16, S32,  // signed integer
    F16, F32, F64,  // float
};

// Image format specific textual information prepended by file ext
using ImageDict = std::unordered_map<String, String>;

struct ImagePlane {
    ImageValueType val;  // Value type
    Size<int> size;  // Dimensions
    int prec;  // Precision (bits per sample)
    int bpr;  // Bytes per row
    ByteBuffer buffer;

    [[nodiscard]] auto data() noexcept {  return buffer.data();  }
    [[nodiscard]] auto data() const noexcept {  return buffer.data();  }
};


struct ImageSubSampling {
    ImageIndex j, a, b;
    ImageIndex alpha;

    ImageIndex bytesPerRow(ImageIndex width, ImageIndex depth) const noexcept {
        return width * (j + a + b + alpha) * depth / (j/2) / 8;
    }

    ImageIndex rowsPerHeight(ImageIndex height) const noexcept {
        return b ? height : height / 2;
    }

    ImageIndex dx() const noexcept { return j / a; }
    ImageIndex dy() const noexcept { return b ? 1 : 2; }
};

struct Image;
using ImageA = const Image&;

struct Image {
    using PointI = Point<ImageIndex>;
    using SizeI = Size<ImageIndex>;
    using RectI = Rect<ImageIndex>;

    using Dim = SizeI;
    using Dict = ImageDict;
    using Model = ColorModel;
    using ValueType = ImageValueType;

    SizeI size{};  // image dimensions

    ColorModel cm{};  // color model

    ValueType val{};  // value type
    int depth{};  // the type size in bits: 1, 8, 16, 32
    int prec{};  // How many bits actually carry information

    ImageSubSampling sub{};  // subsampling

    ImageIndex bytesPerSample{};
    ImageIndex bytesPerRow{};

    //
    // Information about components
    //
    struct CompsLayout {
        int r{-1}, g{-1}, b{-1}, a{-1};
        int h{-1}, s{-1}, y{-1}, u{-1}, v{-1}, c{-1};
    };

    CompsLayout compsLayout{};
    constexpr static Index MaxComps = 5;
    ImageIndex numComps{};  // number of color channels.

    // numPlanes is 0 if the image representation is not planar
    constexpr static Index MaxPlanes = 5;
    ImageIndex numPlanes{};  // Number of planes.
    VectorCont<ImagePlane> planes;

    ImageDict dict{};  // Dictionary of various strings

    ByteBuffer buffer{};
    bool dataLoaded{};


    auto getSize() const noexcept { return size; }
    auto width() const noexcept { return size.w; }
    auto height() const noexcept { return size.h; }

    auto bpp() const noexcept { return bytesPerSample; }
    auto bps() const noexcept { return bytesPerRow; }
    auto bpr() const noexcept { return bytesPerRow; }

    auto dataSize() const noexcept { return bytesPerRow * size.h; }


    constexpr static ImageIndex alignBytesPerRow32(ImageIndex bpr) noexcept {
        // bytes per row are always aligned by 32 bits (4 bytes)
        if (bpr & 0x3)
            bpr = (bpr & ~0x3) + 4;
        return bpr;
    }

    constexpr static ImageIndex bytesForDepth(ImageIndex depth) noexcept {
        return (depth + 7) / 8;
    }



    bool isPlanar() const noexcept { return !planes.empty(); }



    template <class T = std::uint8_t>
    AForceInline T *pixelData() noexcept {
        return reinterpret_cast<T*>(buffer.data());
    }

    template <class T = std::uint8_t>
    AForceInline const T *pixelData() const noexcept {
        return reinterpret_cast<const T*>(buffer.data());
    }


    //
    // Initialization, setup
    //
    struct InitInfo {
        Model cm;
        SizeI size;
        ValueType val;
        ImageSubSampling sub;
        int prec, depth, bpp, bpr;
        int planes;
    };

    void init(const InitInfo& ii) noexcept {
        cm = ii.cm;
        size = ii.size;
        val = ii.val;
        sub = ii.sub;
        prec = ii.prec;
        depth = ii.depth;
        bytesPerSample = ii.bpp;
        bytesPerRow = ii.bpr;
        numPlanes = ii.planes;
        processMaintenance();
    }

    // Convenient shortcut
    void init(Model icm, SizeI sz, ValueType ival = ValueType::U8) noexcept {
        init({.cm = icm, .size = sz, .val = ival});
    }


    void create(const InitInfo& ii) {
        init(ii);
        alloc();
    }

    void alloc() {
        if (isPlanar()) {
            for (auto& pl: planes) {
                pl.buffer.alloc(pl.size.h * pl.bpr);
            }
        } else {
            buffer.alloc(dataSize());
        }
    }

    static Image make(const InitInfo& ii) {
        Image img;
        img.create(ii);
        return img;
    }

    // Convenient shortcut
    static Image make(Model icm, Dim idim, ValueType ival = ValueType::U8) noexcept {
        return make({.cm = icm, .size = idim, .val = ival});
    }



    void swap(Image& b) noexcept {
        using std::swap;

        swap(size, b.size);
        swap(planes, b.planes);
        swap(cm, b.cm);
        swap(val, b.val);
        swap(depth, b.depth);
        swap(prec, b.prec);
        swap(sub, b.sub);

        swap(bytesPerSample, b.bytesPerSample);
        swap(bytesPerRow, b.bytesPerRow);

        swap(compsLayout, b.compsLayout);
        swap(numComps, b.numComps);

        swap(numPlanes, b.numPlanes);
        swap(planes, b.planes);

        swap(dict, b.dict);

        swap(buffer, b.buffer);
        swap(dataLoaded, b.dataLoaded);
    }

    friend void swap(Image& a, Image& b) noexcept { a.swap(b); }


    //
    // Information inquiry
    //
    //auto getSize() const noexcept { return size;  }
    auto getColorModel() const noexcept { return cm; }

    // Format information
    bool isRGBA() const noexcept { return ColorModel::RGBA == cm;  }
    bool isRGB() const noexcept { return ColorModel::RGB == cm;  }
    bool isYCC() const noexcept { return ColorModel::YCC == cm;  }


    static StrView colorModelTag(ColorModel c) noexcept;
    StrView colorModelTag() const noexcept { return colorModelTag(cm); }

    void processMaintenance();


    //
    // Pixel data access
    //

    void allocateData();

    auto data() noexcept {  return buffer.data();  }
    auto data() const noexcept {  return buffer.data();  }
    auto getData() noexcept {  return buffer.data();  }
    auto getData() const noexcept {  return buffer.data();  }

    ImageIndex calcBytesPerSample() const noexcept {
        ImageIndex bytes{};

        switch (cm) {
        case Model::Gray:
            bytes = 1;
            break;

        case Model::RGB:
        case Model::BGR:
            bytes = 3;
            break;

        case Model::RGBA:
        case Model::BGRA:
            bytes = 4;
            break;

        case Model::YCC:
            bytes = 3;
            break;

        default :
            ADebugTrap();
            break;
        }

        return bytes * depth / 8;
    }

    ImageIndex calcBytesPerRow() const noexcept {
        return alignBytesPerRow32(bytesPerSample * size.w);
    }

    AForceInline ImageIndex pixelRowIndex(ImageIndex row) const noexcept {
        return row * bytesPerRow;
    }

    AForceInline ImageIndex pixelColIndex(ImageIndex col) const noexcept {
        return col * bytesPerSample;
    }


    template <class T = std::uint8_t>
    AForceInline auto pixelRowData(ImageIndex row) noexcept {
        return reinterpret_cast<T *>(buffer.data() + pixelRowIndex(row));
    }

    template <class T = std::uint8_t>
    AForceInline auto pixelRowData(ImageIndex row) const noexcept {
        return reinterpret_cast<const T *>(buffer.data() + pixelRowIndex(row));
    }


    template <class T = std::uint8_t, class P>
    AForceInline auto pixelPosData(Point<P> pos) noexcept {
        return reinterpret_cast<T *>(buffer.data() + pixelRowIndex(pos.y) + pixelColIndex(pos.x));
    }

    template <class T = std::uint8_t, class P>
    AForceInline auto pixelPosData(Point<P> pos) const noexcept {
        return reinterpret_cast<const T*>(buffer.data() + pixelRowIndex(pos.y) + pixelColIndex(pos.x));
    }

    template <class T = std::uint8_t>
    AForceInline auto pixelPosData(std::integral auto x, std::integral auto y) noexcept {
        return reinterpret_cast<T*>(buffer.data() + pixelRowIndex(y) + pixelColIndex(x));
    }

    template <class T = std::uint8_t>
    AForceInline auto pixelPosData(std::integral auto x, std::integral auto y) const noexcept {
        return reinterpret_cast<const T*>(buffer.data() + pixelRowIndex(y) + pixelColIndex(x));
    }


    //
    // Pixel manipulation
    //
    void zero() noexcept {
        std::memset(buffer.data(), 0, dataSize());
    }

    void fillRGBA(unsigned hex) noexcept {
        auto color = (uint32_t)hex;

        auto ptr = buffer.get();

        for (auto row = 0; row < size.h; ++row, ptr += bytesPerRow) {
            auto p = (uint32_t *)ptr;
            for (auto col = 0; col < size.w; ++col, ++p) {
                *p++ = color;
            }
        }
    }

    // Image Conversion
    struct ConvertParams {
    };

    // Converts to created 
    void convert(Image& to, ConvertParams parms={}) const;

    // Returns newly created converted image
    Image clone(ColorModel f = ColorModel::None, ConvertParams parms={}) const {
        if (f == ColorModel::None) {
            f = cm;
        }

        //    // TODO: not implemented
        //    //ADebugTrap();
        //    auto to = make({.cm = f, .size = size});
        //    std::memcpy(to.buffer.get(), buffer.data(), to.dataSize());
        //    return to;
        //    //return make({.cm = f});
        //}

        auto to = make({.cm = f, .size = size, .planes = numPlanes});
        convert(to, parms);
        return to;
    }

    // Various conversion primitives
    struct ConvertContext {
        SizeI size{};

        template <class DataType>
        struct Plane {
            ColorModel cm;
            ValueType val;
            DataType* data;
            Index bpr;
        };

        //// Description
        //struct Data {
        //    Index pc{1}; // planes count

        //};

        Index spc{1};  // planes count
        //VectorCont<Plane> sp;
        std::array<Plane<const uint8_t>, MaxPlanes> sp{};

        Index dpc{1};  // planes count
        std::array<Plane<uint8_t>, MaxPlanes> dp{};
    };

    static void convertCore(const ConvertContext& ctx);

    /*
    // r, g, b must be normalized: 0:1
    static std::tuple<float, float, float> rgb_to_hsv(float r, float g, float b) noexcept
    {
        auto [mn, mx] = std::minmax({r, g, b});
        auto c = mx - mn;

        if (mx == 0)
            return {};

        if (c == 0)
            return {0.f, 0.f, mx - mn / 2};

        float h{};

        if (mx == r) {
            auto seg = (g - b) / c;
            auto shift = 0;
            if (seg < 0)
                shift = 6;  // 360/6
            h = seg + shift;

            //h = std::fmod(std::abs(g - b) / c, 6.f);
            //h = std::fmod((g - b) / c + 60.f, 6.f);
        } else if (mx == g)
            h = (b - r) / c + 2;  // 120/60
        else if (mx == b) {
            h = (r - g) / c + 4;  // 240/60
        }


        h *= 60.f;

        //if (h< 0) {
        //    int a = 0;
        //}


        auto s = c / mx;

        auto v = (mx + mn) / 2;
        //auto v = (r + g + b) / 3;

        return {h, s, v};
    }
    */

    /*
    static std::tuple<float, float, float> hue_to_rgb_int(float hue) noexcept
    {
        using Rgb = std::tuple<float, float, float>;

        if (hue >= 0 && hue < 60) {
            auto l = hue / 60;
            return {1.f, l, 0.f};
        }

        if (hue >= 60 && hue < 120) {
            auto l = (hue - 60)/ 60;
            return {1.f - l, 1.f, 0.f};
        }

        if (hue >= 120 && hue < 180) {
            auto l = (hue - 120) / 60.f;
            return {0.f, 1.f, l};
        }

        if (hue >= 180 && hue < 240) {
            auto l = (hue - 180) / 60;
            return {0.f, 1 - l, 1.f};
        }

        if (hue >= 240 && hue < 300) {
            auto l = (hue - 240) / 60;
            return {l, 0.f, 1.f};
        }

        if (hue >= 300 && hue <= 360) {
            auto l = (hue - 300)/ 60;
            return {1.f, 0.f, 1 - l};
        }


        return {};
    }
    */
};

inline Image createImage(const Image::InitInfo& ii) {
    Image img;
    img.create(ii);
    return img;
}

inline Image initImage(const Image::InitInfo& ii) {
    Image img;
    img.init(ii);
    return img;
}


enum class ImageFileFormat {
    Unknown = 0,
    Bmp, Ico, Icns, Jpeg, Jp2, Jpx, J2k, Png, Tiff,
};


struct ImageContainer {

    ImageFileFormat fileFormat{};
    ImageDict dict;

    VectorCont<Image> images;
    VectorCont<std::pair<int, int>> byHeight;  // Image indices sorted by height in descending order

    void clear() {
        fileFormat = {};
        dict.clear();
        images.clear();
        byHeight.clear();
    }

    //
    // Creating and adding images
    //
    Index numImages() const noexcept {  return std::ssize(images);  }

    auto& emplaceImage(Image&& image={}) {
        auto& img = images.emplace_back(std::move(image));
        processImagesMaintenance();
        return img;
    }

    auto& lastImage() {  return images.back();  }
    auto& lastImage() const {  return images.back();  }


    void processImagesMaintenance();


    //
    // Image status
    //
    bool valid() const noexcept {
        if (images.empty())
            return false;
        return ColorModel::None != images.front().cm;
    }
    explicit operator bool() const { return valid(); }

    void verifyValid() const {
        verify(valid(), "Image is not loaded");
        verify(!byHeight.empty(), "Images are not sorted properly");
    }

    void verifyIndex(Index index = 0) const {
        verify(index >= 0 && index < std::ssize(images), "Invalid image index: %d", index);
    }



    //
    // Access images
    //
    auto& front() {
        verifyValid();
        return images.front();
    }

    auto& front() const {
        verifyValid();
        return images.front();
    }

    auto& getImage(Index index) {
        verifyValid();
        verifyIndex(index);
        return images.at(index);
    }

    auto& getImage(Index index) const {
        verifyValid();
        verifyIndex(index);
        return images.at(index);
    }


    auto& firstByHeight() {
        verifyValid();
        auto index = byHeight.begin()->second;
        verifyIndex(index);
        return images[index];
    }

    auto& firstByHeight() const {
        verifyValid();
        auto index = byHeight.begin()->second;
        verifyIndex(index);
        return images[index];
    }

    auto& lastByHeight() {
        verifyValid();
        auto index = byHeight.rbegin()->second;
        verifyIndex(index);
        return images[index];
    }

    auto& lastByHeight() const {
        verifyValid();
        auto index = byHeight.rbegin()->second;
        verifyIndex(index);
        return images[index];
    }

    // common tags
    constexpr static auto descriptionTag = "description";
    constexpr static auto artistTag = "artist";
};

using ImageContainerA = const ImageContainer&;

struct ImageFile {
    struct LoadParams {
        ImageFileFormat ffmt;  // file format
        bool oneOnly; // load only one image
        Index index;  // which image to load
        bool infoOnly;  // load only info, will not allocate all load the data
    };

    struct SaveParams {
        ImageFileFormat ffmt;  // file format
        Index index;  // which image to save
        bool lossy;
        bool flat;  // no tiles
        Size<int> tileSize;  // tile dimensions

        int jpegQuality;  // 80 is common

        int jp2NumLayers;  // 2-3 usually
        float jp2Psnr[3]; // max 3. 30, 40 usually is a good number
    };


    static void load(StrView filename, ImageContainer* ic, LoadParams params = {});

    static ImageContainer load(StrView filename, LoadParams params={}) {
        ImageContainer ic;
        load(filename, &ic, params);
        return ic;
    }

    static ImageContainer loadInfo(StrView filename, LoadParams params={}) {
        params.infoOnly = true;
        return load(filename, params);
    }

    static void loadImage(StrView filename, Image* image, LoadParams parms={}) {
        parms.oneOnly = true;
        ImageContainer ic;
        load(filename, &ic, parms);
        *image = std::move(ic.front());
    }

    static Image loadImage(StrView filename, LoadParams parms={}) {
        parms.oneOnly = true;
        auto ic = load(filename, parms);
        return std::move(ic.front());
    }


    static void loadBmp(StrView filename, ImageContainer* ic, LoadParams params={});
    static void loadIcns(StrView filename, ImageContainer* ic, LoadParams params={});  // Apple icon format
    static void loadIco(StrView filename, ImageContainer* ic, LoadParams lp={});  // Windows icon format
    static void loadJpeg(StrView filename, ImageContainer* ic, LoadParams parms={});
    static void loadJp2(StrView filename, ImageContainer* ic, LoadParams parms={});
    static void loadPng(StrView filename, ImageContainer* ic, LoadParams parms={});
    static void loadTiff(StrView filename, ImageContainer* ic, LoadParams parms={});


    static void save(StrView filename, ImageA image, SaveParams parms={});
    static void save(StrView filename, ImageContainerA image, SaveParams parms={});

    static void saveBmp(StrView filename, ImageA image, SaveParams parms={});

    static void saveIco(StrView filename, ImageA image, SaveParams parms={});
    static void saveIco(StrView filename, ImageContainerA image, SaveParams parms={});

    static void saveIcns(StrView filename, ImageA image, SaveParams parms={});

    static void saveJpeg(StrView filename, ImageA image, SaveParams parms={});
    static void saveJpeg(StrView filename, ImageContainerA image, SaveParams parms={});

    static void saveJp2(StrView filename, ImageA image, SaveParams parms={});
    static void saveJp2(StrView filename, ImageContainerA image, SaveParams parms={});

    static void savePng(StrView filename, ImageA image, SaveParams parms={});
    static void savePng(StrView filename, ImageContainerA image, SaveParams parms={});

    static void saveTiff(StrView filename, ImageA image, SaveParams parms={});


    static StrView fileFormatExt(ImageFileFormat fmt);
    static ImageFileFormat detectFileFormat(StrView loc);
    static ImageFileFormat guessFileFormat(StrView loc);
    //static void processImageInfo(ImageInfo& ii);
};

inline
Image loadImage(StrView filename, ImageFile::LoadParams parms={}) {
    return ImageFile::loadImage(filename, parms);
}

inline
void saveImage(StrView filename, ImageA image, ImageFile::SaveParams parms={}) {
    ImageFile::save(filename, image, parms);
}

// Copies subimage
inline
void copySubImage(const Image& src, Point<int> dstPos, Image* dst) {
    verify(src.cm == dst->cm, "Source and destination color modes must match");

    verify(dst->size.w >= dstPos.x + src.size.w, "Destination image width is small");
    verify(dst->size.h >= dstPos.y + src.size.h, "Destination image height is small");

    auto psrc = src.pixelRowData(0);
    auto pdst = dst->pixelPosData(dstPos);
    auto srcBytesPerRow = src.bytesPerRow;
    auto dstBytesPerRow = dst->bytesPerRow;

    for (int y = 0; y < src.size.h; y++) {
        std::memcpy(pdst, psrc, srcBytesPerRow);

        psrc += srcBytesPerRow;
        pdst += dstBytesPerRow;
    }
}

inline void getSubImage(const Image& src, const Rect<int>& rect, Image* dst) {
    verify(src.cm == dst->cm, "Source and destination color modes must match");

    verify(src.size.w >= rect.right(), "Source image area is out of bounds");
    verify(src.size.h >= rect.top(), "Source image area is out of bounds");

    verify(dst->size.w >= rect.w, "Destination image width is small");
    verify(dst->size.h >= rect.h, "Destination image height is small");


    auto psrc = src.pixelPosData(rect.position());
    auto pdst = dst->data();
    auto srcBytesPerRow = src.bytesPerRow;
    auto dstBytesPerRow = dst->bytesPerRow;

    for (int y = 0; y < rect.h; y++) {
        std::memcpy(pdst, psrc, rect.w * src.bytesPerSample);

        psrc += srcBytesPerRow;
        pdst += dstBytesPerRow;
    }
}

}  // namespace apl
