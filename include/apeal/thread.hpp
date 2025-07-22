/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "def.hpp"
#include "error.hpp"
#include "assert.hpp"
#include "string.hpp"
#include "util.hpp"
#include "event.hpp"

#include <stdexcept>
#include <cstdint>

#include <memory>
#include <type_traits>

#if APEAL_OS_POSIX
#include <pthread.h>
#endif

#include <thread>



namespace apl {

namespace apl_private {


struct ThreadContext {
#if APEAL_OS_WINDOWS

    using Handle = HANDLE;
    //using Func = DWORD (*)(LPVOID) ;

#elif APEAL_OS_POSIX

    using Handle = pthread_t;
    //using Func = void *(*)(void *) ;

#else

    using Handle = int;

#endif

    Handle handle{};

    void* parm{};
    unsigned long flags{};
    unsigned long id{};
};

struct ThreadLauncherBase {
    virtual ~ThreadLauncherBase() = default;

    using Self = ThreadLauncherBase;

    AEvent launchEvent_;

    void launch(ThreadContext* ctx);

    static void* threadFuncBase(void* params) noexcept {
        return static_cast<Self*>(params)->onLaunchFunc(params);
    }

    virtual void* onLaunchFunc(void* params) = 0;

};


template <class Target>
struct ThreadLauncher: public ThreadLauncherBase {
    using Self = ThreadLauncher;

    template <class L>
    ThreadLauncher(L&& t): target_(std::forward<L>(t)) {}

    //void launch(ThreadContext* ctx);

    //virtual ~ThreadLauncher() {
    //	int a = 0 ;
    //}


//     {
//         launchEvent_.create();

//         ctx->parm = this; //&target_ ;

// #if APEAL_OS_WINDOWS

//         SECURITY_ATTRIBUTES sa{};

//         sa.nLength = sizeof(SECURITY_ATTRIBUTES);
//         sa.lpSecurityDescriptor = NULL;
//         sa.bInheritHandle = TRUE;

//         //this->ptarget = target_.release() ;

//         ctx->handle = ::CreateThread(&sa, 0, (LPTHREAD_START_ROUTINE)threadFunc_, ctx->parm, 0, &ctx->id);

//         //if ( handle_ == 0 )
//         //	return handle_error_(::GetLastError()) ;

// #elif APEAL_OS_POSIX

//         int errorCode = ::pthread_create(&ctx->handle, 0, threadFunc_, ctx->parm);
 
//         //throw std::runtime_error("launched");

//         if (errorCode)
//             throw std::runtime_error("Cannot launch pthread");
// #endif
//         launchEvent_.wait();
//     }

    template <size_t... Idxs>
    static void execute(typename Target::element_type& up, std::index_sequence<Idxs...>) {
        std::invoke(std::move(std::get<Idxs>(up))...);
    }

    void* threadFunc_(void* params) noexcept {
        //auto target = static_cast<Target*>(params) ;
        auto self = static_cast<Self*>(params);

        Target local(std::forward<Target>(self->target_));

        self->launchEvent_.pulse();

        execute(*local, std::make_index_sequence<std::tuple_size_v<typename Target::element_type>>());

        return 0;
    }

    void* onLaunchFunc(void* params) override {
        return threadFunc_(params);
    }


private:
    Target target_;
    //typename Target::element_type *ptarget ;
};


template<class Target>
inline
void launchThread(ThreadContext* ctx, Target&& t) {
    ThreadLauncher<Target> launcher(std::forward<Target>(t));
    launcher.launch(ctx);
}




}


class Thread {
    using Self = Thread;

public:
    using Handle = apl_private::ThreadContext::Handle;

private:
    bool valid_{};
    bool detached_{};

public:
    apl_private::ThreadContext ctx_;

    enum {
#if APEAL_OS_WINDOWS
        wait_timeout = WAIT_TIMEOUT,
        wait_signaled = WAIT_OBJECT_0
#else
        wait_timeout,
        wait_signaled
#endif
    };

public:
    // Construct the object without launching the thread
    Thread() noexcept {}

    template <class F, class ...Args>
    explicit Thread(F&& f, Args&&...args) {
        operator() (f, args...);
    }


