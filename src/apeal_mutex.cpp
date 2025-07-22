/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/mutex.hpp"


namespace apl {


bool AMutex::create() {
    
        destroy() ;

        #if APEAL_OS_WINDOWS

        handle_ = ::CreateMutex ( 0, FALSE, 0 ) ;

        if ( handle_ == 0 )
            throw std::runtime_error("Mutex:Cannot create") ;


        #elif APEAL_OS_POSIX

        int error_code = ::pthread_mutex_init ( &handle_, 0 ) ;

        if ( error_code )
            throw std::runtime_error("Mutex:Cannot create") ;

        #endif

        valid_ = true ;

        return true ;
}




void AMutex::destroy() {

        if ( !valid_ )
            return ;

#if APEAL_OS_WINDOWS
        ::CloseHandle(handle_) ;
        handle_ = INVALID_HANDLE_VALUE ;
#elif APEAL_OS_POSIX
        ::pthread_mutex_destroy(&handle_) ;
#endif

        valid_ = false ;
}



bool AMutex::lock ( int timeout ){

        if ( !valid_ )
            create() ;

        #if APEAL_OS_WINDOWS

        DWORD status = ::WaitForSingleObject ( handle_, timeout ) ;

        if ( status == WAIT_FAILED ) {
            return false ;
        // return handle_error_() ;
        //get_error_string_windows(error_code = ::GetLastError(), error_string ) ;
        //raise_error() ;
        //return false ;
        }

        #elif APEAL_OS_POSIX

        // needed timed mutex
        unused(timeout) ;
        int error_code = ::pthread_mutex_lock(&handle_) ;

        if ( error_code != 0 ) {
            // GetErrorStringUnix(this->error_code,this->error_string) ;
            // return this->raise_error() ;
            return false ;
        }

        #endif

        return true ;
    }




bool AMutex::unlock(){

        if ( !valid_ )
            create() ; //throw std::runtime_error("Mutex:Attempt to unlock invalid mutex") ;

        #if APEAL_OS_WINDOWS

        if ( !::ReleaseMutex ( handle_ ) ) {
        // get_error_string_windows(this->error_code = ::GetLastError(), this->error_string ) ;
        // return raise_error() ;
            return false ;
        }

        #elif APEAL_OS_POSIX

        int error_code = pthread_mutex_unlock(&handle_) ;

        if ( error_code != 0 ) {
            return false ;
        // GetErrorStringUnix(this->error_code,this->error_string) ;
        // return this->raise_error() ;
        }

        #endif

        return true ;
}



} // namespace


