/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/image.hpp"

#include "../include/apeal/file.hpp"
#include "../include/apeal/image/ico.hpp"

#include "../include/apeal/endian.hpp"


namespace apl {

void ImageFile::loadIco(StrView filename, ImageContainer* ic, LoadParams lp)
{
    ImageContainer ii{};

    auto contents = loadFile(filename);

    verify(contents.size() >= sizeof(IcoFileHeader) + sizeof(IcoDirEntry), 
        "Invalid ico file: %s", filename);

    auto data = contents.data();

    auto icoHdr = reinterpret_cast<const IcoFileHeader *>(data);

    auto imageType = from_le_16(icoHdr->image_type);
    switch (imageType) {
    case 1: ii.dict["ico.image_type"] = "ico"; break;
    case 2: ii.dict["ico.image_type"] = "cur"; break;
    default: throwError("Invalid icon format: %s", filename);
    }

    Index numImages = from_le_16(icoHdr->images_count);

    verify(contents.size() >= sizeof(IcoFileHeader) + sizeof(IcoDirEntry) * numImages, 
        "Corrupted ico file: %s", filename);

    auto icoEntry = reinterpret_cast<const IcoDirEntry *>(data + sizeof(IcoFileHeader));

    ii.images.reserve(numImages);

    for (Index imageIndex = 0; imageIndex < numImages; ++imageIndex) {
        auto& frame = ii.images.emplace_back();

        frame.size = {icoEntry->width, icoEntry->height};
        if (!frame.size.w)
            frame.size.w = 256;
        if (!frame.size.h)
            frame.size.h = 256;

        ++icoEntry;
    }

    ii.fileFormat = ImageFileFormat::Ico;
}


void ImageFile::saveIco(StrView filename, ImageA image, SaveParams parms)
{
    throwError("ImageFile::saveIco: Not implemented");
}

void ImageFile::saveIco(StrView filename, ImageContainerA image, SaveParams parms)
{
    throwError("ImageFile::saveIco: Not implemented");
}


} // namespace apl

