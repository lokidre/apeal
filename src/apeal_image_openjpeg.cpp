/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/image/openjpeg.hpp"

#if HAVE_OPENJPEG

#include "../include/apeal/file.hpp"
#include "../include/apeal/on_exit.hpp"

#include <openjpeg.h>

namespace apl {

struct OpjStreamContext {
    File file;
};

static OPJ_SIZE_T opj_stream_read_fn(void* buffer, OPJ_SIZE_T bytes, void* user)
{
    auto ctx = reinterpret_cast<OpjStreamContext*>(user);
    return (OPJ_SIZE_T)ctx->file.read(buffer, bytes);
}

static OPJ_SIZE_T opj_stream_write_fn(void* buffer, OPJ_SIZE_T bytes, void* user)
{
    auto ctx = reinterpret_cast<OpjStreamContext*>(user);
    return (OPJ_SIZE_T)ctx->file.write(buffer, bytes);
}


// Callback function prototype for skip function
static OPJ_OFF_T opj_stream_skip_fn(OPJ_OFF_T bytes, void* user)
{
    //ADebugPrintFunc("%d", bytes);

    auto ctx = reinterpret_cast<OpjStreamContext*>(user);
    ctx->file.seek(bytes, FileOrigin::Current);
    return bytes;
}

// Callback function prototype for seek function
static OPJ_BOOL opj_stream_seek_fn(OPJ_OFF_T bytes, void* user)
{
    //ADebugPrintFunc("%d", bytes);

    auto ctx = reinterpret_cast<OpjStreamContext*>(user);
    ctx->file.seek(bytes, FileOrigin::Begin);
    return OPJ_TRUE;
}

// Callback function prototype for free user data function
static void opj_stream_free_user_data_fn(void* user)
{
}


static void opj_error_callback(const char* msg, void*) {
    ADebugPrintFunc("%s", msg);
    throwError("%s", msg);
}
static void opj_warning_callback(const char* msg, void*) {
    ADebugPrintFunc("%s", msg);
}
static void opj_info_callback(const char* msg, void*) {
    //ADebugPrintFunc("%s", msg);
}


namespace impl {

struct OpenJpegOpContextImpl {

    opj_codec_t* codec{};
    opj_stream_t* stream{};
    opj_image_t* opjImage{};

    ByteBuffer opjParams;
    ByteBuffer opjComps;  // components

    bool opjCompressStarted{};

    OpjStreamContext sctx;

    bool opjImageDataAllocated{};

    OPJ_COLOR_SPACE opjColorSpace{};
    OPJ_UINT32 opjPrec{};
    OPJ_UINT32 opjNumComps{};

    bool tilesEnabled{};
    Size<int> tileSize{};
    ByteBuffer tileBuffer;

    bool needConvert{};
    ColorModel dfmt{};

    OpenJpegOpContextImpl() noexcept = default;

    OpenJpegOpContextImpl(const OpenJpegOpContextImpl& other) = delete;
    OpenJpegOpContextImpl& operator=(const OpenJpegOpContextImpl& other) = delete;

    ~OpenJpegOpContextImpl() noexcept { release();  }

    void release() noexcept {
        if (stream) {
            opj_stream_destroy(stream);
            stream = nullptr;
        }

        if (opjImage) {
            if (!opjImageDataAllocated)
                for (OPJ_UINT32 i{}; i < opjImage->numcomps; ++i)
                    opjImage->comps[i].data = 0;
            opj_image_destroy(opjImage);
            opjImage = nullptr;
        }

        if (codec) {
            opj_destroy_codec(codec);
            codec = nullptr;
        }
    }

    static void assertOpjObj (const void* res, const char* message=nullptr) {
        if (!res)
            throwError(message ? message : "opj error");
    };

