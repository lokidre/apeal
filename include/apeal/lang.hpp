/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

// Some small C++ language enhancements and commonly used tools

namespace apl {

#if __GNUC__
    #define AForceInline inline __attribute__((always_inline))
#elif _MSC_VER
    #define AForceInline inline __forceinline
#else
    #define AForceInline inline
#endif

#ifdef __GNUC__
    #define ALikelyDetail(_expr) static_cast<bool>(__builtin_expect(static_cast<bool>(_expr), 1))
#else
    #define ALikelyDetail(_expr) static_cast<bool>((_expr))
#endif

#define ALikely ALikelyDetail


//
// faking gcc/clang __has_ for Microsoft Studio until they are supported
//
//#if _MSC_VER
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif
//#endif



// A function doint nothing. Useful in macros
// #define macro(...) noop(__VA_ARGS__)

constexpr void noop() noexcept {}

template <class ...Args>
constexpr void noop(Args&&...) noexcept{}

#define ADebugNoop(...) ::apl::noop(__VA_ARGS__)



}  // namespace apl

