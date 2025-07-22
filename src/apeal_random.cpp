/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/random.hpp>

#include <ctime>


#if APEAL_OS_MAC
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>
#endif

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#endif



namespace apl {

void RandomSeedGenerator::collectNoise() noexcept
{
    // timer is a good source for noise
    // first read the system ticks

    // common system timer
    noise(std::time(0));
    noise(clock());

    // read timer tick
#if APEAL_OS_WINDOWS
    {
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        noise(&t, sizeof(t));
    }
#endif

#if __EMSCRIPTEN__
    {
        noise(emscripten_random());
    }
#endif

    //
    // High precision timers
    //
#if APEAL_OS_MAC
    noise(mach_absolute_time());
#endif
}


}  // namespace apl



