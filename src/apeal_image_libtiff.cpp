/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/image/libtiff.hpp"

#if HAVE_LIBTIFF
#include <tiffio.h>

#include "../include/apeal/on_exit.hpp"
//#include "../include/apeal/str/sysapi.hpp"
#include "../include/apeal/console.hpp"


namespace apl {

static void tif_error_handler(const char* module, const char* fmt, va_list ap)
{
    constexpr int bufferSize = 1024;
    char message[bufferSize+1];
    std::vsnprintf(message, bufferSize, fmt, ap);
    //throwError("%s:%s:%s", currentImageFile_, module, message);
    throwError("%s:%s", module, message);
}

static void tif_warning_handler(const char* module, const char* fmt, va_list ap)
{
    constexpr int bufferSize = 1024;
    char message[bufferSize+1];
    std::vsnprintf(message, bufferSize, fmt, ap);

    auto warningTag = "tiff.warning";

    //if (currentImageInfo_) {

    //    auto& warn = currentImageInfo_->dict[warningTag];
    //    if (!warn.empty())
    //        warn += '\n';
    //    warn += message;
    //} else {
        println("%s:%s:%s", warningTag, module, message);
    //}
}



namespace impl {

struct LibTiffApiImpl {
    TIFF *ptif{};


    TIFFErrorHandler prevErrorHandler{}, prevWarningHandler{};

    void openRead(StrView filename) {
        ApiString apiFileName{filename};

#if APEAL_OS_WINDOWS
        ptif = TIFFOpenW(apiFileName.win(), "r");
#else
        ptif = TIFFOpen(apiFileName.utf8(), "r");
#endif
        verify(ptif, "Cannot open: %s", filename);


        prevErrorHandler = TIFFSetErrorHandler(tif_error_handler);
        prevWarningHandler = TIFFSetWarningHandler(tif_warning_handler);
    }

    ~LibTiffApiImpl() noexcept { release(); }

    void release() noexcept {
        if (ptif) {
            TIFFClose(ptif);
            ptif = nullptr;


            TIFFSetErrorHandler(prevErrorHandler);
            TIFFSetWarningHandler(prevWarningHandler);
        }

    }

    [[nodiscard]] auto reqField32(uint32_t tag) const {
        uint32_t val{};
        auto res = TIFFGetField(ptif, tag, &val);
        verify(res, "TIFF field is missing: %d", tag);
        return val;
    };

    [[nodiscard]] auto reqField16Pair(uint32_t tag, std::pair<uint16_t, uint16_t> def = {}) const {
        uint16_t val[2];
        auto res = TIFFGetField(ptif, tag, &val[0], &val[1]);
        if (def.first == 0 && def.second == 0)
            verify(res, "TIFF field is missing: %d", tag);
        if (!res)
            return def;
        return std::pair{val[0], val[1]};
    };

    [[nodiscard]] auto reqField16(uint32_t tag) const {
        uint16_t val{};
        auto res = TIFFGetField(ptif, tag, &val);
        verify(res, "TIFF field is missing: %d", tag);
        return val;
    };

    [[nodiscard]] auto reqFieldArray64(uint32_t tag) const {
        uint64_t *val{};
        auto res = TIFFGetField(ptif, tag, &val);
        verify(res, "TIFF field is missing: %d", tag);
        return val;
    };

    [[nodiscard]] auto reqFieldArray32(uint32_t tag) const {
        uint32_t *val{};
        auto res = TIFFGetField(ptif, tag, &val);
        verify(res, "TIFF field is missing: %d", tag);
        return val;
    };

    [[nodiscard]] auto getFieldString(uint32_t tag) const -> char* {
        char *val{};
        auto res = TIFFGetField(ptif, tag, &val);
        return res ? val : nullptr;
    };

};


struct LibTiffLoadContextImpl {

    LibTiffApiImpl tif;

    bool dirValid{};
    int dirIndex{};

    ByteBuffer tileBuffer;

