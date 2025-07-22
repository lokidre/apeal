/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "./noncopyable.hpp"
#include "./string.hpp"
#include "./str/format.hpp"
#include "./str/sysapi.hpp"

#include <mutex>

namespace apl {

//
// Main console class
//
class Console: public NonCopyable {
    using Self = Console;
    using SelfR = Self&;  // Self return
    using Ref = SelfR;  // Functions return

    // Console output media
    static FILE* out_;
//    static FILE* err_;  // TODO: error

    // Current carret position
    static int carret_;

    static char spaceChar_;

    static char indentChar_;
    static int indentCount_;

    static char tabChar_;
    static int tabCount_;

    static char lfChar_;  // Line Feed (new line) char
    static char crChar_;  // Carriage return char


    static bool atomic_;  // whether to use use mutexes on output

    //using Mutex = std::mutex;
    using Lock = std::unique_lock<std::mutex>;

    static std::mutex mutex_;


private:
    //
    // Core printing function.
    // Returns number of characters printed (cursor moved)
    //
    static int print_core(std::string_view text) noexcept;

    // Core text string and character printing
    static int str_core(StrView text) noexcept;

    // Character printing
    static int ch_core(char c, int count = 1) noexcept;



public:

    //
    // Setup and initialization
    //
    static void setAtomic(bool flag = true) {
         // if (atomic_ == flag)
         //    return;
         atomic_ = flag;
    }

    static void setIndent(int count, Char ch = 0) {
        indentCount_ = count;

        if (ch)
            indentChar_ = ch;
    }

    //
    // Printing
    //



    //
    // String and character printing
    //
    static int str(StrView text) noexcept {
        if (text.empty())
            return 0;

        if (atomic_) {
           Lock lock{mutex_};
           auto count = str_core(text);
           flush();
           return count;
        }

        return str_core(text);
    }

    // String with newline printing
    static int strln(StrView text) noexcept {
        int count = 0;

        if (atomic_) {
            Lock lock{mutex_};

            if (!text.empty())
                count += str_core(text);
            count += ch_core('\n');
            flush();
            return count;
        }

        if (!text.empty())
            count += str_core(text);
        count += ch_core('\n');
        return count;
    }

    // Character printing
    static int ch(char c, int count = 1) noexcept {
        if (atomic_) {
            Lock lock{mutex_};

            ch_core(c, count);
            flush();
            return count;
        }

        return ch_core(c, count);
    }

    static int chln(Char ch, int count = 1) noexcept {
        int c = 0;
        if (atomic_) {
            Lock lock{mutex_};

            c += ch_core(ch, count);
            c += ch_core('\n');
            flush();
            return count;
        }

        c += ch_core(ch, count);
        c += ch_core('\n');
        return c;
    }




    //
    // Control characters printing
    //
    // Equivalent newline functions
    static auto nl(int count = 1) noexcept { return ch(lfChar_, count); }
    static auto n(int count = 1) noexcept { return nl(count); }

    // Carriage return
    static auto cr() noexcept { return ch(crChar_); }


    // Tabulation/Indent
    static auto indent(int count = 1) noexcept {
        return ch(indentChar_, indentCount_ * count);
    }

    static auto tab(int count = 1) noexcept {
        return ch(tabChar_, count);
    }

    static auto space(int count = 1) noexcept {
        return ch(spaceChar_, count);
    }


    //
    // Formatted printing
    //
    template <class ...Args>
    static auto print(StrView fmts, Args&&... args) noexcept {
        return str(fmt(fmts, std::forward<Args>(args)...));
    }

    static auto print(StrView s) noexcept { return str(s); }
    static auto print(Char c) noexcept { return ch(c); }





    //
    // Printing with line feed
    //
    template <class ...Args>
    static auto println(StrView fmts, Args&&... args) noexcept {
        return strln(fmt(fmts, std::forward<Args>(args)...));
    }

    static auto println(StrView text) noexcept { return strln(text); }

