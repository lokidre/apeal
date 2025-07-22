/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "geo.hpp"
#include "../val_range.hpp"

namespace apl {

template <typename U>
using ValRangetU = ValRange<typename U::value_type>;


template <class T>
ValRange<T> parseRange(StrView s) {
    std::array<T, 2> r{};
    parseGeoCoordinates<T, r.size()>(s, r, false);
    return {r[0], r[1]};
}


template <class T>
ValRange<T> parseRange(StrView s, const ValRange<T>& def) noexcept {
    std::array<T, 2> r{def.x, def.y};
    parseGeoCoordinates<T, r.size()>(s, r, true);
    return {r[0], r[1]};
}


template <class Rng, std::enable_if_t<std::is_same_v<Rng, ValRangetU<Rng>>, int> = 0>
Rng parse(StrView s) {
    return parseRange<typename Rng::T>(s);
}

template <class Rng, std::enable_if_t<std::is_same_v<Rng, ValRangetU<Rng>>, int> = 0>
Rng parse(StrView s, const Rng& def) {
    return parseRange<typename Rng::T>(s, def);
}

}  // namespace apl
