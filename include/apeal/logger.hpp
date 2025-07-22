/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#if 0 // DocHeader
Log Class
---

Logger class allows applications create, write, and manage log files.

Log maintenance can be roughly divided into following cathegories:

    1. 	Unlimited file growth. Simple, but might be dangerous if application
        has a lot of inormation to log.

    2.	Circular overwrite. Good for small application if only latest actions
        are important.

    3.	New file generation. The solution for large and complex applications.
        The drawback is number of log files being generated and often
        requirement to keep separate 'log directory' for such applications.
        New log files can be generated based by two criterias:

        3a.	Date/time parameter.

        3b.	Log file reaching given maximum size.


It is conventient to have timestamp for each log record. By default the class writes
current date/time at the beginning of each record. This behaviour can be customized.


The class is completely thread safe.

By default Log class will not throw error if itself encountered an error.
This is due to the fact that the log file is often used	to log errors. This
behaviour can be customized, if application wants to make sure that logs were
written succefully.

#endif


#include "file.hpp"
#include "mutex.hpp"

#include <mutex>
#include <stdexcept>


namespace apl {


//
// Rare log
// The file gets opened and closed

class Logger {
private:
    using Mutex = std::mutex;
    using Lock = std::scoped_lock<Mutex>;


public:
    Logger() {}

    Logger(StrView filename) { setLogFile(filename); }


private:                               // setup
    Index maxSize_ = 1024 * 1024 * 16; // 16 M
    String logFile_;

    Mutex mutex_;


public:
    void setMaxSize(Index s) { maxSize_ = s; }
    void setLogFile(StrView s) { logFile_ = s; }

    //
    // options
    //
private:
    bool usePrefix_{true};
    bool useEndl_{true};

public:
    // bool use_crlf_ = false ;
    bool useSpace_{true};
    char separator_{'|'};

    void set_use_prefix(bool b) { usePrefix_ = b; }
    void set_use_endl(bool b) { useEndl_ = b; }


private:
    void writeData(const void* data, Index size) noexcept {
        Lock lock(mutex_);

        try {
            File f;
            // String s;

            f.create(logFile_, FileCreate::OpenAlways, FileAccess::ReadWrite, FileAttribute::Normal,
                     FileShare::Read);
            f.seek(0, FileOrigin::End);

            // s.clear();

            if (usePrefix_) {
                f.write(makeTimestamp());
            }

            f.write(data, size);

            if (useEndl_) {
                f.writeChar('\n');
            }


            // check the size of log file
            Index siz = f.size();
            if (maxSize_ && siz > maxSize_) {
                auto newSize = siz - maxSize_ / 3;
                ByteBuffer d;
                d.alloc(newSize);
                f.seek(siz - newSize);
                f.read(d.get(), newSize);

                // find the new line
                auto p = d.get();
                while (newSize) {
                    if (*p == '\n') {
                        ++p;
                        --newSize;
                        break;
                    }
                    ++p;
                    --newSize;
                }

                f.seek(0);
                if (newSize)
                    f.write(p, newSize);
                f.trunc();
            }

            f.close();
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        } catch (...) {
            std::cerr << logFile_ << ": "
                      << "log exception" << std::endl;
        }
    }

public: // service
    void write(std::span<std::uint8_t> message) noexcept {
        writeData(message.data(), message.size());
    }

    void write(StrView message) noexcept { writeData(message.data(), message.size()); }

    template <class... Args>
    void format(StrView fmt, Args&&... args) {
        write(sformat(fmt, std::forward<Args>(args)...));
    }

    void format(StrView fmt) noexcept { write(fmt); }


    template <class... Args>
    void operator()(Args&&... args) noexcept {
        format(std::forward<Args>(args)...);
    }

    void operator<<(int i) noexcept { write(sformat("%d", i)); }

    template <class T>
    void operator<<(const T& v) {
        write(sformat("%d", v));
    }

    /*
    void operator << ( const size_t i )
    {
        write ( sformat ( "%d",i) ) ;
    }

    void operator << ( const char c ) { 	write(&c,1) ; }
    void operator << ( const bool b ) { 	operator << (b?1:0) ; }
    void operator << ( const char *s ) { 	write(s,(int)strlen(s)) ; }


    void operator << ( const typename string_type::value_type &c )
    {
        write(&c,sizeof(typename string_type::value_type)) ;
    }

    void operator << ( const string_type &s )
    {
        write(s) ;
    }
    */


    void clear() {
        Lock lock(mutex_);

        try {
            File f;
            f.create(logFile_, FileCreate::OpenAlways, FileAccess::ReadWrite, FileAttribute::Normal,
                     FileShare::Read);
            f.trunc();
        } catch (...) {
        }
    }

private:
    String makeTimestamp() noexcept {
        Time t{TimeZone::Local};

        auto ts =
            fmt("%04d-%02d-%02d %02d:%02d:%02d", t.Year, t.Month, t.Day, t.Hour, t.Min, t.Sec);

        if (useSpace_)
            ts += ' ';
        ts += separator_;
        if (useSpace_)
            ts += ' ';
        return ts;
    }
};

} // namespace apl
