/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

//
// Definition of core fundamental types
//

#include "def.hpp"

#include <cstdint>
#include <vector>
#include <type_traits>
#include <span>


#include "index.hpp"


namespace apl {

template <class T>
using VectorCont = std::vector<T>;


//
// Function parameters helpers
//

//
// Pass by value if sizeof(T) <= sizeof(void *) * 2
// Otherwise pass by const reference
//

template <class T>
using IntegParam =
    typename std::conditional<sizeof(T) <= sizeof(void*) * 2, const T, const T&>::type;


template <class T, int N, class U>
using StructParamN =
    typename std::conditional<sizeof(T) * N <= sizeof(void*) * 2, const U, const U&>::type;


} // namespace apl
