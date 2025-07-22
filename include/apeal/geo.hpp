/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#if 0

Geometric Primitives
---


#endif

#include "deprecated.hpp"
#include "util.hpp"

#include <algorithm>
#include <cmath>

#include "geo/point.hpp"
#include "geo/point3d.hpp"
#include "geo/rect.hpp"
#include "geo/margins.hpp"


namespace apl {

//
// find intersection of two segments
// returns positive if intersection, negitive - if zero intersection
//
template <class T>
T intersect(const T& a_min, const T& a_max, const T& b_min, const T& b_max) {
    if (a_min > b_min) {
        if (a_max > b_max) {
            return b_max - a_min;
        } else {
            return a_max - a_min;
        }
    } else {
        if (a_max < b_max) {
            return a_max - b_min;
        } else {
            return b_max - b_min;
        }
    }
}


//
// returns minmax values of an array of coordinates
//
template <class iterator, class T>
void minmax_xy(iterator begin, iterator end, T& minx, T& maxx, T& miny, T& maxy) {
    if (begin == end)
        return;
    minx = maxx = begin->x, miny = maxy = begin->y;
    for (++begin; begin != end; ++begin) {
        if (begin->x < minx)
            minx = begin->x;
        else if (begin->x > maxx)
            maxx = begin->x;

        if (begin->y < miny)
            miny = begin->y;
        else if (begin->y > maxy)
            maxy = begin->y;
    }
}

template <class iterator, class T>
void minmax_x(iterator begin, iterator end, T& minx, T& maxx) {
    if (begin == end)
        return;
    minx = maxx = begin->x;
    for (++begin; begin != end; ++begin) {
        if (begin->x < minx)
            minx = begin->x;
        else if (begin->x > maxx)
            maxx = begin->x;
    }
}


template <class iterator, class T>
void minmax_y(iterator begin, iterator end, T& miny, T& maxy) {
    if (begin == end)
        return;
    miny = maxy = begin->y;
    for (++begin; begin != end; ++begin) {
        if (begin->y < miny)
            miny = begin->y;
        else if (begin->y > maxy)
            maxy = begin->y;
    }
}

} // namespace apl
