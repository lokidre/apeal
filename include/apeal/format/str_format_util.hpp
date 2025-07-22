/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <utility>

#include "../string.hpp"
#include "../index.hpp"

//
// Formatting utilities
//

#include "str_format_type_util.hpp"


namespace apl {

// TODO: these functions need a good rewrite

template <class C>
constexpr C format_hex_digit(int digit, C a = 'A') noexcept
{
    return (((digit&0xF) > 9) ? (digit&0xf) + a - 10 : (digit&0xf) + '0');
}


template <class dig_type, class char_type>
constexpr void dig_str_bin(const dig_type &d, char_type &c) noexcept
{
    c = (char_type)(d ? '1' : '0');
}

template <class dig_type, class char_type>
constexpr void dig_str_dec(const dig_type &d, char_type &c) noexcept
{
    c = (char_type)(d+'0');
}

constexpr int gener_dig_hex(int d) noexcept {
    return ((d<10) ? d+'0' : d+'a'-10);
}


template <class dig_type, class char_type>
constexpr void dig_str_hex(dig_type d, char_type &c) noexcept
{
    c = (char_type)((d<10) ? d+'0' : d+'a'-10);
}

template <class dig_type, class char_type>
constexpr void dig_str_HEX(dig_type d, char_type &c) noexcept
{
    c = (char_type)((d<10) ? d+'0' : d+'A'-10);
}


template <class T, class Char>
constexpr void dig_str(T d, Char &c, int radix, bool uppercase = false) noexcept
{
    if (radix == 2)
        dig_str_bin(d, c);
    else if (radix == 8 || radix == 10)
        dig_str_dec(d, c);
    else if (radix == 16)
        if (uppercase)
            dig_str_HEX(d, c);
        else
            dig_str_hex(d, c);
    else {
        // needed
    }
}


// unformatted, unflagged conversion of integer number into string

template <class num_type, class string_type>
void int_str(const num_type &num, string_type &str, int radix = 10) noexcept
{
    str.clear();

    typename string_type::value_type c;
    num_type q, r;
    num_type n = num<0 ? -num : num;

    // generate the number in reverse order
    while (n > 0) {
        // calculate quotient/remainder (better as one operation)
        q = n/radix, r = n%radix;
        dig_str(r, c, radix);
        str.append(1, c);
        n = q;
    }

    if (str.length() == 0)
        str.append(1, '0');

    if (num < 0)
        str.append(1, '-');

    // reverse the string
    size_t cnt = str.size()/2;
    for (size_t i = 0, j = str.size()-1; i < cnt; i++, j--)
        c = str[i], str[i] = str[j], str[j] = c;
}



//
// Number formattter with lots of options and flags
// TODO: old code, needs rewrite
//
struct ANumberFormatter {
    Index precision{-1};
    Index width{-1};
    int radix{10};

    bool showbase{};
    bool showpos{};
    bool uppercase{};

    enum class FloatFmt {
        Fixed,   // insert floating point numbers without exponent field
        Scientific  // always show exponent
    };
    FloatFmt floatfield{FloatFmt::Fixed};


    enum class Adjust{Internal, Left, Right};
    
    Adjust adjustfield{Adjust::Right};

    char fill_char{' '};
    char pos_sign{'+'};

    bool append{};

    void clear()
    {
        *this = {};
    }