    void release() noexcept {
        tif.release();
        dirValid = false;
    }
};

TiffLoadContext::TiffLoadContext() noexcept {
    pimpl = std::make_unique<LibTiffLoadContextImpl>();
}

TiffLoadContext::TiffLoadContext(TiffLoadContext&& other) noexcept {
    swap(other);
}

TiffLoadContext& TiffLoadContext::operator=(TiffLoadContext&& other) noexcept {
    swap(other);
    return *this;
}


TiffLoadContext::~TiffLoadContext() noexcept = default;


TiffDirectoryContext TiffLoadContext::readDir() {
    return LibTiff::loadDirectory(*this);
}

}  // namespace impl



// No multi threading inherently
//static ImageContainer *currentImageInfo_{};
//static StrView currentImageFile_{};


impl::TiffLoadContext  LibTiff::startLoadProcess(StrView filename, LoadParams parms)
{
    impl::TiffLoadContext loadContext;

    auto& ctx = *loadContext.pimpl.get();

    ctx.tif.openRead(filename);

    ctx.dirValid = true;
    ctx.dirIndex = 0;

    return std::move(loadContext);
}

impl::TiffDirectoryContext LibTiff::loadDirectory(impl::TiffLoadContext& loadContext)
{
    impl::TiffDirectoryContext dir{};

    dir.loadContext = std::addressof(loadContext);

    auto& ctx = *loadContext.pimpl;

    auto& tif = ctx.tif;
    auto& ptif = tif.ptif;

    if (!ctx.dirValid) {
        // Prepare next directory once we are done with the current
        auto readResult = TIFFReadDirectory(ptif);
        ctx.dirValid = readResult ? true : false;
        ctx.dirIndex++;
    }

    dir.valid = ctx.dirValid;

    if (!dir.valid)
        return dir;

    dir.index = ctx.dirIndex;

    auto& ii = dir.ii;

    auto tifWidth = tif.reqField32(TIFFTAG_IMAGEWIDTH);
    auto tifHeight = tif.reqField32(TIFFTAG_IMAGELENGTH);
    ii.size = Size<int>::make(tifWidth, tifHeight);


    auto tifBitsPerSample = tif.reqField16(TIFFTAG_BITSPERSAMPLE);
    ii.prec = ii.depth = tifBitsPerSample;


    auto tifPhotometric = tif.reqField16(TIFFTAG_PHOTOMETRIC);


    switch (tifPhotometric) {
    case PHOTOMETRIC_RGB:   
        ii.cm = ColorModel::RGB;
        ii.sub = {4, 4, 4};
        //ii.bpr = ii.dim.w * 3 * ii.depth / 8;
        break;

    case PHOTOMETRIC_YCBCR: {
        ii.cm = ColorModel::YCC;
        auto tifSubsampling = ctx.tif.reqField16Pair(TIFFTAG_YCBCRSUBSAMPLING, {uint16_t(2), uint16_t(2)});
        ii.sub.j = 4;
        ii.sub.a = tifSubsampling.first ? 2 : 4;
        ii.sub.b = tifSubsampling.second ? 0 : tifSubsampling.first ? 2 : 4;
        break;
    }

    default:
        throwError("Unsupported pixel format detected");
    }

    ii.bpr = ii.sub.bytesPerRow(ii.size.w, ii.depth);

    if (auto tifDesc = ctx.tif.getFieldString(TIFFTAG_IMAGEDESCRIPTION))
        dir.dict[ImageContainer::descriptionTag] = tifDesc;

    dir.isTiled = TIFFIsTiled(ptif);
    if (dir.isTiled) {
        dir.tilesCount = TIFFNumberOfTiles(ptif);
        dir.tileUncopressedSize = TIFFTileSize64(ptif);
        dir.tileBytesPerRow = TIFFTileRowSize64(ptif);

        auto tifTileWidth = (int)tif.reqField32(TIFFTAG_TILEWIDTH);
        auto tifTileHeight = (int)tif.reqField32(TIFFTAG_TILELENGTH);
        dir.tileSize = {tifTileWidth, tifTileHeight};
    } else {
        //int a = 0;
        //throwError("Striped TIFF are not supported yet");
    }

