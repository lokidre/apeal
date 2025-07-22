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


//
// Accepts dictionary container with value of std::pair<key,value> will return value converted to R
//
template <class I, class D, class R>
constexpr bool parse_selection_it_core ( I begin, I end, const D &dict, R &res )  noexcept
{
    // TODO: Skip white space

    for ( auto &d: dict ) {
        if ( d.first.compare ( 0, begin, end - begin ) == 0 ) {
            res = R(d.second) ;
            return true ;
        }
    }

    return false ;
}

}  // namespace apl
