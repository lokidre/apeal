/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/uid.hpp>

#include <unordered_set>

int numTests = 1000;

void body() {
    using namespace apl;

    std::unordered_set<uint64_t> prev;

    for (int i = 0; i < numTests; i++) {
        auto uid = generateUid64();

        printf("%lx\n", uid);

        if (prev.contains(uid)) {
            throw std::runtime_error("Error:Duplicate!");
        }

        prev.insert(uid);
    }
}