    template <class S, class T>
    void gen_unsigned(S &str, T n)
    {
        using Char = typename S::value_type;
        using num_type = T;

        using std::ssize;

        Char c{};
        Index str_begin{};

        if (append)
            str_begin = ssize(str);
        else
            str.clear();


        // generate the number in reverse order
        while (n > 0) {
            // calculate quotient/remainder (better as one operation)
            auto q = T(n/radix);
            auto r = T(n%radix);
            dig_str(r, c, radix, uppercase);
            str += c;
            n = q;
        }

        if (ssize(str) - str_begin == 0)
            str += '0';

        // append the base (if required)
        if (showbase) {
            switch (radix) {
            case  2: str += uppercase ? 'B' : 'b'; break;
            case  8: str += uppercase ? 'O' : 'o'; break;
            case 10: str += uppercase ? 'D' : 'd'; break;
            case 16: str += uppercase ? 'X' : 'x'; break;
            default: str += uppercase ? 'U' : 'u'; break;
            }
            str += '0';
        }

        // append the sign
        if (showpos)
            str += pos_sign;

        // work with field width
        if (width > ssize(str) - str_begin) {
            Char sign{};
            if (fill_char != ' ') {
                sign = str[ssize(str) - 1];
                if (sign != '-' && sign != pos_sign)
                    sign = 0;
            }

            // fill up with char
            if (showbase || sign) {
                Char buffer[3];
                int i, cnt = 0;
                if (sign) {
                    buffer[cnt] = str[str.size() - str_begin - cnt - 1];
                    ++cnt;
                }

                if (showbase) {
                    buffer[cnt] = str[str.size() - str_begin - cnt - 1];
                    ++cnt;
                    buffer[cnt] = str[str.size() - str_begin - cnt - 1];
                    ++cnt;
                }

                for (i = 0; i < cnt; i++)
                    str[str.size()-str_begin-i-1] = fill_char;
                str.append(width - (ssize(str)-str_begin) - cnt, fill_char);
                for (i = 0; i < cnt; i++)
                    str.append(1, buffer[cnt-1-i]);
            } else {
                str.append(width - (ssize(str)-str_begin), fill_char);
            }
        }


        // reverse the string
        // needed to do iterators instead of indexes
        Index cnt = (str.size() - str_begin)/2;
        for (Index i = str_begin, j = str.size() - 1; i < str_begin + cnt; ++i, --j)
            c = str[i], str[i] = str[j], str[j] = c;
    }

    template <class S, class T>
    void gen(S &str, const T &num)
    {
        using char_type = typename S::value_type;
        using num_type = T;

        char_type c = ' ';
        num_type q, r;
        //auto n = std::abs(num); //num < 0 ? -num : num;

        auto n = num < 0 ? static_cast<std::make_unsigned<T>::type>(-num) : num;
        size_t str_begin = 0;



        if (append)
            str_begin = str.size();
        else
            str.clear();


        // generate the number in reverse order
        while (n > 0) {
            // calculate quotient/remainder (better as one operation)
            q = n/(num_type)radix, r = n%(num_type)radix;
            dig_str(r, c, radix, uppercase);
            str.append(1, c);
            n = q;
        }

        if (str.length() - str_begin == 0)
            str.append(1, '0');

        // append the base (if required)
        if (showbase) {
            switch (radix) {
            case  2: str.append(1, uppercase ? 'B' : 'b'); break;
            case  8: str.append(1, uppercase ? 'O' : 'o'); break;
            case 10: str.append(1, uppercase ? 'D' : 'd'); break;
            case 16: str.append(1, uppercase ? 'X' : 'x'); break;
            default: str.append(1, uppercase ? 'U' : 'u'); break;
            }
            str.append(1, '0');
        }

        // append the sign
        if (num < 0)
            str.append(1, '-');
        else if (showpos)
            str.append(1, pos_sign);

        // work with field width
        if (width >(int)(str.size()-str_begin)) {
            char_type sign = 0;
            if (fill_char != ' ') {
                sign = str[str.size()-1];
                if (sign != '-' && sign != pos_sign)
                    sign = 0;
            }

            // fill up with char
            if (showbase || sign) {
                char_type buffer[3];
                int i, cnt = 0;
                if (sign)
                    buffer[cnt] = str[str.size()-str_begin-cnt-1], cnt++;
                if (showbase)
                    buffer[cnt] = str[str.size()-str_begin-cnt-1], cnt++, buffer[cnt] = str[str.size()-str_begin-cnt-1], cnt++;
                for (i = 0; i < cnt; i++)
                    str[str.size()-str_begin-i-1] = fill_char;
                str.append(width-(str.size()-str_begin)-cnt, fill_char);
                for (i = 0; i < cnt; i++)
                    str.append(1, buffer[cnt-1-i]);
            } else {
                str.append(width-(str.size()-str_begin), fill_char);
            }
        }


        // reverse the string
        // needed to do iterators instead of indexes
        std::size_t cnt = (str.size() - str_begin)/2;
        for (std::size_t i = str_begin, j = str.size() - 1; i < str_begin + cnt; i++, j--)
            c = str[i], str[i] = str[j], str[j] = c;
    }
};

}  // namespace apl

