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



template <class string_type, class C, class D>
string_type &str_replace ( string_type &s, const C &a, const D &b )
{
    typename string_type::size_type idx ;

    while ( (idx=s.find(a)) != s.npos ) {
        s.replace ( idx, a.size(), b ) ;
    }

    return s ;
}

template <class string_type, class D>
string_type &str_replace ( string_type &s, const char a, const D &b )
{
    typename string_type::size_type idx ;

    while ( (idx=s.find(a)) != s.npos ) {
        s.replace ( idx, 1, b ) ;
    }

    return s ;
}


template <class S, class D>
S &str_replace ( S &s, const char *a, const D &b )
{
    typename S::size_type idx ;

    while ( (idx=s.find(a)) != s.npos ) {
        s.replace ( idx, strlen(a), b ) ;
    }

    return s ;
}

template <class S>
S &str_replace ( S &s, const char *a, const char b )
{
    typename S::size_type idx ;

    while ( (idx=s.find(a)) != s.npos ) {
        s.replace ( idx, strlen(a), 1, b ) ;
    }

    return s ;
}




/*
// note!! inefficient
template <class S, class C, class U>
S str_replace ( const S &s, const C &a, const U &b )
{
    S t ;

    for ( S::const_iterator c = s.begin(), c_end = s.end() ; c != c_end ; ++c ) {
        if ( *c == a )
            t += b ;
        else
            t += *c ;
    }

    return t ;
}
*/

}
