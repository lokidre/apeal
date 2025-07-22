/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/parse/yaml.hpp>

void body()
{
    auto filename = requireCmdlineArg();

    auto yaml = apl::loadYaml(filename);

    auto str = apl::generateYaml(yaml);

    std::cout << str;
}
