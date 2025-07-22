/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/zlib.hpp>

#include <apeal/verify.hpp>

void body()
{
    using namespace apl;

    ZLib zlib;

    verify(cmdlineVector_.size() >= 2, "usage: gzip-inflate <gzip-file> <output-file>");

    zlib.inflateGzipFile(cmdlineVector_[0], cmdlineVector_[1]);
}
