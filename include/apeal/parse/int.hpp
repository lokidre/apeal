/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

/*
        Parsing/Building integers
*/

#include "parse_util.hpp"
#include "../concepts.hpp"

namespace apl {


template <class I, class T>
bool parse_uint_body_it(I curr, I end, T& num, int userRadix = 0) noexcept {
    auto radix = (T)userRadix; // default radix

    num = 0;

    // skip the spaces at the beginning
    for (; curr != end && parse_is_space(*curr); ++curr)
        ;

    if (curr == end)
        return false;


    // check for the sign
    if (*curr == '+')
        curr++;
    else
        // check for the radix definition
        if (radix == 0 && *curr == '0') {
        ++curr;

        if (curr == end)
            return true;

        if (!isdigit(*curr)) {
            if (*curr == 'X' || *curr == 'x')
                radix = 16;
            else if (*curr == 'B' || *curr == 'b')
                radix = 10;
            else if (*curr == 'O' || *curr == 'o')
                radix = 8;
            else
                return false;

            ++curr;

            if (curr == end)
                return false;
        }
    }

    // if ( user_radix != 0 )
    //  radix = user_radix ;

    if (radix == 0)
        radix = 10;

    bool was_digit = false;
    T digit_value;

    for (; curr != end; curr++) {

        // check for the space
        if (parse_is_space(*curr)) { // isspace
            // the rest of the characters must be space too
            for (++curr; curr != end; ++curr)
                if (!parse_is_space(*curr))
                    return false;
            break; // exit - the number has finished
        }

        // check the digits against the given radix
        // needed if ( radix == 16 )

        if (radix == 16) {
            if (!(*curr >= '0' && *curr <= '9') &&
                !((*curr >= 'a' && *curr <= 'f') || (*curr >= 'A' && *curr <= 'F')))
                return false;

            if (*curr >= 'a' && *curr <= 'f')
                digit_value = T(*curr - 'a' + 10);
            else if (*curr >= 'A' && *curr <= 'F')
                digit_value = T(*curr - 'A' + 10);
            else
                digit_value = T(*curr - '0');

        } else {
            if (*curr < '0' || *curr > '9')
                return false;
            digit_value = T(*curr - '0');
        }

        num = num * radix + digit_value;

        was_digit = true;
    }

    // if there were no digits, exit with error
    if (!was_digit)
        return false;

    return true;
}

template <class I, class T>
bool parse_int_body_it(I c, I end, T& num, int radix = 0) noexcept {
    if (c == end)
        return false;

    bool neg = false;
    if (*c == '-') {
        neg = true;
        ++c;
    }
    bool result = parse_uint_body_it(c, end, num, radix);
    if (neg)
        num = -num;
    return result;
}


template <class I, class T>
bool parse_int_it(I begin, I end, T& v) noexcept {
    return parse_int_body_it(begin, end, v);
}

// template <class T>
// bool parse_int(const char *s, T &v) noexcept { return s ? parse_int_body_it(s, s + strlen(s), v)
// : false; }

template <class T>
bool parse_int(StrView s, T& v) noexcept {
    return parse_int_body_it(s.begin(), s.end(), v);
}

// template <class T>
// bool parse_int(char *s, T &v) noexcept { return s ? parse_int_body_it(s, s + strlen(s), v) :
// false; }


template <class I, class T>
bool parse_uint_it(I begin, I end, T& v) noexcept {
    return parse_uint_body_it(begin, end, v);
}

// template <class T>
// bool parse_uint(const char *s, T &v) noexcept { return s ? parse_uint_body_it(s, s + strlen(s),
// v) : false; }

template <class T>
bool parse_uint(StrView s, T& v) noexcept {
    return parse_uint_body_it(s.begin(), s.end(), v);
}

// template <class T>
// bool parse_uint(char *s, T &v) noexcept { return s ? parse_uint_body_it(s, s + strlen(s), v) :
// false; }


template <class I, class T>
bool parse_int_radix_it(I begin, I end, T& v, int radix) noexcept {
    return parse_int_body_it(begin, end, v, radix);
}

template <class T>
bool parse_int_radix(StrView s, T& v, int radix) noexcept {
    return parse_int_body_it(s.begin(), s.end(), v, radix);
}


template <class I, class T>
bool parse_uint_radix_it(I begin, I end, T& v, int radix) noexcept {
    return parse_uint_body_it(begin, end, v, radix);
}

template <class T>
bool parse_uint_radix(StrView s, T& v, int radix) noexcept {
    return parse_uint_body_it(s.begin(), s.end(), v, radix);
}


template <class T>
T parseIntGeneric(StrView s, int radix = 10) {
    T v;
    verify(parse_int_radix(s, v, radix), "Invalid integer value: %s", s);
    return v;
}

template <class T, class I>
T parseIntGenericIt(I begin, I end, int radix) {
    T v;
    auto res = parse_int_radix_it(begin, end, v, radix);
    verify(res, "Invalid integer value: %s", StrView{begin, end});
    return v;
}


// Doesn't throw error, returns default value
template <class T>
T parseIntDefGeneric(StrView s, T def, int radix = 10) noexcept {
    T v;
    if (!parse_int_radix(s, v, radix))
        return def;
    return v;
}

template <class T, class I>
T parseIntDefGenericIt(I begin, I end, T def, int radix = 10) noexcept {
    T v;
    if (!parse_int_radix_it(begin, end, v, radix))
        return def;
    return v;
}


template <class T>
T parseUIntGeneric(StrView s, int radix = 10) {
    T v;
    verify(parse_uint_radix(s, v, radix), "Invalid unsigned integer value: %s", s);
    return v;
}

template <class T, class I>
T parseUIntGenericIt(I begin, I end, int radix = 10) {
    T v;
    verify(parse_uint_radix_it(begin, end, v, radix), "Invalid unsigned integer value: %s",
           String{begin, end});
    return v;
}


// Doesn't throw error, returns default value
template <class T>
T parseUIntDefGeneric(StrView s, T def, int radix = 10) noexcept {
    T v;
    if (!parse_uint_radix(s, v, radix))
        return def;
    return v;
}

template <class T, class I>
T parseUIntDefGenericIt(I begin, I end, T def, int radix = 10) noexcept {
    T v;
    if (!parse_uint_radix(begin, end, v, radix))
        return def;
    return v;
}


// template <class S>
inline int parseInt(StrView s) { return parseIntGeneric<int>(s); }

template <class I>
int parseIntIt(I begin, I end) {
    return parseIntGenericIt<int>(begin, end);
}


// template <class S>
inline int parseInt(StrView s, int def) noexcept { return parseIntDefGeneric(s, def); }

template <class I>
int parseIntIt(I begin, I end, int def) noexcept {
    return parseIntDefGenericIt(begin, end, def);
}


// template <class S>
inline unsigned int parseUInt(StrView s) { return parseUIntGeneric<unsigned int>(s); }

template <class I>
unsigned int parseUIntIt(I begin, I end) {
    return parseUIntGenericIt<unsigned int>(begin, end);
}


// template <class S>
inline unsigned int parseUInt(StrView s, unsigned int def) noexcept {
    return parseUIntDefGeneric(s, def);
}

template <class I>
unsigned int parseUIntIt(I begin, I end, unsigned int def) noexcept {
    return parseUIntDefGeneric(begin, end, def);
}


// template <class S>
inline unsigned int parseUIntHex(StrView s) { return parseUIntGeneric<unsigned int>(s, 16); }

// Accepts iterators
template <class I>
unsigned int parseUIntHexIt(I begin, I end) {
    return parseUIntGenericIt<unsigned int>(begin, end, 16);
}


// template <class S>
inline unsigned int parseUIntHex(StrView s, unsigned int def) noexcept {
    return parseUIntDefGeneric<unsigned int>(s, def, 16);
}

// Accepts iterators
template <class I>
unsigned int parseUIntHexIt(I begin, I end, unsigned int def) noexcept {
    return parseUIntDefGenericIt<int>(begin, end, def, 16);
}



void parse(TextParamT auto s, std::signed_integral auto* val) {
    *val = parseIntGeneric<std::remove_reference_t<decltype(*val)>>(s);
}

void parse(TextParamT auto s, std::signed_integral auto* val, std::signed_integral auto def) {
    *val = parseIntDefGeneric<std::remove_reference_t<decltype(*val)>>(s, def);
}

void parse(TextParamT auto s, std::unsigned_integral auto* val) {
    *val = parseUIntGeneric<std::remove_reference_t<decltype(*val)>>(s);
}

void parse(TextParamT auto s, std::unsigned_integral auto* val, std::unsigned_integral auto def) {
    *val = parseUIntDefGeneric<std::remove_reference_t<decltype(*val)>>(s, def);
}



} // namespace apl
