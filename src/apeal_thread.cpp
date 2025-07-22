/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/thread.hpp"


namespace apl {

namespace apl_private {

void ThreadLauncherBase::launch(ThreadContext* ctx) {
    
    launchEvent_.create();

    ctx->parm = this; //&target_ ;

#if APEAL_OS_WINDOWS

    SECURITY_ATTRIBUTES sa{};

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    //this->ptarget = target_.release() ;

    ctx->handle = ::CreateThread(&sa, 0, (LPTHREAD_START_ROUTINE)threadFuncBase, ctx->parm, 0, &ctx->id);

    //if ( handle_ == 0 )
    //	return handle_error_(::GetLastError()) ;

#elif APEAL_OS_POSIX

    int errorCode = ::pthread_create(&ctx->handle, 0, threadFuncBase, ctx->parm);

    //throw std::runtime_error("launched");

    if (errorCode)
        throw std::runtime_error("Cannot launch pthread");
#endif
    launchEvent_.wait();
}

}

void Thread::destroy() noexcept{
        if (!valid_)
            return;

#if APEAL_OS_WINDOWS
        ::CloseHandle(ctx_.handle);
#elif APEAL_OS_POSIX

#if !APEAL_OS_ANDROID
        ::pthread_cancel(ctx_.handle);
#endif

        ::pthread_detach(ctx_.handle);  // Must be called to free up the resources
#endif

        valid_ = false;
}



bool Thread:: terminate(int exitCode){
#if APEAL_OS_WINDOWS
        if (!::TerminateThread(ctx_.handle, exitCode))
            return false;
#else
        unused(exitCode);
#endif
        return true;
}



int Thread::get_exit_code(){
        int exit_code = 0;
#if APEAL_OS_WINDOWS
        DWORD c;
        if (!::GetExitCodeThread(ctx_.handle, &c))
            return 0;
        exit_code = int(c);
#else
#endif
        return exit_code;
}
    



int Thread::wait(uint32_t msec){
        uint32_t result = 0;

#if APEAL_OS_WINDOWS

        result = ::WaitForSingleObject(ctx_.handle, msec);

#elif APEAL_OS_POSIX

        unused(msec);
        ::pthread_join(ctx_.handle, 0);
#endif

        return (int)result;
}



Thread::Handle Thread::self(){
#if APEAL_OS_WINDOWS
        return GetCurrentThread();
#elif APEAL_OS_POSIX
        return pthread_self();
#endif
}



void Thread::exit(int exitCode){
#if APEAL_OS_WINDOWS
        ExitThread(exitCode);
#elif APEAL_OS_POSIX
        pthread_exit(&exitCode);
#endif
}



bool Thread::handle_error_(int error_code){
        String error_string, s;
        AErrorPolicy error_policy;

#if APEAL_OS_WINDOWS
        s = getErrorStringWindows(error_code);
#elif APEAL_OS_POSIX
        s = getErrorStringUnix(error_code);
#endif

        error_string = sformat("thread:%d:%s", error_code, s);

        return error_policy(error_string);
}



}  // namespace apl

