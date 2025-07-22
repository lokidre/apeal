/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <test_main.h>

#include <apeal/geo.hpp>

template <class A, class B, class C>
void printBinary(A a, B b, C c, apl::StrView op) {
    con_("%g %s %g --> %g", a, op, b, c);
}

