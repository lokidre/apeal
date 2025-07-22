/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../common/test_main.h"

#include <apeal/random.hpp>

#include <algorithm>
#include <array>
#include <map>

void body() {
    using namespace apl;

    Random rand;

    //auto mn = std::mt19937_64::min();
    //auto mx = std::mt19937_64::max();


    //auto mrn = Random::min();
    //auto mrx = Random::max();

    int tests = 10000;
    constexpr int a = 0, b = 20;
    int hlength = 80;

    Random::UniformInt dis(a, b);

    std::array<int, b - a + 1> hist{};

    for (int i = 0; i < tests; ++i) {
        ++hist[dis(rand)];
    }

    auto hmax = *std::max_element(hist.begin(), hist.end());

    //std::for_each(hist.begin(), hist.end(), [&](auto& e) { e = e * hlength / hmax; });

    std::ranges::for_each(hist, [&](auto& e) { e = e * hlength / hmax; });

    int val = a;
    for (auto& e: hist) {
        con_.print("%2f", val++);
        con_.tab();
        con_.print("= %.14f", e);
        con_.nl();
    }
}
