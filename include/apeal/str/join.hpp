/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../string.hpp"
#include "../concepts.hpp"


namespace apl {


template <template <typename...> class ContT, class ValT, typename... ArgsT, class SepT, class PredT>
String str_join(const ContT<ValT, ArgsT...>& container, const SepT& sep, PredT pred) {
    String s;
    s.reserve(container.size() * 2);

    for (auto& el: container) {
        if (!s.empty())
            s += sep;
        s += pred(el);
    }

    return s;
}

template <template <typename...> class ContT, typename... ArgsT, TextParamT ValT, class SepT>
String str_join(const ContT<ValT, ArgsT...>& container, const SepT& sep) {
    return str_join(container, sep, [](auto& el){ return el;} );
}

template <template <typename...> class ContT, typename... ArgsT, std::integral ValT, class SepT>
String str_join(const ContT<ValT, ArgsT...>& container, const SepT& sep) {
    return str_join(container, sep, [](auto& el){ return std::to_string(el); } );
}

template <template <typename...> class ContT, typename... ArgsT, std::floating_point ValT, class SepT>
String str_join(const ContT<ValT, ArgsT...>& container, const SepT& sep) {
    return str_join(container, sep, [](auto& el){ return std::to_string(el); } );
}



}  // namespace apl