    // Schedule next directory load
    ctx.dirValid = false;
    //ctx.dirIndex++;

    return dir;
}

void LibTiff::finishLoadProcess(impl::TiffLoadContext& loadContext) {
    if (loadContext.pimpl)
        loadContext.pimpl->release();
}


void LibTiff::loadCore(ImageContainer* ic, Image* pimg, StrView filename, const LoadParams& params)
{
    //verify(ic || pimg, "Either image container or image must be supplied");
    if (!ic && !pimg)
        return throwError("Either image container or image must be supplied");

    if (ic)
        ic->fileFormat = ImageFileFormat::Tiff;


    auto opctx = startLoadProcess(filename, params);

    auto& ctx = *opctx.pimpl;

    bool imageLoaded{false};
    bool oneOnly = pimg || params.oneOnly;


    for (auto dir = opctx.readDir(); dir.valid; dir = opctx.readDir()) {

        if (imageLoaded && oneOnly)
            break;

        if (params.oneOnly && dir.index != params.index)
            continue;

        auto imgp = pimg ? pimg : std::addressof(ic->images.emplace_back());

        auto& img = *imgp;
        //auto& img = ic ? ic->images.emplace_back() : *pimg;

        img.init(dir.ii);
        img.dict = dir.dict;

        imageLoaded = true;

        // If we just want the information then load it and continue
        if (params.infoOnly)
            continue;

        img.alloc();
        img.zero();

        auto& tif = ctx.tif;
        auto& ptif = tif.ptif;

        if (dir.isTiled) {

// 
#if 0 // Load RGBA
            auto rgb = std::make_unique<uint32_t[]>(tifTileWidth * tifTileHeight);
            cbi.rgbaData = rgb.get();

            for (cbi.row = 0; cbi.row < cbi.ii.size.h; cbi.row += tifTileHeight) {
                for (cbi.col = 0; cbi.col < cbi.ii.size.w; cbi.col += tifTileWidth) {
                    TIFFReadRGBATile(tif, (uint32)cbi.col, (uint32)(cbi.row), rgb.get());
                }
            }
#endif

#if 1  // Load and decode the data as it is
            ctx.tileBuffer.alloc(dir.tileUncopressedSize);

            for (Point<int> pos{}; pos.y < img.size.h; pos.y += dir.tileSize.h) {
                for (pos.x = 0; pos.x < img.size.w; pos.x += dir.tileSize.w) {
                    decodeTile(dir, pos, img, pos);
                }
            }
#endif




        } else {
#if 1 // Load the whole image
            auto rgba = std::make_unique<uint32_t[]>(img.size.w * img.size.h);
            TIFFReadRGBAImage(ptif, (uint32_t)img.size.w, (uint32_t)img.size.h, rgba.get());

            // TODO: data conversion

#endif
        }
    }


    finishLoadProcess(opctx);
}

impl::TiffTilesInfo LibTiff::getTilesInfo(impl::TiffDirectoryContext& dir) {
    impl::TiffTilesInfo ti{};

    auto& ctx = *dir.loadContext->pimpl;
    auto& tif = ctx.tif;
    auto& ptif = tif.ptif;

    ti.tilesCount = TIFFNumberOfTiles(ptif);


    //struct JpegTables {
    //    uint32 length;
    //    void tables;
    //};

    //JpegTables jtables;

    //uint8_t jtables[2 + 8];
    uint32_t jpegTableLength{};
    uint8_t *jpegTables{};

    TIFFGetField(ptif, TIFFTAG_JPEGTABLES, &jpegTableLength, &jpegTables);

    ti.jpegTableLength = jpegTableLength;
    ti.jpegTable.alloc(jpegTableLength);
    std::memcpy(ti.jpegTable.data(), jpegTables, jpegTableLength);


