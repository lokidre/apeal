/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/process.hpp"


namespace apl {

bool Process::create(StrView pgm, StrView arg){

#if APEAL_OS_WINDOWS
        si_ ={};
        si_.cb = sizeof(si_);

        ::GetStartupInfoW(&si_);

        // prepare for windows
        string_type cmdline_ex;
        DWORD f = 0;

        if (new_console)
            f |= CREATE_NEW_CONSOLE;

        if (no_window)
            f |= CREATE_NO_WINDOW;

        if (suspend)
            f |= CREATE_SUSPENDED;

        if (stdin_handle != InvalidHandle || stdout_handle != InvalidHandle || stderr_handle != InvalidHandle) {
            si_.dwFlags = STARTF_USESTDHANDLES;
            si_.hStdInput  = (stdin_handle == InvalidHandle) ? ::GetStdHandle(STD_INPUT_HANDLE) : stdin_handle;
            si_.hStdOutput = (stdout_handle == InvalidHandle) ? ::GetStdHandle(STD_OUTPUT_HANDLE) : stdout_handle;
            si_.hStdError  = (stderr_handle == InvalidHandle) ? ::GetStdHandle(STD_ERROR_HANDLE) : stderr_handle;
        }

        string_type environ_str;

        if (environment.size()) {
            for (const auto& e : environment) {
                environ_str.append(e.first);
                environ_str.append(1, '=');
                environ_str.append(e.second);
                environ_str.append(1, 0);
            }

            environ_str.append(1, 0);
        }

        ApiString argApi{arg};


        BOOL ret = ::CreateProcessW(pgm.empty() ? 0 : ApiString{pgm}.win(),
            arg.empty() ? 0 : (LPWSTR)argApi.win(),
            0, 0,   // security attributes
            TRUE,  // inherit handles
            f, // various flags
            environ_str.empty() ? 0 : (LPVOID)environ_str.data(),
            current_dir.empty() ? 0 : ApiString{current_dir}.win(),
            &si_, &pi_);

        verifyWindowsLastError(ret, pgm.empty() ? arg : pgm);

#elif APEAL_OS_POSIX

        // prepare environment for the child process
        /* TODO: environment
        char** envp = 0;

        if (!environ.empty()) {
            envp = new char* [environ.size()+1];
            envp[environ.size()] = 0;

            char** penv = envp;

            for (auto e = environ.begin(); e != environ.end(); ++e, ++penv) {
                size_t name_size = e->first.size(), value_size = e->second.size();
                *penv = new char[name_size+1+value_size+1];
                memcpy(*penv, e->first.data(), name_size);
                *((*penv)+name_size) = '=';
                memcpy((*penv)+name_size+1, e->second.data(), value_size);
                *((*penv)+name_size+1+value_size) = 0;
            }
        }
         */

        //const char **argv = argv_(arg) ;

        child_pid_ = ::fork();

        if (child_pid_ == pid_t(-1))
            return this->handle_errno_error();

        if (child_pid_ == 0) {  // child process
            // change directory, if needed
            //if ( !current_dir.empty() ) {
            //  if ( !::chdir(current_dir.c_str()) )
            //    ::exit(errno) ;
            //}

            String apgm{pgm};
            String aarg{arg};

            ::execl(apgm.c_str(), aarg.c_str(), nullptr);
            //::system(pgm.c_str()) ;
            ::exit(2);  // child should never reach here, it should be an error
        }

        // parent process, (child should never be here, since execv should never return)

#elif APEAL_OS_XAOS

        XaosParam parm;

        parm.object  = this;
        parm.buffer  = Module;
        parm.buffer2 = CommandLine;
        parm.param   = 0;
        if (!XaosApi(&parm, API_CLASS_PROCESS, API_PROCESS_CREATE))
            return false;

#endif

        return true;
}




bool Process::wait(int timeout){

#if APEAL_OS_WINDOWS
        if (::WaitForSingleObject(pi_.hProcess, timeout == wait_infinite ? INFINITE : timeout) != WAIT_OBJECT_0)
            return false;
#else
        // needed
#endif

        return true;
}




int Process::pid() const{
#if APEAL_OS_WINDOWS
        return int(pi_.dwProcessId);
#else
        return child_pid_;
#endif
}




int Process::exit_code(){

#if APEAL_OS_WINDOWS
        if (::WaitForSingleObject(pi_.hProcess, INFINITE) != WAIT_OBJECT_0)
            return -1;

        DWORD ec = 0;
        auto res = ::GetExitCodeProcess(pi_.hProcess, &ec);
        
        verifyWindowsLastError(res, "GetExitCodeProcess");

        return int(ec);

#elif APEAL_OS_POSIX

        int status;
        if (::waitpid(child_pid_, &status, WUNTRACED) == -1) {
            this->handle_errno_error();
            return -1;
        }

        return status;
#endif
}




bool Process::terminate(int exitCode){
#if APEAL_OS_WINDOWS
        ::TerminateProcess(pi_.hProcess, exitCode);
#elif APEAL_OS_POSIX
#endif
        return true;
}




int Process::self_pid(){
#if APEAL_OS_WINDOWS
        return ::GetCurrentProcessId();
#else
        return ::getpid();
#endif
}




void Process::self_exit(int exitCode){
#if APEAL_OS_WINDOWS
        ::ExitProcess(exitCode);
#else
        ::exit(exitCode);
    #endif
}



} // namespace


