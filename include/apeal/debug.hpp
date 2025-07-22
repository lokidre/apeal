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
#include "str/format.hpp"

#include "debug/debug_print_core.hpp"
#include "debug/debug_trap.hpp"



namespace apl {


template <class ...Args>
std::string debug_sformat(std::string_view fmt, Args&& ...args) noexcept {
    return sformat(fmt, std::forward<Args>(args)...);
}

constexpr
auto debug_sformat(std::string_view fmt) noexcept { return fmt; }


constexpr
auto debug_format_function(std::string_view func) noexcept { return func; }

constexpr
auto debug_print_format_function(std::string_view func) noexcept { return func;}


inline
void debug_print_stdout(std::string_view s) noexcept {
    debug_print_console(stdout, s);
}

template <class ...Args>
void debug_print_stdout(std::string_view fmt, Args&&... args) noexcept {
    debug_print_console(stdout, debug_sformat(fmt, std::forward<Args>(args)...));
}

inline
void debug_print_stdout_prefix(std::string_view prefix, std::string_view msg) noexcept {
    debug_print_console_prefix(stdout, prefix, msg);
}

template <class ...Args>
void debug_print_stdout_prefix(std::string_view prefix, std::string_view fmt, Args&&... args) noexcept {
    debug_print_console_prefix(stdout, prefix, debug_sformat(fmt, std::forward<Args>(args)...));
}




inline
void debug_print_stdout_pathline(std::string_view file, int line, std::string_view s) noexcept {
    debug_print_console(stdout, debug_sformat("%s:%d: %s", file, line, s));
}

template <class ...Args>
void debug_print_stdout_pathline(std::string_view file, int line, std::string_view fmt, Args &&...args) noexcept {
    debug_print_stdout_pathline(file, line, debug_sformat(fmt, std::forward<Args>(args)...));
}



inline
void debug_print_stdout_fileline(std::string_view file, int line, std::string_view s) noexcept {
    debug_print_stdout_pathline(file, line, s);
}

template <class ...Args>
void debug_print_stdout_fileline(std::string_view file, int line, std::string_view fmt, Args &&...args) noexcept {
    debug_print_stdout_pathline(file, line, fmt, std::forward<Args>(args)...);
}


inline
void debug_print_stdout_func(std::string_view func) noexcept {
    debug_print_stdout(func);
}

inline
void debug_print_stdout_func(std::string_view func, std::string_view msg) noexcept {
    debug_print_stdout("%s: %s", func, msg);
}

inline
void debug_print_stdout_func_prefix(std::string_view prefix, std::string_view func, std::string_view msg) noexcept {
    debug_print_stdout_prefix(prefix, "%s: %s", func, msg);
}


template <class ...Args>
void debug_print_stdout_func(std::string_view func, std::string_view fmt, Args&&... args) noexcept {
    debug_print_stdout_func(func, debug_sformat(fmt, std::forward<Args>(args)...));
}

inline
void debug_print_stdout_func_prefix(std::string_view prefix, std::string_view func) noexcept {
    debug_print_stdout_prefix(prefix, func);
}

template <class ...Args>
void debug_print_stdout_func_prefix(std::string_view prefix, std::string_view func, std::string_view fmt, Args &&...args) noexcept {
    debug_print_stdout_func_prefix(prefix, func, debug_sformat(fmt, std::forward<Args>(args)...));
}

inline
void debug_print_stdout_log(std::string_view func) noexcept {
    debug_print_console(stdout, func);
}

inline
void debug_print_stdout_log(std::string_view func, std::string_view s) noexcept {
    debug_print_console_prefix(stdout, func, s);
}

template <class ...Args>
void debug_print_stdout_log(std::string_view func, std::string_view fmt, Args &&...args) noexcept {
    debug_print_stdout_log(func, debug_sformat(fmt, std::forward<Args>(args)...));
}


#if _DEBUG

#define ADebugPrint(...)                        ::apl::debug_print_stdout(__VA_ARGS__)

#if _MSVC_TRADITIONAL

#define ADebugPrintPrefix(_prefix, ...)         ::apl::debug_print_stdout_prefix((_prefix), __VA_ARGS__)

#define ADebugPrintFunc(...)                    ::apl::debug_print_stdout_func(__FUNCTION__, __VA_ARGS__)
#define ADebugPrintFuncPrefix(_prefix, ...)     ::apl::debug_print_stdout_func_prefix((_prefix), __FUNCTION__, __VA_ARGS__)

#else  // Not MSVC traditional

#define ADebugPrintPrefix(_prefix, ...)         ::apl::debug_print_stdout_prefix((_prefix) __VA_OPT__(,) __VA_ARGS__)

#if _MSC_VER
#define ADebugPrintFunc(...)                    ::apl::debug_print_stdout_func(__FUNCTION__ __VA_OPT__(,) __VA_ARGS__)
#define ADebugPrintFuncPrefix(_prefix, ...)     ::apl::debug_print_stdout_func_prefix((_prefix), __FUNCTION__ __VA_OPT__(,) __VA_ARGS__)
#else  // if clang or gcc
#define ADebugPrintFunc(...)                    ::apl::debug_print_stdout_func(__PRETTY_FUNCTION__ __VA_OPT__(,) __VA_ARGS__)
#define ADebugPrintFuncPrefix(_prefix, ...)     ::apl::debug_print_stdout_func_prefix((_prefix), __PRETTY_FUNCTION__ __VA_OPT__(,) __VA_ARGS__)
#endif  // MSC
#endif  // MSVC Traditional


// Need further development
#define ADebugLog(...)                          ::apl::debug_print_stdout_log(__FUNCTION__, __VA_ARGS__)


#define ADebugPrintPath(...)                    ::apl::debug_print_stdout_pathline(__FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define ADebugPrintFile(...)                    ::apl::debug_print_stdout_fileline(__FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)

#define ADebugPrintPathPrefix(_prefix, ...)     ::apl::debug_print_stdout_pathline_prefix((_prefix), __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define ADebugPrintFilePrefix(_prefix, ...)     ::apl::debug_print_stdout_fileline_prefix((_prefix), __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define ADebugPrintLogPrefix(_prefix, ...)      ::apl::debug_print_stdout_log_prefix((_prefix), __FUNCTION__ __VA_OPT__(,) __VA_ARGS__)

#else  // release

#define ADebugPrint                     ADebugNoop
#define ADebugPrintFunc                 ADebugNoop

#define ADebugPrintPrefix               ADebugNoop
#define ADebugPrintFuncPrefix           ADebugNoop

#endif

}  // namespace apl

