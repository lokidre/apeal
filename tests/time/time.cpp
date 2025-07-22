/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/time.hpp>

void body()
{
    auto present_time = time(nullptr);

    auto t = apl::fromUnixTime(present_time);

    auto formattedDate = formatDate(t);

    con_("TIME: %s", formattedDate);
}
