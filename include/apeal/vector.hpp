/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <vector>
#include <array>
#include <memory>


//
// Overload some operators for std::vector and std::array for convenience
//
// -v - return (int)size()
// ++v - adds the element and returns reference to it (emplace_back)


//template <class T, int N>
//constexpr int operator -(const std::array<T,N> &v) {  return (int)v.size() ;  }
//
//template <class T>
//constexpr int operator -(const std::vector<T> &v) {  return (int)v.size() ;  }


//template <class T, class ...Args>
//inline T& operator ++(std::vector<T> &v, Args&& ...args) {  return v.emplace_back(std::forward<Args>(args)...) ;  }

