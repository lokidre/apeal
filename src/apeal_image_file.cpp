/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/image.hpp"
#include "../include/apeal/hash.hpp"

#include "../include/apeal/str/case.hpp"

#include "../include/apeal/file.hpp"


#include "../include/apeal/image/libjpeg.hpp"
#include "../include/apeal/image/libpng.hpp"
#include "../include/apeal/image/libtiff.hpp"
#include "../include/apeal/image/openjpeg.hpp"


namespace apl {


ImageFileFormat ImageFile::guessFileFormat(StrView loc)
{
    using Fmt = ImageFileFormat;

    auto [dir, name, ext] = parseFilePath(loc);

    auto extLo = str_lower(ext);

    using namespace literals;

    switch (pshHash(extLo)) {
    case "png"_psh:
        return Fmt::Png;
    
    case "jpg"_psh:
    case "jpe"_psh:
    case "jif"_psh:
    case "jfif"_psh:
    case "jfi"_psh:
    case "jpeg"_psh:    
        return Fmt::Jpeg;

    case "jpx"_psh:     // more comprehensive file format
        return Fmt::Jpx;
    case "jp2"_psh:     // container format
        return Fmt::Jp2;
    case "j2k"_psh:     // more like raw format
        return Fmt::J2k;
    case "jpf"_psh: 
    case "jpm"_psh: 
    case "mj2"_psh:
        return Fmt::Jp2;

    case "tiff"_psh:
    case "tif"_psh:     
        return Fmt::Tiff;

    case "bmp"_psh:
    case "dib"_psh:     
        return Fmt::Bmp;

    case "ico"_psh:
    case "cur"_psh:     
        return Fmt::Ico;

    case "icns"_psh:    
        return Fmt::Icns;
    default: break;
    }

    return Fmt::Unknown;
}

ImageFileFormat ImageFile::detectFileFormat(StrView loc) {
    ADebugTrap(); // throwError("Not implemented: %s", __FUNCTION__);
    return {};
}



void ImageFile::loadIcns(StrView filename, ImageContainer* ic, LoadParams params) {
    throwError("loadIcnd is not implemented: %s", filename);
}

void ImageFile::loadJpeg(StrView filename, ImageContainer* ic, LoadParams lp) {
#if HAVE_LIBJPEG
    LibJpeg::loadImage(filename, lp);
#else
    throwError("LibJPEG is not enabled: %s", filename);
#endif
}

void ImageFile::saveJpeg(StrView filename, ImageA image, SaveParams parms) {
#if HAVE_LIBJPEG
    LibJpeg::saveImage(filename, image, parms);
#else
    throwError("LibJPEG is not enabled: %s", filename);
#endif
}

void ImageFile::saveJpeg(StrView filename, ImageContainerA image, SaveParams parms) {
#if HAVE_LIBJPEG
    LibJpeg::saveImage(filename, image, parms);
#else
    throwError("LibJPEG is not enabled: %s", filename);
#endif
}

void ImageFile::loadJp2(StrView filename, ImageContainer* ic, LoadParams params){
#if HAVE_OPENJPEG
    OpenJpeg::loadImage(filename, ic, params);
#else
    throwError("OpenJPEG is not enabled: %s", filename);
#endif
}

void ImageFile::saveJp2(StrView filaname, ImageA image, SaveParams parms) {
#if HAVE_OPENJPEG
    OpenJpeg::saveImage(filaname, image, parms);
#else
    throwError("OpenJPEG is not enabled: %s", filaname);
#endif
}

void ImageFile::saveJp2(StrView filepath, ImageContainerA image, SaveParams parms) { 
#if HAVE_OPENJPEG
    OpenJpeg::saveImage(filepath, image, parms); 
#else
    throwError("OpenJPEG is not enabled: %s", filepath);
#endif
}

void ImageFile::loadPng(StrView filename, ImageContainer* ic, LoadParams params) {
#if HAVE_LIBPNG
    return LibPng::loadImage(filename, ic, params);
#else
    throwError("LibPNG is not enabled: %s", filename);
#endif
}

void ImageFile::savePng(StrView filename, ImageA image, SaveParams parms) {
#if HAVE_LIBPNG
    LibPng::saveImage(filename, image, parms);
#else
    throwError("LibPNG is not enabled: %s", filename);
#endif
}

void ImageFile::savePng(StrView filepath, ImageContainerA image, SaveParams parms) { 
#if HAVE_LIBPNG
    LibPng::saveImage(filepath, image, parms); 
#else
    throwError("LibPNG is not enabled: %s", filepath);
#endif
}

void ImageFile::loadTiff(StrView filepath, ImageContainer* ic, LoadParams params){
#if HAVE_LIBTIFF
    LibTiff::load(filepath, ic, params);
#else
    throwError("LibTIFF is not enabled: %s", filepath);
#endif
}




void ImageFile::load(StrView filename, ImageContainer* ic, LoadParams params) {
    using Fmt = ImageFileFormat;

    if (Fmt::Unknown == params.ffmt) {
        params.ffmt = guessFileFormat(filename);

        if (Fmt::Unknown == params.ffmt)
            throwError("Unknown or unsupported image file: %s", filename);
    }

    switch(params.ffmt) {
    case Fmt::Bmp:  loadBmp(filename, ic, params);   break;
    case Fmt::Ico:  loadIco(filename, ic, params);   break;
    case Fmt::Icns: loadIcns(filename, ic, params);  break;
#if HAVE_OPENJPEG
    case Fmt::J2k:
    case Fmt::Jpx:
    case Fmt::Jp2:  loadJp2(filename, ic, params);   break;
#endif
#if HAVE_LIBJPEG
    case Fmt::Jpeg: loadJpeg(filename, ic, params);  break;
#endif
#if HAVE_LIBPNG
    case Fmt::Png: loadPng(filename, ic, params); break;
#endif
#if HAVE_LIBTIFF
    case Fmt::Tiff: loadTiff(filename, ic, params);  break;
#endif
    default:
        ADebugTrap();
        break;
    }

    ic->fileFormat = params.ffmt;

    ic->processImagesMaintenance();
}


void ImageFile::save(StrView filename, ImageA image, SaveParams parms)
{
    using Fmt = ImageFileFormat;

    if (Fmt::Unknown == parms.ffmt) {
        parms.ffmt = guessFileFormat(filename);

        verify(Fmt::Unknown != parms.ffmt,
            "Unknown or unsupported image file: %s", filename);
    }

    switch(parms.ffmt) {
    case Fmt::Ico:  saveIco(filename, image, parms);   break;
    case Fmt::J2k:  
    case Fmt::Jp2:  saveJp2(filename, image, parms);   break;
    case Fmt::Jpeg: saveJpeg(filename, image, parms);  break;
    case Fmt::Png:  savePng(filename, image, parms);   break;
    default:
        ADebugTrap();
        break;
    }

}

void ImageFile::save(StrView filename, ImageContainerA image, SaveParams parms)
{
    using Fmt = ImageFileFormat;

    if (Fmt::Unknown == parms.ffmt) {
        parms.ffmt = guessFileFormat(filename);

        verify(Fmt::Unknown != parms.ffmt,
            "Unknown or unsupported image file: %s", filename);
    }

    switch(parms.ffmt) {
    case Fmt::Ico:  saveIco(filename, image, parms);   break;
    case Fmt::Jp2:  
    case Fmt::J2k:  saveJp2(filename, image, parms);   break;
    case Fmt::Jpeg: saveJpeg(filename, image, parms);  break;
    case Fmt::Png:  savePng(filename, image, parms);   break;
    default:
        ADebugTrap();
        break;
    }
}


}  // namespace apl

