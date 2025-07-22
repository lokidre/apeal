/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <cstring>

#include "../str/trim.hpp"

namespace apl {

// TODO: implement everything for the string_view

template <class I, class Char, class Str>
void str_split_it(I begin, I end, Char sep, Str &left, Str &right) noexcept
{
    auto c = begin;

    while (c != end && *c != sep)
        ++c;

    //left.assign(begin, c);
    auto count = (std::size_t)std::distance(begin, c);
    if (count)
        left = Str{&*begin, count};
    else
        left = {};
    //left = {begin, c}; //Str{&*begin, std::distance(begin, c)};

    if (c == end) {
        //right.clear();
        right = {};
        return;
    }

    ++c;

    if (c == end) {
        //right.clear();
        right = {};
        return;
    }

    //right.assign(c, end);
    count = (std::size_t)std::distance(c, end);
    if (count)
        right = {&*c, count};
    else
        right = {};
}

template <class I, class C, class O>
void str_split(I begin, I end, C delim, O &left, O &right) noexcept {
    str_split_it(begin, end, delim, left, right);
}

//template <class C, class O>
//inline
//void str_split(char *str, C delim, O &left, O &right) {
//    str_split_basic(str, str+strlen(str), delim, left, right);
//}


//template <class C, class O>
//void str_split(const char *str, C delim, O &left, O &right) {
//    str_split_basic(str, str+strlen(str), delim, left, right);
//}



template <class Sep, class O>
void str_split(StrView str, Sep sep, O &left, O &right) noexcept {
    //str_split_it(str.data(), str.data() + str.size(), sep, left, right);
    str_split_it(str.begin(), str.end(), sep, left, right);
}

template <class Sep, class O>
void str_split_trim(StrView str, Sep sep, O &left, O &right) noexcept {
    //str_split_it(str.data(), str.data() + str.size(), sep, left, right);
    str_split_it(str.begin(), str.end(), sep, left, right);
    left = str_trim(left);
    right = str_trim(right);
}



// This assumes "std::string" ish class
template <class Char>
auto str_split(StrView str, Char sep) noexcept {
    std::pair<std::string_view, std::string_view> ret;
    str_split(str, sep, ret.first, ret.second);
    return ret;
}


template <class SepT>
auto str_split_trim(StrView text, SepT sep) noexcept {
    std::pair<std::string_view, std::string_view> ret;
    str_split(text, sep, ret.first, ret.second);
    ret.first = str_trim(ret.first);
    ret.second = str_trim(ret.second);
    return ret;
}

template <class SepT>
auto strSplitTrim(StrView text, SepT sep) noexcept {
    return str_split_trim(text, sep);
}



}  // namespace apl