    //auto jpegTables = tif.reqFieldArray32(TIFFTAG_JPEGTABLES);

    auto counts = tif.reqFieldArray64(TIFFTAG_TILEBYTECOUNTS);

    ti.tilesSizes.resize(ti.tilesCount);
    auto sit = ti.tilesSizes.begin();

    for (Index i = 0; i < ti.tilesCount; ++i) {
        *sit++ = counts[i];
    }

    return ti;
}

Index LibTiff::readRawTile(impl::TiffDirectoryContext& dir, const impl::TiffTilesInfo& ti, Point<int> tilePos, ByteBuffer& buffer)
{
    auto& ctx = *dir.loadContext->pimpl;
    auto& tif = ctx.tif;
    auto& ptif = tif.ptif;

    auto tifX = (uint32_t)tilePos.x;
    auto tifY = (uint32_t)tilePos.y;

    auto tileIndex = TIFFComputeTile(ptif, tifX, tifY, 0, static_cast<uint16_t>(-1));

    verify(tileIndex < ti.tilesCount, "Tile index out of range");

    auto tileSize = ti.tilesSizes[tileIndex];

    if (tileSize <= 0)
        return tileSize;

    buffer.alloc(tileSize);

    auto bytes = TIFFReadRawTile(ptif, tileIndex, buffer.data(), tileSize);

    return bytes;
}

bool LibTiff::decodeTile(impl::TiffDirectoryContext& dir, Point<int> tilePos, Image& img, Point<int> imgPos)
{
    auto& ctx = *dir.loadContext->pimpl.get();
    auto& tif = ctx.tif;
    auto& ptif = tif.ptif;

    ctx.tileBuffer.alloc(dir.tileUncopressedSize);
    auto bufferData = ctx.tileBuffer.data();

    auto tifX = (uint32_t)tilePos.x;
    auto tifY = (uint32_t)tilePos.y;

    //auto tilesPerRow = (dir.ii.dim.w + dir.tileDim.w - 1) / dir.tileDim.w;

    //auto tileCol = tilePos.x / dir.tileDim.w;
    //auto tileRow = tilePos.y / dir.tileDim.h;

    //auto tileIndex = tileRow * tilesPerRow + tileCol;

    auto tileIndex = TIFFComputeTile(ptif, tifX, tifY, 0, (uint16_t)-1);

    auto bytesCount = TIFFGetStrileByteCount(ptif, tileIndex);

    if (bytesCount == 0) {
        //img.buffer.zero();
        return false;
    }

    auto res = TIFFReadEncodedTile(ptif, tileIndex, bufferData, (tmsize_t)-1);

    //auto res = TIFFReadTile(ptif, bufferData, tifX, tifY, 0, (uint16)-1);

    if (res == (tmsize_t)-1) {
        //img.buffer.zero();
        return false;
    }

    auto srow = bufferData;

    auto dbpr = img.bpr();
    auto dy = img.sub.dy();

    auto imgTileRow = imgPos.y/dy;
    auto remainingRows = img.size.h - imgTileRow;

    auto colBytes = img.sub.bytesPerRow(imgPos.x, img.depth);
    auto drow = img.data() + imgTileRow * dbpr + colBytes;

    auto remainingColBytes = img.sub.bytesPerRow(img.size.w - imgPos.x, img.depth);
    auto copyBytes = std::min(dir.tileBytesPerRow, remainingColBytes);

    auto copyRows = img.sub.rowsPerHeight(std::min(dir.tileSize.h, remainingRows));
    auto sadvance = dir.tileBytesPerRow;

    if (img.isRGB() || img.isRGBA()) {
        // flip the rows
        sadvance = -sadvance;
        srow = srow + dir.tileBytesPerRow * (dir.tileSize.h - 1);
    }

    for (Index j{}; j < copyRows; ++j) {
        std::memcpy(drow, srow, copyBytes);
        drow += dbpr;
        srow += sadvance;
    }

    return true;
}

}  // namespace apl


#endif
