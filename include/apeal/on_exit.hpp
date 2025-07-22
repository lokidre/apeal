/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <functional>

#if HEADER_OnExit__

OnExit
---

A very simple class which calls supplied function when goes out of scope


Usage:

    OnExit onExit([&]{
        doSomething();
    });

#endif


namespace apl {

struct OnExit {
    OnExit(std::function<void()> i): f{i}{}
    ~OnExit() {  f(); }
    std::function<void()> f;
};

}  // namespace apl
