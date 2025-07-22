/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>
#include <string>

#include <atomic>

#include <apeal/event.hpp>
#include <apeal/thread.hpp>
#include <apeal/time.hpp>

using namespace apl;
using namespace std;

AEvent event_;
atomic_bool quit{false};


void *event_thread_func(void *) {
    while (!quit) {
        try {
            event_.wait();
            cout << "event" << endl;
        }
        catch (...) {
            cerr << "exception!" << endl;
        }
    }
    return 0;
}


void body() {
    string s;

    event_.create();

    cout << "'quit' to quit or anything to pulse..." << endl;

        Thread th(event_thread_func);

        while (!quit) {
            cin >> s;
            if (s == "quit")
                quit = true;
            event_.pulse();
        }

}

