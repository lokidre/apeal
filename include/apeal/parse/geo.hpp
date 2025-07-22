/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../parse.hpp"
#include "../parse/vector.hpp"

#include <array>
#include <span>

namespace apl {


//
// TODO: move to the parse utilities and perhaps provide separator or search for the separator more efficiently
// or even better provide parse_vetor with the list of separators
//
template <class T, std::size_t N>
void parseGeoCoordinates(StrView str, std::span<T, N> coords, bool def)
{
    std::vector<String> vec;
    
    parse_vector(str, ',', vec);
    if (vec.size() == 1)
        parse_vector(str, 'x', vec);
    if (vec.size() == 1)
        parse_vector(str, '-', vec);

    auto count = coords.size();
    if (vec.size() != count && !def)
        throwError("Invalid coord count: %s", str);

    for (decltype(count) index{}; auto&& vs: vec) {
        if (index >= count)
            break;
        if (def) {
            parse(vs, &coords[index], coords[index]);
        } else {
            parse(vs, &coords[index]);
        }
        //coords[index] = def ? parse<T>(vs, coords[index]) : parse<T>(vs);
        ++index;
    }
}

// template <class T>
// void parsePoint(StrView s, Point<T>* p) {
//     parseGeoCoordinates<T, 2>(s, p->data(), false);
// }

template <class T>
void parse(StrView s, Point<T>* p) {
    // parsePoint(s, p);
    parseGeoCoordinates<T, 2>(s, p->span(), false);
}

template <class T>
void parse(StrView s, Point<T>* p, const Point<T>& def) {
    std::array r{def.x, def.y};
    parseGeoCoordinates<T, 2>(s, r, true);
    p->x = r[0];
    p->y = r[1];
}



// template <class T>
// void parseSize(StrView s, Size<T>* p) {
//     parseGeoCoordinates<T, 2>(s, p->span(), false);
// }

template <class T>
void parse(StrView s, Size<T>* p) {
    parseGeoCoordinates<T, 2>(s, p->span(), false);
}

template <class T>
void parse(StrView s, Size<T>* p, const Size<T>& def) {
    std::array r{def.w, def.h};
    parseGeoCoordinates<T, 2>(s, r, true);
    p->w = r[0];
    p->h = r[1];
}




// template <class T>
// Point<T> parsePoint(StrView s) {
//     std::array<T, 2> r{};
//     parseGeoCoordinates<T, r.size()>(s, r, false);
//     return {r[0], r[1]};
// }


// template <class T>
// Point<T> parsePoint(StrView s, typename Point<T>::Arg def) {
//     std::array<T, 2> r{def.x, def.y};
//     parseGeoCoordinates<T, r.size()>(s, r, true);
//     return {r[0], r[1]};
// }


// template <class T>
// Size<T> parseSize(StrView s) {
//     std::array<T, 2> r{};
//     parseGeoCoordinates<T, r.size()>(s, r, false);
//     return {r[0], r[1]};
// }


// template <class T>
// Size<T> parseSize(StrView str, typename Size<T>::Arg def) {
//     std::array<T, 2> r{def.w, def.h};
//     parseGeoCoordinates<T, r.size()>(str, r, true);
//     return {r[0], r[1]};
// }
//
//
// template <class T>
// Rect<T> parseRect(StrView str) {
//     std::array<T, 4> r{};
//     parseGeoCoordinates<T, r.size()>(str, r, false);
//     return {r[0], r[1], r[2], r[3]};
// }
//
//
// template <class T>
// Rect<T> parseRect(StrView str, typename Rect<T>::Arg def) {
//     std::array<T, 4> r{def.x, def.y, def.w, def.h};
//     parseGeoCoordinates<T, r.size()>(str, r, true);
//     return {r[0], r[1], r[2], r[3]};
// }


// // Point with the underlying type
// template <typename U>
// using PointU = Point<typename U::value_type>;
//
// template <class T, typename = std::enable_if_t<std::is_same_v<T, PointU<T>>>>
// T parse(StrView s) { return parsePoint<typename T::T>(s); }
//
// template <class T, typename = std::enable_if_t<std::is_same_v<T, PointU<T>>>>
// T parse(StrView s, typename T::Arg def) {
//     return parsePoint<typename T::T>(s, def);
// }




// Size
// template <typename U>
// using SizeU = Size<typename U::value_type>;
//
//
// template <class T, typename = std::enable_if_t<std::is_same_v<T, SizeU<T>>>>
// T parse(StrView s) {  return parseSize<typename T::T>(s);  }
//
// template <class T, std::enable_if_t<std::is_same_v<T, Size<typename T::T>>, int> = 0>
// T parse(StrView s, typename T::Arg def) { return parseSize<typename T::T>(s, def); }
//
//
// // Rect
// template <class T, typename std::enable_if_t<std::is_same_v<T, Rect<typename T::T>>, int> = 0>
// T parse(std::string_view s) { return parseRect<typename T::T>(s);  }
//
// template <class T, typename std::enable_if_t<std::is_same_v<T, Rect<typename T::T>>, int> = 0>
// T parse(StrView s, typename T::Arg def) { return parseRect<typename T::T>(s, def); }


}  // namespace apl


