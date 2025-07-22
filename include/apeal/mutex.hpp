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
#include "util.hpp"

#include <stdexcept>

#if APEAL_OS_POSIX
#include <pthread.h>
#endif


namespace apl {


class AMutex {
private :
    #if APEAL_OS_WINDOWS
    typedef HANDLE mutex_handle_type ;
    #elif APEAL_OS_POSIX
    typedef pthread_mutex_t mutex_handle_type ;
    #else
    typedef int mutex_handle_type ;
    #endif

    using Handle = mutex_handle_type ;

private :
    Handle handle_ ;
    bool valid_ ;

    enum {
        infinite = -1
    } ;

public :
    AMutex()
    {
        ctor() ;
    }

    ~AMutex()
    {
        destroy() ;
    }


    // copy constructor
    explicit AMutex(const AMutex &m)
    {
        ctor() ;
        if ( m.valid() )
            create() ;
    }

    void ctor()
    {
        valid_ = false ;
    }

public :  // User interface
    bool valid() const {  return valid_ ;  }

    bool create();
    // {
    //     destroy() ;

    //     #if APEAL_OS_WINDOWS

    //     handle_ = ::CreateMutex ( 0, FALSE, 0 ) ;

    //     if ( handle_ == 0 )
    //         throw std::runtime_error("Mutex:Cannot create") ;


    //     #elif APEAL_OS_POSIX

    //     int error_code = ::pthread_mutex_init ( &handle_, 0 ) ;

    //     if ( error_code )
    //         throw std::runtime_error("Mutex:Cannot create") ;

    //     #endif

    //     valid_ = true ;

    //     return true ;
    // }


    void destroy();
    bool lock ( const int timeout = -1 );
    bool unlock();
};



template <class MutexT>
class ALock {
public:
    ALock(MutexT &m): mutex_{m} {
        mutex_.lock() ;
    }

    ~ALock() {
        mutex_.unlock() ;
    }

private :
    MutexT &mutex_ ;
} ;


//
// Dummy mutex, just mimics the real mutex, for use in templates
//
class AVoidMutex {
public :
    bool create() {  return true ;  }
    void destroy() {}
    bool lock ( const int timeout = -1 ) {  return true ;  }
    bool unlock() {  return true ;  }
} ;


class DummyMutex {
public :
    bool create() { return true; }
    void destroy() {}
    bool lock(int timeout = -1) { return true; }
    bool unlock(){ return true; }
} ;

template <class MutexT>
class DummyLock {
public:
    DummyLock(const MutexT&) {};
};

}  // namespace apl
