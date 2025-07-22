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


template<class S, typename C, class V, class W>
S& str_escape(const std::string& src, const C& escapeChar, const V& escapedChars, const W& escapedValues, S& s) {
    s.clear();

    for (auto& c: src) {
        //if ( c == escapeChar ) {  // escape the escape char
        //  s += escapeChar ;
        //  s += c ;
        //  continue ;
        //}

        // search the current character in escape list
        size_t idx = 0;
        bool found = false;
        for (auto& t: escapedChars) {
            if (t == c) {
                s += escapeChar;
                s += escapedValues[idx];
                found = true;
                break ;
            }
            ++idx;
        }
        if (found)
            continue ;

        s += c;
    }

    return s;
}

template<typename C, class V, class W>
const std::string sescape(const std::string& src, const C& escapeChar, const V& escapedChars, const W& escapedVals) {
    std::string s;
    str_escape(src, escapeChar, escapedChars, escapedVals, s);
    return s;
}


namespace apl_private {

template<class It>
bool compareEscapedChar(It c, It end, char t) {
    return *c == t;
}

template<class It>
bool compareEscapedChar(It c, It end, const std::string& t) {
    for (auto& tc: t) {
        if (c == end)
            return false;
        if (*c != tc)
            return false;
        ++c;
    }
    return true;
}

}


template<class S, class It, class C, class VIt, class WIt>
const S& str_unescape_append(It src, It srcEnd, const C& escapeChar, VIt escapedChars, VIt escapedCharsEnd,
                             WIt escapedValues, WIt escapedValuesEnd, S& s) {

    for (auto c = src; c != srcEnd; ++c) {
        if (*c == escapeChar) {
            // escape the escape char
            ++c; // the escape character itself is suppose to be removed
            if (c == srcEnd) // just caution
                break ;

            // now search for the replacement
            size_t idx = 0;
            for (auto t = escapedValues; t != escapedValuesEnd; ++t) {
                if (apl_private::compareEscapedChar(c, srcEnd, *t)) {
                    s += escapedChars[idx];
                    break ;
                }
                ++idx;
            }
            continue ;
        }

        s += *c;
    }

    return s;
}

template<class S, class It, class C, class VIt, class WIt>
const S& str_unescape(It src, It srcEnd, const C& escapeChar, VIt escapedChars, VIt escapedCharsEnd, WIt escapedValues,
                      WIt escapedValuesEnd, S& s) {
    s.clear();
    return str_unescape_append(src, srcEnd, escapeChar, escapedChars, escapedCharsEnd, escapedValues, escapedValuesEnd,
                               s);
}



template<class S, class C, class V, class W>
const S& str_unescape(const std::string& src, const C& escapeChar, const V& escapedChars, const W& escapedValues,
                      S& s) {
    return str_unescape(src.begin(), src.end(), escapeChar, escapedChars.begin(), escapedChars.end(),
                        escapedValues.begin(), escapedValues.end(), s);
}

template<class S, class C>
const S& str_unescape(const std::string& src, const C& escapeChar, const char* escapedChars, const char* escapedValues,
                      S& s) {
    return str_unescape(src.begin(), src.end(), escapeChar, escapedChars, escapedChars + strlen(escapedChars),
                        escapedValues, escapedValues + strlen(escapedValues), s);
}


template<class S, class C, class W>
const S& str_unescape(const std::string& src, const C& escapeChar, const char* escapedChars, const W& escapedValues,
                      S& s) {
    return str_unescape(src.begin(), src.end(), escapeChar, escapedChars, escapedChars + strlen(escapedChars),
                        escapedValues.end(), escapedValues.end(), s);
}



template<typename C, class V, class W>
auto sunescape(const std::string& src, const C& escapeChar, const V& escapedChars, const W& escapedVals) {
    std::string s;
    str_unescape(src, escapeChar, escapedChars, escapedVals, s);
    return s;
}


}  // namespace apl

