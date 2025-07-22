/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../parse/parse_util.hpp"

#include <cstring>

//
// TODO: clean up.
//

namespace apl {

/*
//
// parses single character digit into integer value
// returns -1 if the digit could not be converted
//
template <class value_type>
inline
int parse_dig_hex ( const value_type &c )
{
  if ( c >= 'a' && c <= 'f' )
    return int(c - 'a' + 10) ;

  if ( c >= 'A' && c <= 'F' )
    return int(c - 'A' + 10) ;

  if ( c >= '0' && c <= '9' )
    return int(c - '0') ;

  return -1 ;
}
*/



template <class iterator_type, class number_type>
bool parse_num_basic ( const iterator_type &begin, const iterator_type &end, number_type &num, int user_radix=0 )
{
  number_type radix = (number_type)user_radix ;  // default radix
  iterator_type curr ;

  num = 0 ;
  // bool neg = false ;


  // skip the spaces at the beginning
  for ( curr = begin ; curr != end && isspace(*curr) ; curr++ )
    ;

  if ( curr == end )
    return false ;


  // check for the sign
  if ( *curr == '+' )
    curr++ ;
  else // if ( *curr == '-' )
    // neg = true ;

  // check for the radix definition
  if ( radix == 0 && *curr == '0' ) {
    curr++ ;

    if ( curr == end )
      return true ;

    if ( !isdigit(*curr) ) {
      if ( *curr=='X' || *curr=='x' )
        radix = 16 ;
      else if ( *curr=='B' || *curr=='b' )
        radix = 10 ;
      else if ( *curr=='O' || *curr=='o' )
        radix = 8 ;
      else
        return false ;

      curr++ ;

      if ( curr == end )
        return false ;
    }
  }

  //if ( user_radix != 0 )
  //  radix = user_radix ;

  if ( radix == 0 )
    radix = 10 ;

  bool was_digit = false ;
  number_type digit_value ;

  for ( ; curr != end ; curr++ ) {

    // check for the space
    if ( isspace(*curr) ) {
      // the rest of the characters must be space too
      for ( curr++ ; curr != end ; curr++ )
        if ( !isspace(*curr) )
          return false ;
      break ;  // exit - the number has finished
    }

    // check the digits against the given radix
    // needed if ( radix == 16 )

    if ( radix == 16 ) {
      if ( !isdigit(*curr) && !((*curr>='a'&&*curr<='f')||(*curr>='A'&&*curr<='F')) )
        return false ;
      if ( *curr >= 'a' && *curr <= 'f' )
        digit_value = (number_type)(*curr - 'a' + 10) ;
      else if ( *curr >= 'A' && *curr <= 'F' )
        digit_value = (number_type)(*curr - 'A' + 10) ;
      else
        digit_value = (number_type)(*curr - '0') ;
    } else {
      if ( !isdigit(*curr) )
        return false ;
      digit_value = (number_type)(*curr - '0') ;
    }

    num = num * radix + digit_value ;

    was_digit = true ;

  }

  // if there were no digits, exit with error
  if ( !was_digit )
    return false ;

  // needed negative numbers, maybe by partial specialization
  //if ( neg )
  //  num = number_type(-num) ;

  return true ;
}


template <class input_iterator_type, class number_type>
bool parse_num ( const input_iterator_type &begin, const input_iterator_type &end, number_type &n )
{
  return parse_num_basic(begin,end,n) ;
}


template <class number_type>
bool parse_num ( const char *s, number_type &n )
{
  return parse_num_basic(s,s+ std::strlen(s),n) ;
}

template <class number_type>
bool parse_num ( char *s, number_type &n )
{
  return parse_num_basic(s,s+strlen(s),n) ;
}


template <class input_container_type, class number_type>
bool parse_num ( const input_container_type &s, number_type &n )
{
  return parse_num_basic(s.begin(),s.end(),n) ;
}


template <class input_iterator_type, class number_type>
bool parse_num_radix ( const input_iterator_type &begin, const input_iterator_type &end, number_type &n, const int &radix )
{
  return parse_num_basic(begin,end,n,radix) ;
}


template <class number_type>
bool parse_num_radix ( const char *s, number_type &n, const int &radix )
{
  return parse_num_basic(s,s+strlen(s),n,radix) ;
}

template <class number_type>
bool parse_num_radix ( char *s, number_type &n, const int &radix )
{
  return parse_num_basic(s,s+strlen(s),n,radix) ;
}


template <class input_container_type, class number_type>
bool parse_num_radix ( const input_container_type &s, number_type &n, const int &radix )
{
  return parse_num_basic(s.begin(),s.end(),n,radix) ;
}

}  // namespace apl