    static void assertOpjRes(OPJ_BOOL res, const char* message=nullptr) {
        if (!res)
            throwError(message ? message : "opj error");
    };


};

OpContext::OpContext() noexcept {
    pimpl = std::make_unique<OpenJpegOpContextImpl>();
}

OpContext::OpContext(OpContext&& other) noexcept {
    swap(other);
}

OpContext& OpContext::operator=(OpContext&& other) noexcept {
    swap(other);
    return *this;
}


OpContext::~OpContext() noexcept {}


} // namespace impl

using namespace impl;


OpContext OpenJpeg::startLoadProcess(StrView filename, LoadParams parms)
{
    OpContext octx;

    auto& ctx = *octx.pimpl.get();

    ctx.opjParams.alloc(sizeof(opj_dparameters_t));
    ctx.opjParams.zero();

    auto opjParams = reinterpret_cast<opj_dparameters_t*>(ctx.opjParams.data());

    opjParams->decod_format = -1;
    opjParams->cod_format = -1;

    opj_set_default_decoder_parameters(opjParams);


    ctx.sctx.file.open(filename, FileAccess::Read);

    // TODO: figure out why reading isn't working with the file class
    ApiString infile{filename};
    ctx.stream = opj_stream_create_default_file_stream(infile.utf8(), OPJ_TRUE);

    //ctx.stream = opj_stream_default_create(OPJ_TRUE);
    //verify(ctx.stream, "Error: opj_stream_default_create");

    auto& stream = ctx.stream;

    //auto userData = std::addressof(ctx.sctx);
    //opj_stream_set_read_function(stream, opj_stream_read_fn);
    //opj_stream_set_skip_function(stream, opj_stream_skip_fn);
    //opj_stream_set_seek_function(stream, opj_stream_seek_fn);
    //opj_stream_set_user_data(stream, userData, opj_stream_free_user_data_fn);
    //opj_stream_set_user_data_length(stream, sizeof(userData));


    OPJ_CODEC_FORMAT opjCodecFormat{};

    switch (parms.ffmt) {
    case ImageFileFormat::J2k:  
        opjCodecFormat = OPJ_CODEC_J2K; 
        opjParams->decod_format = 0;
        break;
    case ImageFileFormat::Jp2:  
        opjCodecFormat = OPJ_CODEC_JP2;
        opjParams->decod_format = 1;
        break; 
    default: 
        opjCodecFormat = OPJ_CODEC_JP2; 
        opjParams->decod_format = 1;
        break;
    }


    ctx.codec = opj_create_decompress(opjCodecFormat);
    verify(ctx.codec, "Error: opj_create_decompress");

    auto& codec = ctx.codec;

    opj_set_error_handler(codec, opj_error_callback, 0);
    opj_set_warning_handler(codec, opj_warning_callback, 0);
    opj_set_info_handler(codec, opj_info_callback, 0);


    auto res = opj_setup_decoder(codec, opjParams);
    ctx.assertOpjRes(res, "Failed to decode image: opj_setup_decoder");

    res = opj_read_header(stream, codec, &ctx.opjImage);

    ctx.opjNumComps = ctx.opjImage->numcomps;

    verify(ctx.opjNumComps, "Image has no components");


    auto& ii = octx.ii;
    ii.size = Size<int>::make(ctx.opjImage->x1 - ctx.opjImage->x0, ctx.opjImage->y1 - ctx.opjImage->y0);


    // Get all the info from the first component
    // differnt precision for differnt components isn't supported anyway
    auto comp = ctx.opjImage->comps;
    ii.prec = comp->prec;
    ii.depth = comp->bpp;
    if (ii.depth == 0)
        ii.depth = ii.prec;

    switch (comp->prec) {
    case 0:
    case 8:
        ii.val = comp->sgnd ? ImageValueType::S8 : ImageValueType::U8;
        break;
    case 16:
        ii.val = comp->sgnd ? ImageValueType::S16 : ImageValueType::U16;
        break;
    case 32:
        ii.val = comp->sgnd ? ImageValueType::S32 : ImageValueType::U32;
        break;
    default: throwError("Unknown or unsupported precision");
    }


    if (ctx.opjImage->numcomps == 1) {
        ii.cm = ColorModel::Gray;
        ii.sub = {4, 4, 4};
    } else if (ctx.opjImage->numcomps == 3) {
        // could be RGB or YCC
        if (comp[0].dx == comp[1].dx)
            ii.sub = {4, 4, 4};
        else
            ii.sub = {4, 2, 0};
    }



    //ctx.tif.openRead(filename);

    //ctx.dirValid = true;
    //ctx.dirIndex = 0;

    return std::move(octx);
}


void OpenJpeg::decodeImage(OpContext& opctx, Image& out)
{
    auto& ctx = *opctx.pimpl.get();

    auto res = opj_decode(ctx.codec, ctx.stream, ctx.opjImage);
    ctx.assertOpjRes(res, "opj_decode failed");

    auto& ii = opctx.ii;

    switch (ctx.opjImage->color_space) {
    case OPJ_CLRSPC_SYCC:
        ii.cm = ColorModel::YCC;
        ii.sub = {4, 2, 0};
        break;

    case OPJ_CLRSPC_GRAY:
        ii.cm = ColorModel::Gray;
        ii.sub = {4, 4, 4};
        break;

    case OPJ_CLRSPC_SRGB:
        if (ctx.opjImage->numcomps == 4) {
            ii.cm = ColorModel::RGBA;
            ii.sub = {4, 4, 4, 4};
        } else {
            ii.cm = ColorModel::RGB;
            ii.sub = {4, 4, 4};
        }
        break;
    default:
        ADebugTrap();
        break;
    }

    out.create(ii);

    ii.bpr = out.bpr();


    // convert the data
    Image::ConvertContext cvt{};

    cvt.size = ii.size;

    cvt.spc = ctx.opjNumComps;
    for (Index i{}; i < ctx.opjNumComps; ++i) {
        auto& opj_comp = ctx.opjImage->comps[i];
        auto& src = cvt.sp[i];

        src.data = (const uint8_t*)opj_comp.data;
        src.cm = ii.cm;
        src.val = ImageValueType::U32;

        if (ii.cm == ColorModel::YCC) {
            if (i == 1 || i == 2)
                src.bpr = ii.size.w * ii.sub.a / 4;
            else
                src.bpr = ii.size.w;
        } else {
            src.bpr = ii.size.w;
        }

        src.bpr *= sizeof(OPJ_INT32);
    }

    
    cvt.dpc = 1;
    auto& dst = cvt.dp[0];
    dst.data = out.data();
    dst.cm = ii.cm;
    dst.bpr = ii.bpr;
    dst.val = ii.val;



    Image::convertCore(cvt);
}




auto OpenJpeg::startSaveProcess(StrView filename, const Image& iimage, SaveParams parms) -> OpContext 
{
    OpContext opctx;

    auto& ctx = *opctx.pimpl.get();

    ctx.opjParams.alloc(sizeof(opj_cparameters_t));
    ctx.opjParams.zero();

    auto opjParams = (opj_cparameters_t *)ctx.opjParams.data();
    opj_set_default_encoder_parameters(opjParams);


    opjParams->subsampling_dx = opjParams->subsampling_dy = 1;

    auto& in = iimage;

    ctx.opjPrec = (OPJ_UINT32)in.prec;
    ctx.opjNumComps = (OPJ_UINT32)in.numComps;


    switch (in.cm) {
    case ColorModel::RGB:
        ctx.opjColorSpace = OPJ_CLRSPC_SRGB;
        ctx.dfmt = ColorModel::RGB;
        break;

    case ColorModel::RGBA:
        ctx.opjColorSpace = OPJ_CLRSPC_SRGB;
        ctx.dfmt = ColorModel::RGBA;
        break;

    case ColorModel::Gray:
        ctx.opjColorSpace = OPJ_CLRSPC_GRAY;
        ctx.dfmt = ColorModel::Gray;
        break;

    case ColorModel::YCC:
        ctx.opjColorSpace = OPJ_CLRSPC_SYCC;
        ctx.dfmt = ColorModel::YCC;
        break;

    default:
        ADebugTrap();
        break;
    }


    switch (parms.ffmt) {
    case ImageFileFormat::J2k:  opjParams->cod_format = OPJ_CODEC_J2K ; break;
    case ImageFileFormat::Jp2:  opjParams->cod_format = OPJ_CODEC_JP2; break;
    //case ImageFileFormat::Jpt:  cparams->cod_format = 2; break;
    default: opjParams->cod_format = 1; break;
    }

    ctx.codec = opj_create_compress(OPJ_CODEC_JP2);
    verify(ctx.codec, "Error: opj_create_compress");

    auto& codec = ctx.codec;

    opj_set_error_handler(codec, opj_error_callback, 0);
    opj_set_warning_handler(codec, opj_warning_callback, 0);
    opj_set_info_handler(codec, opj_info_callback, 0);


    std::memcpy(opjParams->outfile, filename.data(), filename.size() * sizeof(StrView::value_type));
    opjParams->outfile[filename.size()] = 0;


    if (parms.lossy) {

        opjParams->tcp_numlayers = parms.jp2NumLayers;
        for (auto i = 0; i < parms.jp2NumLayers; ++i) {
            opjParams->tcp_distoratio[i] = parms.jp2Psnr[i];
        }

        opjParams->cp_fixed_quality = 1;

    } else {

        // Lossless
        if (opjParams->tcp_numlayers == 0) {
            opjParams->tcp_rates[0] = 0;
            opjParams->tcp_numlayers++;
            opjParams->cp_disto_alloc = 1;
        }
    }

    ctx.tilesEnabled = !parms.flat;

    if (parms.flat) {
        opjParams->tile_size_on = OPJ_FALSE;
    } else {
        //if (!parms.tileDim)
        //    parms.tileSize = {512, 512};

        ctx.tileSize = parms.tileSize;
        if (!ctx.tileSize)
            ctx.tileSize = {512, 512};


        opjParams->tile_size_on = OPJ_TRUE;
        opjParams->cp_tdx = (int)ctx.tileSize.w;
        opjParams->cp_tdy = (int)ctx.tileSize.h;
    }

    ctx.opjComps.alloc(sizeof(opj_image_cmptparm_t) * ctx.opjNumComps);
    ctx.opjComps.zero();

    //auto comp_params = std::make_unique<opj_image_cmptparm_t[]>(opjNumComps);
    //std::memset(comp_params.get(), 0, sizeof(*comp_params.get()) * opjNumComps);

    auto opjWidth = (OPJ_UINT32)in.size.w;
    auto opjHeight = (OPJ_UINT32)in.size.h;

    //auto cmptparm = new opj_image_cmptparm_t[numcomps] ;
    auto compParams = (opj_image_cmptparm_t*)ctx.opjComps.data(); //comp_params.get();
    auto comp = compParams;
    for (Index i = 0; i < ctx.opjNumComps; ++i, ++comp) {

        comp->prec = ctx.opjPrec;

        //cparm->bpp = outFrame.bitDepth ;
        comp->dx = opjParams->subsampling_dx;
        comp->dy = opjParams->subsampling_dy;

        //comp->dx = 1;
        //comp->dy = 1;

        comp->w = opjWidth;
        comp->h = opjHeight;

        if (in.isYCC()) {
            if (i == 1 || i == 2) {
                comp->dx = opjParams->subsampling_dx * 2;
                comp->dy = opjParams->subsampling_dy * 2;// / in.sub.a;
            }
        }

    }

    // Check if we need to convert the image data
    ctx.needConvert = true;

    //if (!in.isYCC())
    //    needConvert = true;


    // Create target image with or without buffer
    if (ctx.needConvert) {
        ctx.opjImage = opj_image_create(ctx.opjNumComps, compParams, ctx.opjColorSpace);
        ctx.opjImageDataAllocated = true;
    } else {
        // If the conversion isn't needed then just create the structure
        ctx.opjImage = opj_image_tile_create(ctx.opjNumComps, compParams, ctx.opjColorSpace);
    }
    ctx.assertOpjObj(ctx.opjImage);

    auto& image = ctx.opjImage;

    image->x0 = 0;
    image->y0 = 0;
    image->x1 = image->x0 + opjWidth * opjParams->subsampling_dx;
    image->y1 = image->y0 + opjHeight * opjParams->subsampling_dy;


    auto res = opj_setup_encoder(codec, opjParams, image);
    ctx.assertOpjRes(res, "Failed to encode image: opj_setup_encoder");


    ctx.sctx.file.create(filename, FileCreate::CreateAlways, FileAccess::Write);

    //ApiString apiFile{filename};
    //stream = opj_stream_create_default_file_stream(apiFile.utf8(), OPJ_FALSE);

    ctx.stream = opj_stream_default_create(OPJ_FALSE);
    verify(ctx.stream, "Error: opj_stream_default_create");

    auto& stream = ctx.stream;

    opj_stream_set_write_function(stream, opj_stream_write_fn);
    opj_stream_set_skip_function(stream, opj_stream_skip_fn);
    opj_stream_set_seek_function(stream, opj_stream_seek_fn);

    auto userData = std::addressof(ctx.sctx);
    opj_stream_set_user_data(stream, userData, opj_stream_free_user_data_fn);
    opj_stream_set_user_data_length(stream, sizeof(userData));

    //res = opj_start_compress(ctx.codec, ctx.opjImage, ctx.stream);
    //ctx.assertOpjRes(res, "opj_start_compress");

    opctx.ii.size = iimage.size;
    opctx.ii.cm = iimage.cm;
    opctx.ii.bpr = iimage.bpr();
    opctx.ii.depth = iimage.depth;
    opctx.ii.prec = iimage.prec;
    opctx.ii.sub = iimage.sub;
    opctx.ii.val = iimage.val;

    return std::move(opctx);
}


void OpenJpeg::encodeTile(OpContext& opctx, Point<int> tilePos, ImageA img, Point<int> imgPos)
{
    auto& ctx = *opctx.pimpl.get();
    OPJ_BOOL res{};

    //auto opjParams = (opj_cparameters_t*)ctx.opjParams.data();

    auto tilesPerRow = (opctx.ii.size.w + ctx.tileSize.w - 1) / ctx.tileSize.w;

    auto tileCol = tilePos.x / ctx.tileSize.w;
    auto tileRow = tilePos.y / ctx.tileSize.h;

    auto tileIndex = tileRow * tilesPerRow + tileCol;

    verify(img.isYCC(), "Only YCC is supported for now");

    auto tileDataSize = img.bpr() * ctx.tileSize.h/2;
    ctx.tileBuffer.alloc(tileDataSize);

    auto opjIndex = (OPJ_UINT32)tileIndex;
    //auto opjData = (OPJ_BYTE*)img.data();
    auto opjData = (OPJ_BYTE*)ctx.tileBuffer.data();
    auto opjDataSize = (OPJ_UINT32)tileDataSize;

    ctx.tileBuffer.alloc(tileDataSize);

    // Convert the data
    Image::ConvertContext cvt{};

    cvt.size = ctx.tileSize;

    auto& sp = cvt.sp[0];
    sp.cm = img.cm;
    sp.val = img.val;
    sp.data = img.buffer.data();
    sp.bpr = img.bpr();


    cvt.dpc = ctx.opjNumComps;

    auto dptr = ctx.tileBuffer.data();
    for (Index i{}; i < ctx.opjNumComps; ++i) {
        auto& dp = cvt.dp[i];

        dp.data = dptr;
        dp.cm = ctx.dfmt;
        dp.val = ImageValueType::U8;

        Index dh = cvt.size.h;
        if (img.isYCC()) {
            if (i == 1 || i == 2) {
                dp.bpr = cvt.size.w * img.sub.a / 4;
                dh = cvt.size.h * img.sub.a / 4;
            } else {
                dp.bpr = cvt.size.w;
            }
        } else {
            dp.bpr = cvt.size.w;
        }

        dptr += dp.bpr * dh;

        //dp.bpr *= sizeof(OPJ_INT32);
    }

    Image::convertCore(cvt);


    if (!ctx.opjCompressStarted) {
        res = opj_start_compress(ctx.codec, ctx.opjImage, ctx.stream);
        ctx.assertOpjRes(res, "opj_start_compress");

        ctx.opjCompressStarted = true;
    }

    res = opj_write_tile(ctx.codec, opjIndex, opjData, opjDataSize, ctx.stream);
    ctx.assertOpjRes(res, "opj_write_tile");
}


void OpenJpeg::encodeImage(OpContext& opctx, const Image& in)
{
    auto& ctx = *opctx.pimpl.get();
    OPJ_BOOL res{};

    // Convert the data
    if (ctx.needConvert) {
        Image::ConvertContext cvt{};

        cvt.size = in.size;
        auto& sp = cvt.sp[0];
        sp.cm = in.cm;
        sp.val = in.val;
        sp.data = in.buffer.data();
        sp.bpr = in.bpr();


        cvt.dpc = ctx.opjNumComps;

        for (Index i{}; i < ctx.opjNumComps; ++i) {
            auto& opj_comp = ctx.opjImage->comps[i];
            auto& dp = cvt.dp[i];

            dp.data = (uint8_t*)opj_comp.data;
            dp.cm = ctx.dfmt;
            dp.val = ImageValueType::U32;
            //dst.bpr = sizeof(OPJ_INT32) * in.dim.w;

            if (in.isYCC()) {
                if (i == 1 || i == 2)
                    dp.bpr = in.size.w * in.sub.a / 4;
                else
                    dp.bpr = in.size.w;
            } else {
                dp.bpr = in.size.w;
            }

            dp.bpr *= sizeof(OPJ_INT32);
        }

        Image::convertCore(cvt);
    } else {
        for (Index i{}; i < ctx.opjNumComps; ++i) {
            auto& opjComp = ctx.opjImage->comps[i];
            opjComp.data = (OPJ_INT32 *)in.planes[i].data();
        }
    }


    res = opj_start_compress(ctx.codec, ctx.opjImage, ctx.stream);
    ctx.assertOpjRes(res, "opj_start_compress");

    res = opj_encode(ctx.codec, ctx.stream);
    ctx.assertOpjRes(res, "opj_encode");
}




void OpenJpeg::finishLoadProcess(OpContext& opCtx)
{
    auto& ctx = *opCtx.pimpl.get();

    auto res = opj_end_decompress(ctx.codec, ctx.stream);
    ctx.assertOpjRes(res);

    ctx.release();
}

void OpenJpeg::finishSaveProcess(OpContext& opCtx)
{
    auto& ctx = *opCtx.pimpl.get();

    auto res = opj_end_compress(ctx.codec, ctx.stream);
    ctx.assertOpjRes(res);

    ctx.opjCompressStarted = false;

    ctx.release();
}




void OpenJpeg::loadCore(StrView filename, JpLoadContext& loadContext, ImageContainer* ic)
{
    auto ctx = startLoadProcess(filename, loadContext.parms);
    auto& image = ic->images.emplace_back();
    decodeImage(ctx, image);
    finishLoadProcess(ctx);
}

//template <int S>
//struct Msg;
//Msg<sizeof(opj_cparameters_t)> aa;


void OpenJpeg::saveCore(StrView filename, JpSaveContext& saveContext)
{
    auto ctx = startSaveProcess(filename, *saveContext.image, saveContext.parms);
    encodeImage(ctx, *saveContext.image);
    finishSaveProcess(ctx);

#if 0
    auto& in = *ctx.image;

    opj_codec_t* codec{};
    opj_stream_t* stream{};
    opj_image_t* image{};
    OPJ_BOOL res;

    OpjStreamContext sctx;
    bool opjImageDataAllocated{};

    auto& parms = ctx.parms;

    OnExit onExit([&]{
        if (stream) {
            opj_stream_destroy(stream);
            stream = nullptr;
        }

        if (image) {
            if (!opjImageDataAllocated)
                for (OPJ_UINT32 i{}; i < image->numcomps; ++i)
                    image->comps[i].data = 0;
            opj_image_destroy(image);
            image = nullptr;
        }

        if (codec) {
            opj_destroy_codec(codec);
            codec = nullptr;
        }
    });

    auto assert_opj_object = [](const void* res, const char* message=nullptr) {
        if (!res)
            throw std::runtime_error(message ? message : "opj error");
    };

    auto assert_opj = [](OPJ_BOOL res, const char* message=0) {
        if (!res)
            throw std::runtime_error(message ? message : "opj error");
    };


    OPJ_COLOR_SPACE opjColorSpace{}; //_color_space = OPJ_CLRSPC_SRGB;
    OPJ_UINT32 opjPrec = static_cast<OPJ_UINT32>(in.prec);
    OPJ_UINT32 opjNumComps = static_cast<OPJ_UINT32>(in.numComps);
    //OPJ_UINT32 opjNumComps = static_cast<OPJ_UINT32>(in.numPlanes);
    ColorModel dfmt{};

    auto opjParams_sp = std::make_unique<opj_cparameters_t>();

    auto opjParams = opjParams_sp.get();
    std::memset(opjParams, 0, sizeof(*opjParams));
    opj_set_default_encoder_parameters(opjParams);


    opjParams->subsampling_dx = opjParams->subsampling_dy = 1;

    switch (in.cm) {
    case ColorModel::RGB:
        opjColorSpace = OPJ_CLRSPC_SRGB;
        dfmt = ColorModel::RGB;
        break;

    case ColorModel::RGBA:
        opjColorSpace = OPJ_CLRSPC_SRGB;
        dfmt = ColorModel::RGBA;
        break;

    case ColorModel::Gray:
        opjColorSpace = OPJ_CLRSPC_GRAY;
        dfmt = ColorModel::Gray;
        break;

    case ColorModel::YCC:
        opjColorSpace = OPJ_CLRSPC_SYCC;
        dfmt = ColorModel::YCC;
        break;

    default:
        ADebugTrap();
        break;
    }


    switch (ctx.parms.ffmt) {
    case ImageFileFormat::J2k:  opjParams->cod_format = OPJ_CODEC_J2K ; break;
    case ImageFileFormat::Jp2:  opjParams->cod_format = OPJ_CODEC_JP2; break;
    //case ImageFileFormat::Jpt:  cparams->cod_format = 2; break;
    default: opjParams->cod_format = 1; break;
    }

    std::memcpy(opjParams->outfile, filename.data(), filename.size() * sizeof(StrView::value_type));
    opjParams->outfile[filename.size()] = 0;

    if (parms.lossy) {

        // Lossy
        opjParams->tcp_numlayers = parms.jp2NumLayers;//3;
        for (auto i = 0; i < parms.jp2NumLayers; ++i) {
            opjParams->tcp_distoratio[i] = parms.jp2Psnr[i];
        }

        //cparams->tcp_distoratio[0] = 30;
        //cparams->tcp_distoratio[1] = 40;
        //cparams->tcp_distoratio[2] = 50;
        opjParams->cp_fixed_quality = 1;

    } else {

        // Lossless
        if (opjParams->tcp_numlayers == 0) {
            opjParams->tcp_rates[0] = 0;
            opjParams->tcp_numlayers++;
            opjParams->cp_disto_alloc = 1;
        }
    }

    if (!parms.tileSize) {
        parms.tileSize = {512, 512};
    }

    if (parms.tileSize.w && parms.tileSize.h) {
        opjParams->tile_size_on = OPJ_TRUE;
        opjParams->cp_tdx = parms.tileSize.w;
        opjParams->cp_tdy = parms.tileSize.h;
    } else {
        opjParams->tile_size_on = OPJ_FALSE;
    }


    auto comp_params = std::make_unique<opj_image_cmptparm_t[]>(opjNumComps);
    std::memset(comp_params.get(), 0, sizeof(*comp_params.get()) * opjNumComps);

    auto opjWidth = (OPJ_UINT32)in.dim.w;
    auto opjHeight = (OPJ_UINT32)in.dim.h;

    //auto cmptparm = new opj_image_cmptparm_t[numcomps] ;
    auto comp = comp_params.get();
    for (Index i = 0; i < opjNumComps; ++i, ++comp) {

        comp->prec = opjPrec;

        //cparm->bpp = outFrame.bitDepth ;
        comp->dx = opjParams->subsampling_dx;
        comp->dy = opjParams->subsampling_dy;

        //comp->dx = 1;
        //comp->dy = 1;

        comp->w = opjWidth;
        comp->h = opjHeight;

        if (in.isYCC()) {
            if (i == 1 || i == 2) {
                comp->dx = opjParams->subsampling_dx * 2;
                comp->dy = opjParams->subsampling_dy * 2;// / in.sub.a;
            }
        }

    }

    // Check if we need to convert the image data
    bool needConvert{true};

    //if (!in.isYCC())
    //    needConvert = true;


    // Create target image with or without buffer
    if (needConvert) {
        image = opj_image_create(opjNumComps, comp_params.get(), opjColorSpace);
        opjImageDataAllocated = true;
    } else {
        // If the conversion isn't needed then just create the structure
        image = opj_image_tile_create(opjNumComps, comp_params.get(), opjColorSpace);
    }
    assert_opj_object(image);


    // Convert the data
    if (needConvert) {
        Image::ConvertContext cvt{};

        cvt.size = in.dim;
        auto& src = cvt.splanes[0];
        src.cm = in.cm;
        src.val = in.val;
        src.data = in.buffer.data();
        src.bpr = in.bytesPerRow;

        cvt.dplanec = opjNumComps;

        for (Index i{}; i < opjNumComps; ++i) {
            auto& opj_comp = image->comps[i];
            auto& dst = cvt.dplanes[i];

            dst.data = opj_comp.data;
            dst.cm = dfmt;
            dst.val = ImageValueType::U32;
            //dst.bpr = sizeof(OPJ_INT32) * in.dim.w;

            if (in.isYCC()) {
                if (i == 1 || i == 2)
                    dst.bpr = in.dim.w * in.sub.a / 4;
                else
                    dst.bpr = in.dim.w;
            } else {
                dst.bpr = in.dim.w;
            }

            dst.bpr *= sizeof(OPJ_INT32);
        }

        Image::cvt_prim(std::move(cvt));
    } else {
        for (Index i{}; i < opjNumComps; ++i) {
            auto& opj_comp = image->comps[i];
            opj_comp.data = (OPJ_INT32 *)in.planes[i].data();
        }
    }

    image->x0 = 0;
    image->y0 = 0;
    //image->x1 = image->x0 + (opjWidth - 1)*opjParams->subsampling_dx + 1;
    //image->y1 = image->y0 + (opjHeight - 1)*opjParams->subsampling_dy + 1;
    image->x1 = image->x0 + opjWidth * opjParams->subsampling_dx;
    image->y1 = image->y0 + opjHeight * opjParams->subsampling_dy;

    // Multiple component transform
    //cparams->tcp_mct = (image->numcomps >= 3) ? 1 : 0;

    codec = opj_create_compress(OPJ_CODEC_JP2);
    verify(codec, "Error: opj_create_compress");

    opj_set_warning_handler(codec, opj_warning_callback, 0);
    opj_set_error_handler(codec, opj_error_callback, 0);

    res = opj_setup_encoder(codec, opjParams, image);
    assert_opj(res, "Failed to encode image: opj_setup_encoder");


    sctx.file.create(filename, FileCreate::CreateAlways, FileAccess::Write);

    //ApiString apiFile{filename};
    //stream = opj_stream_create_default_file_stream(apiFile.utf8(), OPJ_FALSE);

    stream = opj_stream_default_create(OPJ_FALSE);
    verify(stream, "Error: opj_stream_default_create");

    opj_stream_set_write_function(stream, opj_stream_write_fn);
    opj_stream_set_skip_function(stream, opj_stream_skip_fn);
    opj_stream_set_seek_function(stream, opj_stream_seek_fn);

    auto userData = &sctx;
    opj_stream_set_user_data(stream, userData, opj_stream_free_user_data_fn);
    opj_stream_set_user_data_length(stream, sizeof(userData));

    assert_opj(opj_start_compress(codec, image, stream));


    assert_opj(opj_encode(codec, stream));
    assert_opj(opj_end_compress(codec, stream));
#endif

}

}  // namespace apl


#endif


