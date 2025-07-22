/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "str_format_type_util.hpp"

#include "../parse/number.hpp"

//
// Integer types. TODO: std::to_char
//

namespace apl {

template <class F, class T>
void str_format_type_int_prepare(ANumberFormatter &generator, F *pfmt, F fend, T arg) noexcept
{
    generator.append = true;

    auto& fmt = *pfmt;

    // check the formats
    if (fmt != fend && *fmt == ' ')
        generator.pos_sign = ' ', generator.showpos = true, fmt++;

    if (fmt != fend && *fmt == '+')
        generator.pos_sign = '+', generator.showpos = true, fmt++;

    if (fmt != fend && *fmt == '#')
        generator.showbase = true, ++fmt;

    if (fmt != fend && *fmt == '-')
        generator.adjustfield = ANumberFormatter::Adjust::Left;

    if (fmt != fend && *fmt == '0')
        generator.fill_char = '0', ++fmt;

    // Check for the field witdth
    if (fmt != fend && is_str_format_digit(*fmt)) {
        auto widthBegin = fmt++;
        while (fmt != fend && !str_format_type_spec_char(*fmt))
            ++fmt;
        // temporary
        //std::string width_str ;
        //do {
        //	width_str.append(1,*fmt), fmt++ ;
        //} while ( fmt != fend && !str_format_type_spec_char(*fmt) ) ;
        const auto& widthEnd = fmt;
        parse_num(widthBegin, widthEnd, generator.width);
        //generator.width = parse_int_it(widthBegin, widthEnd);

        //parse_num ( width_str, generator.width ) ;
    }

    // try to understand the type
    while (fmt != fend && !str_format_type_spec_char(*fmt))
        ++fmt;

    // here can be format modifier (the size of argument)
    while (fmt != fend && (*fmt=='h' || *fmt=='l' || *fmt=='L'))
        ++fmt;  // just skip it, since we determine the size automatically

    if (fmt != fend) {
        switch (*fmt) {
        case 'X': generator.radix = 16, generator.uppercase = true; break;
        case 'x': generator.radix = 16, generator.uppercase = false; break;
        default: break;
        }

        ++fmt;
    }
}


template <class S, class F, class T>
void str_format_type_int_signed(S &s, F *fmt, F fend, T arg) noexcept
{
    ANumberFormatter generator;
    str_format_type_int_prepare(generator, fmt, fend, arg);
    generator.gen(s, arg);
}

template <class S, class F, class T>
void str_format_type_int_unsigned(S &s, F *fmt, F fend, T arg) noexcept
{
    ANumberFormatter generator;
    str_format_type_int_prepare(generator, fmt, fend, arg);
    generator.gen_unsigned(s, arg);
}



/*
template <class fmt_T, class str_T, class int_T>
inline
void str_format_type_unsigned_int ( str_T &str, fmt_T &fmt, const fmt_T &fend, const int_T arg )
{
  ::apl::t_num_str generator ;
  generator.append = true ;


  // check the formats
  if ( fmt != fend && *fmt == ' ' )
    generator.pos_sign = ' ', generator.showpos = true, fmt++ ;

  if ( fmt != fend && *fmt == '+' )
    generator.pos_sign = '+', generator.showpos = true, fmt++ ;

  if ( fmt != fend && *fmt == '#' )
    generator.showbase = true, fmt++ ;

  if ( fmt != fend && *fmt == '-' )
    generator.adjustfield = generator.left ;

  if ( fmt != fend && *fmt == '0' )
    generator.fill_char = '0', fmt++ ;

  if ( fmt != fend && isdigit(*fmt) ) {
    // temporary
    std::string width_str ;
    do {
      width_str.append(1,*fmt), fmt++ ;
    } while ( fmt != fend && isdigit(*fmt) ) ;
    ::apl::parse_num(width_str,generator.width) ;
  }

  // try to understand the type
  while ( fmt != fend && !isalpha(*fmt) )
    ++fmt ;


  // here can be format modifier (the size of argument)
  while ( fmt != fend && (*fmt=='h' || *fmt=='l' || *fmt=='L') )
    ++fmt ;  // just skip it, since we determine the size automatically


  if ( fmt != fend ) {
    switch(*fmt) {
      case 'X' : generator.radix = 16, generator.uppercase = true ; break ;
      case 'x' : generator.radix = 16, generator.uppercase = false ; break ;
      default: break ;
    }

    ++fmt ;
  }


  generator.gen_unsigned(arg,str) ;
}
*/


// signed integers

template <class S, class F>
void str_format_type(S &s, F* fmt, F fend, short arg) noexcept {
    str_format_type_int_signed(s, fmt, fend, arg);
}


template <class S, class F>
void str_format_type(S &s, F* fmt, F fend, int arg) noexcept {
    str_format_type_int_signed(s, fmt, fend, arg);
}

template <class S, class F>
void str_format_type(S &s, F* fmt, F fend, long arg) noexcept {
    str_format_type_int_signed(s, fmt, fend, arg);
}

template <class S, class F>
void str_format_type(S &s, F* fmt, F fend, long long arg) noexcept {
    str_format_type_int_signed(s, fmt, fend, arg);
}



//
// Unsigned integers
//
//template <class S, class F>
//void str_format_type(S &s, F* fmt, F fend, unsigned char arg) noexcept {
//    str_format_type_int_unsigned(s, fmt, fend, arg);
//}

template <class S, class F>
void str_format_type(S &s, F* fmt, F fend, unsigned short arg) noexcept {
    str_format_type_int_unsigned(s, fmt, fend, arg);
}



template <class S, class F>
void str_format_type(S &s, F *fmt, F fend, unsigned int arg) noexcept {
    str_format_type_int_unsigned(s, fmt, fend, arg);
}

template <class S, class F>
void str_format_type(S &s, F *fmt, F fend, unsigned long arg) noexcept {
    str_format_type_int_unsigned(s, fmt, fend, arg);
}

template <class S, class F>
void str_format_type(S &s, F *fmt, F fend, unsigned long long arg) noexcept {
    str_format_type_int_unsigned(s, fmt, fend, arg);
}


}  // namespace apl

