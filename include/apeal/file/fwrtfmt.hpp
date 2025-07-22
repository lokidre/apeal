/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

namespace apl {


//
// Writting formatted text to the file
//
class FileWriteFormat {

    using Self = FileWriteFormat;
    using Char = char;

    Index bufferOffset_{};
    ByteBuffer buffer_{};
    File file_{};

    bool bufferOnly = false;
    char indentChar = ' ';

    String filename_;

    // TODO: Add formatting options like "indent"
    // TODO: Add option like "indent char"



public:  // Instantiation
    FileWriteFormat() noexcept {}

    explicit FileWriteFormat(StrView filename) {
        open(filename);
    }

    ~FileWriteFormat() noexcept {
        // Make sure we don't throw from destructor
        try {
            if (!bufferOnly)
                close();
        }
        catch (...) {
            ADebugPrintFunc("Destructor Exception");
        }
    }



private:
    void checkBuffer() {
        if (buffer_)
            return;
        buffer_.alloc(1024*10);
    }

    void checkCreated() {
        if (file_.valid())
            return;
        file_.create(filename_);
    }

    void flushBuffer() {
        if (!bufferOffset_)
            return;
        checkCreated();
        file_.write(buffer_.data(), bufferOffset_);
        bufferOffset_ = 0;
        file_.flush();
    }

    void prepareBuffer(size_t size) {
        checkBuffer();

        if (bufferOffset_ + size > buffer_.size()) {
            if (!bufferOnly)
                flushBuffer();
            else
                buffer_.resize(std::max(size, buffer_.size())*2);
        }
    }

    Self& writeData(const void* data, size_t size) {
        prepareBuffer(size);

        memcpy(buffer_.get() + bufferOffset_, data, size);
        bufferOffset_ += size;
        return *this;
    };

    Self& writeChar(Char ch) {
        prepareBuffer(1);
        buffer_.get()[bufferOffset_++] = ch;
        return *this;
    }


public:
    void open(StrView filename) {
        // close();
        filename_ = filename;
        bufferOffset_ = 0;
        // buffer_.clear();
    }

    void close() {
        if (filename_.empty())
            return;
        commit();
        file_.close();
        buffer_.release();
        filename_.clear();
    }


    Self& write(StrView text) { return writeData(text.data(), text.size()); }

    template <class ...Args>
    Self& write(StrView fmt, const Args&... args) { 
        return write(sformat(fmt, args...));
    }

    Self& writeln(StrView text) { return write(text).nl(); }

    template <class ...Args>
    Self& writeln(StrView fmt, const Args&... args) { 
        return write(fmt, args...).nl(); 
    }

    Self& t(int count = 1) { return ch('\t', count); }
    Self& n(int count = 1) { return ch('\n', count); }
    Self& nl(int count = 1) { return ch('\n', count); }


    Self& ch(Char c, int count = 1) { return writeChar(c); }

    Self& commit() {
        checkCreated();
        flushBuffer();
        return *this;
    }


};

}  // namespace apl
