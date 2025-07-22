/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "string.hpp"

#include <concepts>
#include <iterator>


namespace apl {

template <typename T>
//concept TextParamT = std::same_as<T, String> || std::same_as<T, StrView>;
concept TextParamT = std::convertible_to<T, StrView>;


// Define a concept for an iterable container
template <typename T>
concept Iterable = requires(T container) {
    { std::begin(container) } -> std::input_iterator;
    { std::end(container) } -> std::sentinel_for<decltype(std::begin(container))>;
};


}  // namespace apl

