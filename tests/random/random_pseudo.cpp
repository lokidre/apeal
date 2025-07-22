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

void body() {
    apl::Random rand;

    rand.setPseudoRandom(true, 20);


    con_.print("int:");
    for (int i = 0; i < 20; ++i) {
        con_.print(" %d", rand.generate());
    }
    con_.nl();


    con_.print("real:");
    for (int i = 0; i < 20; ++i) {
        con_.print(" %d", rand.generateReal<double>(-10000, 10000));
    }
    con_.nl();
}