    static auto println(Char c) noexcept { return chln(c); }

    template <class ...Args>
    static auto log(StrView fmts, Args&&... args) noexcept {
        return println(fmts, std::forward<Args>(args)...);
    }

    template <class TextT>
    static auto log(TextT text) noexcept { return println(text); }


    template <class ...Args>
    static auto error(StrView fmts, Args&&... args) noexcept {
        return println(fmts, std::forward<Args>(args)...);
    }

    template <class TextT>
    static auto error(TextT text) noexcept { return println(text); }


    //
    // Printing with line feed and indent
    //
    template <class ...Args>
    static auto printiln(int count, StrView fmt, Args&&... args) noexcept {
        auto cnt = indent(count);
        return println(fmt, std::forward<Args>(args)...) + cnt;
    }

    static auto printiln(int count, StrView text) noexcept {
        auto cnt = indent(count);
        return println(text) + cnt;
    }

    static auto printiln(int count, Char c) {
        return indent(count) + chln(c);
    }

    //
    // Printing with tabulation and line feed
    //
    template <class ...Args>
    static int printtln(int count, StrView fmts, Args&&... args) noexcept {
        auto cnt = tab(count);
        cnt += println(fmts, std::forward<Args>(args)...);
        return cnt;
    }

    static int printtln(int count, StrView text) noexcept {
        auto cnt = tab(count);
        cnt += println(text);
        return cnt;
    }

    static int printtln(int count, Char c) noexcept {
        auto cnt = tab(count);
        cnt += chln(c);
        return cnt;
    }



    //
    // Control
    //
    static void flush() noexcept {
        std::fflush(out_);
    }

    //
    // Overloading operators
    //
    //static int operator()(StrView text) noexcept { return println(text); }
    //static int operator()(Char text) noexcept { return println(text); }

    //template <class ...Args>
    //static int operator()(StrView fmt, Args&&... args) noexcept {
    //    return println(fmt, std::forward<Args>(args)...);
    //}


    //
    // iostream-like operator overloading
    //
    // Ref operator<<(StrView text) noexcept { print(text); return *this; }
    // Ref operator<<(Char text) noexcept { print(text); return *this; }

    //// iostream stuff support
    //template <class El, class Traits>
    //Ref operator << (std::basic_ostream<El, Traits>& stream) noexcept {
    //    return *this;
    //}
};

//inline Console console_;  // Standard console


} // namespace apl


#if 0  // Obsolete
namespace apl {


inline void flush_console() noexcept { console_.flush(); }

/*
inline
auto print_console(StrView s) noexcept { return console_.print(s); }

inline
auto print_console(Char s) noexcept { return console_.print(s); }


template <class ...Args>
auto print_console(StrView fmts, Args&&... args) noexcept {
    return console_.print(fmts, std::forward<Args>(args)...);
}
*/

inline
auto print(StrView text) noexcept { console_.print(text); }

inline
auto print(Char ch) noexcept { console_.print(ch); }

template <class ...Args>
auto print(StrView fmt, Args&&... args) noexcept {
    console_.print(fmt, std::forward<Args>(args)...);
}


inline auto println() noexcept { console_.nl(); }

inline auto println(StrView text) noexcept { console_.println(text); }
inline auto println(Char ch) noexcept { console_.println(ch); }

inline auto printtab(char indentChar, Index count) noexcept {
    console_.ch(indentChar, count);
}


template <class ...Args>
auto println(StrView fmt, Args&&... args) noexcept {
    console_.println(fmt, std::forward<Args>(args)...);
}

template <class ...Args>
auto printlns(int spaces, StrView fmt, Args&&... args) noexcept {
    console_.printiln(spaces, fmt, std::forward<Args>(args)...);
}

template <class ...Args>
auto printlnt(int tabs, StrView fmt, Args&&... args) noexcept {
    console_.printtln(tabs, fmt, std::forward<Args>(args)...);
}

inline void conflush() noexcept { console_.flush(); }

}  // namespace apl

#endif
