/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "error.hpp"
#include "policy.hpp"
#include "assert.hpp"
#include "noncopyable.hpp"


#if APEAL_OS_POSIX
#include <pthread.h>
#endif


namespace apl {

class AEvent: NonCopyable {
public:
#if APEAL_OS_WINDOWS
    using Handle = HANDLE;
#elif APEAL_OS_POSIX
    using Handle = pthread_cond_t;
#else
    using Handle = int;
#endif

    enum Wait {
#if APEAL_OS_WINDOWS
        EVENT_WAIT_INFINITE = INFINITE,
        EVENT_WAIT_TIMEOUT = WAIT_TIMEOUT,
        EVENT_WAIT_SIGNALED = WAIT_OBJECT_0
#else
        EVENT_WAIT_INFINITE = 0x7FFFFFFF,
        EVENT_WAIT_TIMEOUT = 1,
        EVENT_WAIT_SIGNALED = 2
#endif
    };


private:
    Handle handle_{};
    bool valid_{};
    AError error_;

#if APEAL_OS_POSIX
    pthread_mutex_t mutex_;  // pthreads require mutex
#endif

private:
    bool manual_reset_{true};
    bool state_{};

public:  // initialization

    ~AEvent()
    {
        destroy();
    }

public:  // configuration
    void manual_reset(bool opt = true) { manual_reset_ = opt; }

public:  // creation
    bool create()
    {
        bool res = true;

        destroy();

#if APEAL_OS_WINDOWS

        handle_ = ::CreateEventW(0, manual_reset_, FALSE, 0);

        verify(handle_, "CreateEvent error");

#else

        int code = ::pthread_mutex_init(&mutex_, 0);

        if (code != 0)
            return error_.handle_unix_error_code(code);

        code = ::pthread_cond_init(&handle_, 0);

        verify(code==0, "CreateEvent error");

#endif

        valid_ = true;

        return res;
    }

    void destroy()
    {
        if (!valid_)
            return;

#if APEAL_OS_WINDOWS

        ::CloseHandle(handle_);
        handle_ = 0;

#elif APEAL_OS_POSIX
        ::pthread_cond_destroy(&handle_);
        ::pthread_mutex_destroy(&mutex_);
#endif

        valid_ = false;
    }


public:  // using
    void set()
    {
#if APEAL_OS_WINDOWS
        auto result = ::SetEvent(handle_) ;
        verify(result, "SetEvent error");
#else
        if (!state_) {
            ::pthread_mutex_lock(&mutex_);
            if (manual_reset_)
                ::pthread_cond_broadcast(&handle_);
            else
                ::pthread_cond_signal(&handle_);
            ::pthread_mutex_unlock(&mutex_);
            state_ = true;
        }
#endif
    }

    void reset()
    {
        if (!valid_)
            return;
#if APEAL_OS_WINDOWS
        auto result = ::ResetEvent(handle_);
        verify(result, "SetEvent error");
#else
        ::pthread_mutex_lock(&mutex_);
        state_ = false;
        ::pthread_mutex_unlock(&mutex_);
#endif
    }


    void pulse()
    {
        if (!valid_)
            return;

#if APEAL_OS_WINDOWS
        auto result = ::PulseEvent(handle_);
        verify(result, "SetEvent error");

#else
        //::pthread_mutex_lock(&mutex) ;
        ::pthread_cond_signal(&handle_);
#endif
    }

    int wait(Wait condition = EVENT_WAIT_INFINITE, int timeout = 0)
    {
        if (!valid_)
            return 0;

        int result = 0;

#if APEAL_OS_WINDOWS
        DWORD mSeconds = INFINITE;

        if (EVENT_WAIT_TIMEOUT == condition)
            mSeconds = timeout;

        result = ::WaitForSingleObjectEx(handle_, mSeconds, FALSE);

#else

#if TARGET_OS_MAC
        ::pthread_mutex_lock(&mutex_);
#else
        if (EVENT_WAIT_INFINITE == condition)
            ::pthread_mutex_lock(&mutex_);
        else {
            struct timespec ts;
            ts.tv_sec = timeout / 1000;
            ts.tv_nsec = (timeout % 1000) * 1000000;

            ::pthread_mutex_timedlock(&mutex_, &ts);
        }
#endif

        if (!state_)
            ::pthread_cond_wait(&handle_, &mutex_);

        if (!manual_reset_)
            state_ = false;

        ::pthread_mutex_unlock(&mutex_);

#endif

        return result;
    }

};

}  // namespace apl

