/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/zlib.hpp"


#include "../3rdparty/zlib/zlib_zconf.h"
#include "../3rdparty/zlib/zlib_zlib.h"

namespace apl {

ZLibImpl::ZLibImpl()
{
}

ZLibImpl::~ZLibImpl()
{
    if (zStream_) {
        auto zs = (z_stream*)zStream_;
        delete zs;
        zStream_ = 0;
    }
}


Index ZLibImpl::deflateCore(ArchiveStreamBase* input, ArchiveStreamBase* output, bool gzip)
{
    Index count{}, total_count{};
    int level = Z_DEFAULT_COMPRESSION;

    inputBuffer_.resize(bufferSize_);
    outputBuffer_.resize(bufferSize_);

    z_stream stream{};
    int ret = gzip ? deflateInit2(&stream, level, Z_DEFLATED, 16 + MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY) : deflateInit(&stream, level);

    if (ret != Z_OK)
        handleError(ret, stream.msg);

    // compress
    for (;;) {
        //	if ( stream.avail_in == 0 ) {
        stream.next_in = (Bytef*)inputBuffer_.get();
        stream.avail_in = (uInt)input->read(inputBuffer_.get(), (int)inputBuffer_.size());

        bool eof = stream.avail_in == 0;

        do {
            stream.next_out = (Bytef*)outputBuffer_.get();
            stream.avail_out = (uInt)outputBuffer_.size();

            if (eof) {
                ret = ::deflate(&stream, Z_FINISH);
            } else {
                ret = ::deflate(&stream, Z_NO_FLUSH);
            }

            if (ret != Z_OK && ret != Z_STREAM_END) {
                deflateEnd(&stream);
                handleError(ret, stream.msg);
            }

            count = outputBuffer_.size() - stream.avail_out;

            if (count) {
                output->write(outputBuffer_.get(), count);
                total_count += count;
            }

            if (ret == Z_STREAM_END)
                break;

        } while (stream.avail_out == 0);

        if (eof)
            break;
    }

    ret = deflateEnd(&stream);

    return total_count;
}

Index ZLibImpl::inflateCore(ArchiveStreamBase* input, ArchiveStreamBase* output, bool gzip)
{
    Index total{}, count{};

    inputBuffer_.resize(bufferSize_);
    outputBuffer_.resize(bufferSize_);

    z_stream stream{};

    int ret = gzip ? inflateInit2(&stream, 16 + MAX_WBITS) : inflateInit(&stream);

    if (ret != Z_OK)
        handleError(ret, stream.msg);

    for (;;) {

        do {
            if (stream.avail_in == 0) {
                stream.next_in = (Bytef*)inputBuffer_.get();
                stream.avail_in = (uInt)input->read(inputBuffer_.get(), inputBuffer_.size());
            }

            stream.next_out = (Bytef*)outputBuffer_.get();
            stream.avail_out = (uInt)outputBuffer_.size();

            ret = ::inflate(&stream, Z_NO_FLUSH);

            if (ret != Z_OK && ret != Z_STREAM_END) {
                inflateEnd(&stream);
                handleError(ret, stream.msg);
            }

            count = outputBuffer_.size() - stream.avail_out;
            output->write(outputBuffer_.get(), count);

            total += count;
        } while (stream.avail_out == 0);

        if (ret == Z_STREAM_END)
            break;
    }

    ret = inflateEnd(&stream);

    return total;
}




void ZLibImpl::inflateCoreBegin(ArchiveStreamBase* output, bool gzip)
{
    z_stream* stream = new z_stream;
    zStream_ = stream;

    memset(stream, 0, sizeof(z_stream));

    if (gzip) {
        inflateInit2(stream, 16 + MAX_WBITS);
    } else {
        inflateInit(stream);
    }

    //int ret = gzip ? inflateInit2(stream, 16+MAX_WBITS) : inflateInit(stream);

    outputBuffer_.resize(bufferSize_);
}

void ZLibImpl::inflateCoreFeed(ArchiveStreamBase* output, const void* buffer, Index count)
{
    auto stream = (z_stream*)zStream_;

    stream->next_in = (Bytef*)buffer;
    stream->avail_in = (uInt)count;

    do {
        if (stream->avail_in == 0)
            break;

        stream->next_out = (Bytef*)outputBuffer_.get();
        stream->avail_out = (uInt)outputBuffer_.size();

        auto ret = ::inflate(stream, Z_NO_FLUSH);

        if (ret != Z_OK && ret != Z_STREAM_END) {
            inflateEnd(stream);
            handleError(ret, stream->msg);
        }

        if (auto pending = outputBuffer_.ssize() - stream->avail_out; pending)
            output->write(outputBuffer_.get(), pending);

        if (ret == Z_STREAM_END)
            return;

        //total += count ;
    } while (stream->avail_out == 0);
}


void ZLibImpl::inflateCoreEnd(ArchiveStreamBase* output)
{
    auto zstream = (z_stream*)zStream_;
    int ret;

    zstream->next_out = (Bytef*)outputBuffer_.get();
    zstream->avail_out = (uInt)outputBuffer_.size();

    ret = ::inflate(zstream, Z_FINISH);

    if (ret != Z_OK && ret != Z_STREAM_END) {
        inflateEnd(zstream);
        handleError(ret, zstream->msg);
    }

    Index count = outputBuffer_.size() - zstream->avail_out;
    if (count)
        output->write(outputBuffer_.get(), count);


    ret = inflateEnd(zstream);
}


}  // namespace apl
