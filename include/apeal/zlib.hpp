/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <string>
#include <istream>
#include <ostream>

#include "policy.hpp"
#include "buffer.hpp"
#include "file.hpp"
#include "types.hpp"



namespace apl {

struct ArchiveStreamBase {
    virtual ~ArchiveStreamBase() noexcept{}
    virtual Index write(const void* buffer, Index count) = 0;
    virtual Index read(void* buffer, Index count) = 0;
};

template <class Processor>
struct ArchiveBuffer: public ArchiveStreamBase {
    constexpr ArchiveBuffer(Processor& buffer) noexcept: buffer_{buffer} {}

    Index write(const void* buffer, Index count) override {
        buffer_.append((const char*)buffer, count);
        return count;
    }

    Index read(void* buffer, Index count) override {
        memcpy(buffer, ((uint8_t*)buffer_.data()) + bufferOffset_, count);
        bufferOffset_ += count;
        return count;
    }

private:
    Processor& buffer_;
    Index bufferOffset_{};
};

template <class File>
struct ArchiveFile: public ArchiveStreamBase {
    ArchiveFile(File& file) noexcept: file_(file) {}

    Index write(const void* buffer, Index count) override {
        return file_.write(buffer, count);
    }

    Index read(void* buffer, Index count) override {
        return file_.readSome(buffer, count);
    }

private:
    File& file_;
};


class ZLibImpl {
public:
    ZLibImpl();
    ~ZLibImpl();

protected:
    Index deflateCore(ArchiveStreamBase* input, ArchiveStreamBase* output, bool gzip = false);
    Index inflateCore(ArchiveStreamBase* input, ArchiveStreamBase* output, bool gzip = false);

    void inflateCoreBegin(ArchiveStreamBase* output, bool gzip = false);
    void inflateCoreFeed(ArchiveStreamBase* output, const void* buffer, Index count);
    void inflateCoreEnd(ArchiveStreamBase* output);

    void handleError(int error_code, const char* msg) {
        throw std::runtime_error(std::string("zlib:") + msg);
    }

    ByteBuffer inputBuffer_, outputBuffer_;
    Index bufferSize_{256*1024};

private:
    void* zStream_{};
};



class ZLib: NonCopyable, public ZLibImpl {

public:
    Index inflateGzipFile(StrView filename, StrView outfile)
    {
        File inFile, outFile;
        inFile.open(filename, AFileAccess::Read);
        outFile.create(outfile, AFileCreate::CreateAlways, AFileAccess::Write);

        ArchiveFile<File> inStream{inFile}, outStream{outFile};

        return inflateCore(&inStream, &outStream, true);
    }

    Index deflateGzipFile(StrView filename, StrView outfile)
    {
        File inFile, outFile;
        inFile.open(filename, AFileAccess::Read);
        outFile.create(outfile, AFileCreate::CreateAlways, AFileAccess::Write);

        ArchiveFile<File> inStream(inFile), outStream(outFile);

        return deflateCore(&inStream, &outStream, true);
    }

private:
    File outFile_;
    ArchiveFile<File> outFileStream_{outFile_};

    std::unique_ptr<ArchiveStreamBase> outBufferStream_{};

public:

    void inflateFileBegin(StrView outfilename, bool gzip = false) {
        outFile_.create(outfilename, FileCreate::CreateAlways, FileAccess::Write);
        inflateCoreBegin(&outFileStream_, gzip);
    }

    void inflateFileFeed(const void* buffer, Index count) {
        inflateCoreFeed(&outFileStream_, buffer, count);
    }

    void inflateFileEnd() { inflateCoreEnd(&outFileStream_); }


    void inflateStringBegin(String *outString, bool gzip = false) {
        outBufferStream_ = std::make_unique<ArchiveBuffer<String>>(*outString);
        inflateCoreBegin(outBufferStream_.get(), gzip);
    }
    void inflateStringFeed(const void* buffer, const int count) {
        inflateCoreFeed(outBufferStream_.get(), buffer, count);
    }

    void inflateStringEnd() {
        inflateCoreEnd(outBufferStream_.get());
        outBufferStream_.reset();
    }

};

}  // namespace apl


