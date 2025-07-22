/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/image.hpp"

#include "../include/apeal/image/bmp.hpp"

#include "../include/apeal/file.hpp"

//
// Mime: image/x-ms-bmp
//

/*
class ABmpImageFile {
public:
    // BMP file constants and structures
    enum {
        bmp_file_header_size = 14,
        bmp_info_header_size = 40
    };

public:
    ABmpImageFile()
    {
    }


public:
    bool check_extension(const char* s)
    {
        //#ifdef __HAVE_STRICMP
        //if ( stricmp_s(s,"bmp") == 0 )
        //	return true ;
        //#else
        if (_stricmp(s, "bmp") == 0)
            return true;
        //#endif

        return false;
    }

    // read the data from file
    bool check_format(const char* filename)
    {
        // we have to read file header and info header sizes of data
        File file;
        const int data_size = bmp_file_header_size + bmp_info_header_size;
        file.open(filename, File::AccessMode::Read);
        char image_data[data_size];
        int read_size = file.read(image_data, data_size);

        return check_format(image_data, read_size);
    }

    // read the data from buffer
    bool check_format(const void* buffer, const int size)
    {
        if (size < bmp_file_header_size + bmp_info_header_size)
            return false;

        if (memcmp(buffer, "BM", 2) != 0)
            return false;

        return true;
    }

    // load from file
    bool load_file(const char* filename, AImageData& image)
    {
        File file;
        file.open(filename, file.access_read);
        const int data_size = bmp_file_header_size + bmp_info_header_size;
        char header_data[data_size];
        file.read(header_data, data_size);

        BmpFileHeader fh;
        BmpInfoHeader ih;

        load_bmp_file_header(header_data, fh);
        load_bmp_info_header(header_data + bmp_file_header_size, ih);


        // now decide if we need color pallette 
        int color_table_size = ih.clrs_used;
        Buffer<char> color_table_buffer;

        if (color_table_size == 0) {
            // check if we still need the color table
            switch (ih.bits_per_pixel) {
            case 1: color_table_size = 2;
            case 4: color_table_size = 16;
            case 8: color_table_size = 256;
            default: break;
            }
        }

        // read color table if we have to
        if (color_table_size != 0) {
            color_table_buffer.resize(color_table_size*4);
            file.read(color_table_buffer.get(), color_table_size*4);
        }



        // now load the data
        unsigned long image_offset, image_size;

        // image data suppose to be provided in the header
        // if it's missing we assume that it follows immediately
        image_offset = fh.off_bits;
        if (image_offset == 0)
            image_offset = bmp_file_header_size + bmp_info_header_size;

        // image_size is provided in the header
        // but if image is not compressed it could be 0, prim we have to compute it
        image_size = ih.image_size;
        if (image_size == 0) {
            image_size = ih.width * ih.height * ih.bits_per_pixel / 8;
            if (ih.planes != 1)
                image_size *= ih.planes;
        }

        // now allocate the buffer for the image and load it
        Buffer<char> image_buffer(image_size);

        file.seek(image_offset, file.pos_begin);
        file.read(image_buffer.get(), image_size);

        file.close();  // we don't need the file anymore

        return load_bmp_data(fh, ih, color_table_buffer.get(), color_table_size, image_buffer.get(), image_size, image);
    }

    // load image from buffer 
    bool load_buffer(const void* buffer, const int size, AImageData& image)
    {
        //const char *ptr = (const char *)buffer ;

        //if ( size < bmp_file_header
        //load_bmp_file_header(header_data,fh) ;
        //load_bmp_info_header(header_data + bmp_file_header_size,ih) ;

        return true;
    }


    // save image to file
    bool save_file(const char* filename, const AImageData& image)
    {
        return true;
    }

    // generate image in buffer
    bool save_buffer(void* buffer, const int size, AImageData& image)
    {
        return true;
    }

private:
    void load_bmp_file_header(const void* buffer, BmpFileHeader& fh)
    {
        const char* ptr = (const char*)buffer;

        fh.header = htobs(*(uint16_t*)ptr), ptr += 2;
        fh.size = htobl(*(uint32_t*)ptr), ptr += 4;
        fh.res = htobl(*(uint32_t*)ptr), ptr += 4;
        fh.off_bits = htobl(*(uint32_t*)ptr), ptr += 4;
    }

    void load_bmp_info_header(const void* buffer, BmpInfoHeader& ih)
    {
        const char* ptr = (const char*)buffer;

        ih.bih_size = htobl(*(uint32_t*)ptr), ptr += 4;  // 40 - size of the structure in bytes
        ih.width = htobl(*(uint32_t*)ptr), ptr += 4;
        ih.height = htobl(*(uint32_t*)ptr), ptr += 4;
        ih.planes = htobs(*(uint16_t*)ptr), ptr += 2;
        ih.bits_per_pixel = htobs(*(uint16_t*)ptr), ptr += 2;
        ih.compression = htobl(*(uint32_t*)ptr), ptr += 4;
        ih.image_size = htobl(*(uint32_t*)ptr), ptr += 4;
        ih.x_pels_per_meter = htobl(*(uint32_t*)ptr), ptr += 4;
        ih.y_pels_per_meter = htobl(*(uint32_t*)ptr), ptr += 4;
        ih.clrs_used = htobl(*(uint32_t*)ptr), ptr += 4;
        ih.clrs_important = htobl(*(uint32_t*)ptr), ptr += 4;
    }


    bool load_bmp_data(const BmpFileHeader& fh, const BmpInfoHeader& ih, const void* color_table, const int color_table_size, const void* image_data, const int image_data_size, AImageData& image)
    {
        image.page_count = 1;
        image.selected_page = 0;

        AImageData::Frame& frame = image.frames[image.selected_page];

        frame.geometry.width = ih.width;
        frame.geometry.height = ih.height;
        frame.planes = 1;
        frame.depth[0] = ih.bits_per_pixel;
        frame.alignment = 32;

        if (color_table_size) {
            frame.palette.resize(color_table_size);
            frame.format = image.format_palette;
            memcpy(frame.palette.get(), color_table, color_table_size);
        } else {
            frame.format = image.format_rgb;
        }

        frame.image.resize(image_data_size);
        memcpy(frame.image.get(), image_data, image_data_size);


        return true;
    }

};
*/


namespace apl {

void ImageFile::loadBmp(StrView filename, ImageContainer* ic, LoadParams params)
{
    ic->clear();
    try {

        ByteBuffer fileBuffer;
        loadFile(filename, &fileBuffer);

        verify(fileBuffer.size() > BmpFileHeaderSize + BmpInfoHeaderSize, "File invalid or empty");

        //auto fileHeader = reinterpret_cast<BmpFileHeader*>(fileBuffer.get());
        //auto infoHeader = reinterpret_cast<BmpInfoHeader*>(fileBuffer.get() + BmpFileHeaderSize);

    }
    catch(const std::exception& e) {
        throwError("%s: %s", filename, e.what());
    }
}


void ImageFile::saveBmp(StrView filename, ImageA image, SaveParams parms)
{
    throwError("ImageFile::saveBmp: Not implemented");
}


} // namespace apl


