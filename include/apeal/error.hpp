/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "types.hpp"
#include "assert.hpp"
#include "string.hpp"
#include "str/sysapi.hpp"

#include "err/eerrno.hpp"

#if APEAL_OS_POSIX
#include <errno.h>
#include "err/eunix.hpp"
#endif

#if APEAL_OS_WINDOWS
#include "err/ewin.hpp"
#endif

#include <stdexcept>


#if 0 // TODO: Error classification
3 types of error classes
    * empty - the error throws the runtime_error with error string
    * code - the error remembers the error code
    * string - error remembers the code and error string
    * message - error contains code, string and prefix and postfix for more formatted error string
#endif

namespace apl {


//
// Obsolete. Needs redesign
//
class AErrorPolicy {
public:
    bool errorPolicyThrow{true};

    bool operator()(const String& message, int code = -1) const {
        if (errorPolicyThrow)
            throw std::runtime_error(message.c_str());

        return false;
    }
};

class AErrorProducer {
public:
    bool errorPolicyThrow{true};
    int errorCode{};
    String errorMessage;

    bool raiseError(StrView message, int code = -1) {
        errorCode = code;
        errorMessage = message;
        if (errorPolicyThrow)
            throw std::runtime_error(errorMessage);
        return false;
    }

protected:
    AErrorPolicy error_;
};


//
// Obsolete class
// Needs redesign
class AError {
public:
    // typedef typename typemap::find<KStringPolicy,policy_T>::X::Value::string_type string_type ;
    // typedef typename typemap::find<KErrorPolicy,policy_T>::X::Value ErrorPolicy ;

protected:
    AErrorPolicy error_;

public:
    int error_code{};
    String error_string;

public:
    AError() {}

    bool raise_error(int raise_error_code = 0, const String& raise_error_string = "") {
        if (raise_error_code != 0)
            error_code = raise_error_code;

        if (!raise_error_string.empty())
            error_string = raise_error_string;

        if (error_string.empty())
            error_string = "generic error";

        return error_(error_string);
    }

public:
#if ATARGET_OS_POSIX
    bool handle_errno_error() { return handle_unix_error_code(errno); }
    bool handle_unix_error_code(const int code) {
        error_string = getErrorStringUnix(code);
        return error_(error_string, code);
    }
#endif

    //#if APEAL_OS_WINDOWS
    //    bool handle_windows_last_error()
    //    {
    //        int errorCode = GetLastError();
    //        error_string = getErrorStringWindows(errorCode);
    //        return error_(error_string, errorCode);
    //    }
    //#endif
};


template <class... Args>
void assert_runtime_error(bool test, StrView fmt, Args... args) {
    if (!test)
        throw std::runtime_error(sformat(fmt, args...));
}

using Error = AError;

} // namespace apl


//#ifdef _DEBUG
//#define internal_error() { throw std::runtime_error("Internal Error: " __FILE__ " " __LINE__ ) ; }
//#else
//#define internal_error()
//#endif
