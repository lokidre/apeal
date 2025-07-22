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
#include "../../common/test_main.h"

#include <apeal/mutex.hpp>
#include <apeal/time.hpp>

#if APEAL_OS_UNIX
#include <pthread.h>
#endif

apl::AMutex consoleMutex;


void *threadFunc1(void *p)
{
    for(;;) {
        {
            apl::ALock<apl::AMutex> lock(consoleMutex);
            std::cout << "thread 1" << std::endl;
        }
        apl::sleep(1000);
    }
    return 0;
}

void *threadFunc2(void *)
{
    for(;;) {
        {
            apl::ALock<apl::AMutex> lock(consoleMutex);
            std::cout << "thread 2" << std::endl;
        }
        apl::sleep(2500);
    }
    return 0;
}

void body()
{
    using namespace apl;

    int ret_code = 1;

#if APEAL_OS_UNIX
    consoleMutex.create();

    std::cout << "type 'quit' to quit..." << std::endl;
    std::string s;

    pthread_t thread_handle_1, thread_handle_2;

        int error_code = 0 ;

        error_code = ::pthread_create(&thread_handle_1, 0, threadFunc1, 0);
        error_code = ::pthread_create(&thread_handle_2, 0, threadFunc2, 0);

        if (error_code)
            throw std::runtime_error("thread failure");

        while (s != "quit")
            std::cin >> s;

        ret_code = 0;



    ::pthread_cancel(thread_handle_1) ;
    ::pthread_cancel(thread_handle_2) ;
#endif

}
