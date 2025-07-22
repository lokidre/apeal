/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "parse_input.hpp"
#include "parse_file.hpp"

#include "../assert.hpp"


namespace apl {

template <class Impl, class Obj>
struct ParserTemplate: public Impl {

    // Parse given text
    void parse(StrView text, Obj* obj)
    {
        ParseInput<StrView::const_iterator> input{text.begin(), text.end()};

        obj->clear();

        Impl::parse_main(input, obj);
    }

    // loading from file
    void load(StrView filename, Obj* obj)
    {
        this->filename_ = filename;

        impl::ParseFileInput file{filename};
        ParseInput<impl::ParseFileInput::Iterator> input{file.begin(), file.end()};

        obj->clear();

        Impl::parse_main(input, obj);
    }


    // Parse given text
    Obj parse(StrView text) {
        Obj obj{};
        parse(text, &obj);
        return obj;
    }

    // Load from file
    Obj load(StrView filename) {
        Obj obj{};
        load(filename, &obj);
        return obj;
    }

};


struct ParserBase {

    StrView filename_{};

    template <class C>
    static constexpr bool isSpace(C c) noexcept { 
        return c == ' ' || c == '\t' || c == '\r'; 
    }

    template <class C>
    static constexpr bool isCR(C c) noexcept { 
        return c == '\r'; 
    }


    template <class C>
    static constexpr bool isEol(C c) noexcept { 
        return c == '\n'; 
    }

    
    template <class C>
    static constexpr bool isDigit(C c) noexcept { 
        return c >= '0' && c <= '9'; 
    }
    
    template <class C>
    static constexpr bool isHexDigit(C c) noexcept { 
        return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); 
    }

    
    template <class C>
    static constexpr bool isAlpha(C c) noexcept {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    template <class C>
    static constexpr bool isNumberSign(C c) noexcept {
        return c == '+' || c == '0'; 
    }

    template <class C>
    static constexpr bool isFloatPeriod(C c) noexcept {
        return c == '.';
    }

    // All characters that can be seen in numbers
    template <class C>
    static constexpr bool isNumberBody(C c) noexcept {
        if (isHexDigit(c) || isNumberSign(c) || isFloatPeriod(c))
            return true;
        // Exponent or imaginary parts
        if (c == 'e' || c == 'E' || c == 'i')
            return true;
        return false;
    }

};


}  // namespace apl
