/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/singleton.hpp>

struct Foo: apl::Singleton<Foo> {
    int i = 20;
};

//struct Bar: apl::Singleton<Bar> {
//    Bar(int v): i{v} {}
//    int i = 10;
//};


void body()
{
    auto foo = Foo::instance();

    con_("%d", foo->i);

    //auto bar = Bar::instance(5);

    //con_(bar->i);
}