    ~Thread() {
        if (detached_)
            return;
        destroy();
    }

public:
    template <class F, class ...Args>
    Self& operator()(F&& f, Args&&...args) {
        return create(f, std::forward<Args>(args)...);
    }

    template <class F, class ...Args>
    Self& create(F&& f, Args&&...args) {
        destroy();  // if there was a thread before, terminate it

        apl_private::launchThread(&ctx_, 
            std::make_unique<std::tuple<std::decay_t<F>, 
            std::decay_t<Args>...>>(std::forward<F>(f), std::forward<Args>(args)...)
        );

        return *this;
    }

    void destroy() noexcept;
//     {
//         if (!valid_)
//             return;

// #if APEAL_OS_WINDOWS
//         ::CloseHandle(ctx_.handle);
// #elif APEAL_OS_POSIX

// #if !APEAL_OS_ANDROID
//         ::pthread_cancel(ctx_.handle);
// #endif

//         ::pthread_detach(ctx_.handle);  // Must be called to free up the resources
// #endif

//         valid_ = false;
//     }


public:
    /*
        bool suspend()
        {
            if ( !valid_ )
                return false ;

            #if APEAL_OS_WINDOWS
            if ( ::SuspendThread(handle_) == DWORD(-1) ) {
                //error_code = ::GetLastError(), error_string = get_error_string_windows(error_code) ;
                //raise_error() ;
                return false ;
            }
            #else
            #endif

            return true ;
        }

        bool resume()
        {
            #if ( APEAL_OS_WINDOWS )
            if ( ::ResumeThread(handle_) == DWORD(-1) ) {
                //error_code = ::GetLastError(), error_string = get_error_string_windows(error_code) ;
                //raise_error() ;
                return false ;
            }
            #else
            #endif
            return true ;
        }

        int get_priority()
        {
            int priority = 0 ;
            #if ( APEAL_OS_WINDOWS )
            priority = ::GetThreadPriority ( handle_ ) ;
            #else
            #endif
            return priority ;
        }

        bool set_priority ( const int priority )
        {
            #if ( APEAL_OS_WINDOWS )
            if ( !::SetThreadPriority ( handle_, priority ) )
                return false ;
            #else
            //priority = priority ;
            #endif
            return true ;
        }
    */
    bool terminate(int exitCode = 0);
//     {
// #if APEAL_OS_WINDOWS
//         if (!::TerminateThread(ctx_.handle, exitCode))
//             return false;
// #else
//         unused(exitCode);
// #endif
//         return true;
//     }


    int get_exit_code();
//     {
//         int exit_code = 0;
// #if APEAL_OS_WINDOWS
//         DWORD c;
//         if (!::GetExitCodeThread(ctx_.handle, &c))
//             return 0;
//         exit_code = int(c);
// #else
// #endif
//         return exit_code;
//     }

    void detach() {
        detached_ = true;
    }

    int wait(uint32_t msec = 0x7FFFFFFF);
//     {
//         uint32_t result = 0;

// #if APEAL_OS_WINDOWS

//         result = ::WaitForSingleObject(ctx_.handle, msec);

// #elif APEAL_OS_POSIX

//         unused(msec);
//         ::pthread_join(ctx_.handle, 0);
// #endif

//         return (int)result;
//     }


public:  // current thread functions

    static Handle self();
//     {
// #if APEAL_OS_WINDOWS
//         return GetCurrentThread();
// #elif APEAL_OS_POSIX
//         return pthread_self();
// #endif
//     }

    static void exit(int exitCode = 0);
//     {
// #if APEAL_OS_WINDOWS
//         ExitThread(exitCode);
// #elif APEAL_OS_POSIX
//         pthread_exit(&exitCode);
// #endif
//     }

private:
    bool handle_error_(int error_code);
//     {
//         String error_string, s;
//         AErrorPolicy error_policy;

// #if APEAL_OS_WINDOWS
//         s = getErrorStringWindows(error_code);
// #elif APEAL_OS_POSIX
//         s = getErrorStringUnix(error_code);
// #endif

//         error_string = sformat("thread:%d:%s", error_code, s);

//         return error_policy(error_string);
//     }
};


}  // namespace apl

