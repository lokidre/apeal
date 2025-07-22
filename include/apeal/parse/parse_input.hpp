/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../assert.hpp"

#include <array>

namespace apl {

// Input stream reader
template <class It>
struct ParseInput {
    using Iterator = It;
    using Ch = char;

    using IteratorPair = std::pair<Iterator, Iterator>;

    //
    // Setup
    //
    bool ignoreEol{};
    Ch eolChar{'\n'};
    Ch singleQuoteChar{'\''};
    Ch doubleQuoteChar{'\"'};


    //
    // Current input stream status
    //
    Index index{}, line{}, column{};


private:
    std::array<Ch, 128> buffer{};
    Index head{};

    std::array<uint8_t, 3> utf8Signature_{0xef, 0xbb, 0xbf};

    Iterator it, itBegin, itEnd;
    Ch poken{};
    bool poke{};

public:
    ParseInput(Iterator beginIt, Iterator endIt) noexcept
        : itBegin{beginIt}, it{beginIt}, itEnd{endIt} {
        if (it != itEnd) {
            if (uint8_t(*it) == utf8Signature_.front()) {
                for (auto& sch: utf8Signature_) {
                    if (sch != uint8_t(*it))
                        break;
                    ++it;
                }
            }
        }
    }

    void emplace(Iterator beginIt, Iterator endIt) noexcept {
        itBegin = beginIt;
        it = beginIt;
        itEnd = endIt;
    }

    auto getCurrent() const noexcept { return poke ? it - 1 : it; }
    auto getPtr() const noexcept { return &*getCurrent(); }
    auto getDistance(Iterator begin) const noexcept { return std::distance(begin, it); }
    auto makeStrView(Iterator begin) const noexcept {
        return StrView{&*begin, (std::size_t)std::distance(begin, it)};
    }

    auto sourceIterator() const noexcept { return it; }
    auto sourceEnd() const noexcept { return itEnd; }

    auto read() {
        if (poke) {
            poke = false;
            return poken;
        }

        if (head)
            return buffer[--head];

#if _DEBUG
        if (it == itEnd) {
            return '\n';
        }
#endif

        // check end of input
        verify(it != itEnd, "End of input reached");


        auto ch = *it;
        ++it;

        ++index;
        ++column;

        if (eolChar == ch) {
            ++line;
            column = 0;
        }

        return ch;
    }

    auto get() { return read(); }

    void skip() {
        if (end())
            return;
        read();
    }

    void advance(Index count) {
        if (count <= 0)
            return;

        while (count > 0) {
            skip();
            --count;
        }
    }

    auto peek() {
        if (poke)
            return poken;

        if (head)
            return buffer[head - 1];

        poken = read();
        poke = true;
        return poken;
    }

    void unread(Ch uc) { buffer[head++] = uc; }

    void unread(Ch* a, Index n) {
        // needed better
        auto p = &a[n - 1];
        for (Index i{}; i < n; ++i, --p)
            unread(*p);
    }

    bool isSpace() {
        auto ch = peek();

        if (ignoreEol && eolChar == ch)
            return true;

        return ' ' == ch || '\t' == ch || '\r' == ch;
    }

    bool eol() {
        if (eof())
            return true;
        return peek() == eolChar;
    }

    bool eof() const noexcept { return head == 0 && !poke && it == itEnd; }
    bool end() const noexcept { return eof(); }


    void skipSpaces() {
        while (!end() && isSpace())
            skip();
    }

    void skipSpacesEol() {
        while (!end() && (isSpace() || eol()))
            skip();
    }


    auto readAfterSpaces() {
        skipSpaces();
        return read();
    }

    auto peekAfterSpaces() {
        skipSpaces();
        return peek();
    }


    // More specific functions
    bool isQuote() { return isSingleQuote() || isDoubleQuote(); }
    bool isSingleQuote() { return peek() == singleQuoteChar; }
    bool isDoubleQuote() { return peek() == doubleQuoteChar; }


    // Convenient "reading" functions

    template <class Parser>
    IteratorPair readNumberIt(Parser& p) {
        skipSpaces();
        verify(!eof(), "Unexpected End");

        auto begin = getCurrent();
        // auto end = begin;

        auto ch = read();
        verify(p.isDigit(ch) || p.isNumberSign(ch) || p.isFloatPeriod(ch), "Invalid char: %c", ch);

        while (!eof() && p.isNumberBody(peek()))
            skip();
        //++end; //s += read();
        return {begin, getCurrent()};
    }


    //
    // Reading numbers using strings
    //
    template <class Parser, class S>
    S& readNumber(Parser& p, S& s) {
        skipSpaces();
        verify(!eof(), "Unexpected End");

        s.clear();

        auto ch = read();
        verify(p.isDigit(ch) || p.isNumberSign(ch) || p.isFloatPeriod(ch), "Invalid char: %c", ch);

        s += ch;

        while (!eof() && p.isNumberBody(peek()))
            s += read();

        return s;
    }

    String readNumber() {
        String s;
        readNumber(s);
        return s;
    }
};

} // namespace apl
