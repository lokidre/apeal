/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "string.hpp"
#include "str/format.hpp"
#include "lang.hpp"
#include "stl.hpp"


// we can implement assert only as define
namespace apl {

[[noreturn]]
inline void throwError(StrView msg) {
    throw std::runtime_error(String{msg});
}

template <class ...Args>
[[noreturn]]
void throwError(StrView fmt, Args&& ...args) {
    auto msg = sformat(fmt, std::forward<Args>(args)...);
    throw std::runtime_error(msg);
}



template <class Condition>
void verify(const Condition& cond, StrView msg) {
    if (isFalse(cond))
        throwError(msg);
}

template <class Condition, class ...Args>
void verify(const Condition& cond, StrView fmt, Args&& ...args) {
    if (isFalse(cond))
        throwError(fmt, std::forward<Args>(args)...);
}



//template <class Cond, class ...Args, 
//    std::enable_if_t<!std::is_convertible_v<Cond, std::string_view>, int> = 0>
//void verify(const Cond& cond, StrView fmt, Args&& ...args) {
//    if (isFalse(cond))
//        throwError(fmt, std::forward<Args>(args)...);
//}


//template <class ...Args>
//void verify(StrView str, StrView fmt, Args&& ...args) {
//    verify(!str.empty(), fmt, std::forward<Args>(args)...);
//}



#if _DEBUG
#define ADebugVerify(cond, ...)     ::apl::verify((cond), __VA_ARGS__)
#else  // Release
#define ADebugVerify(...)           ::apl::noop(__VA_ARGS__)
#endif

}  // namespace apl
