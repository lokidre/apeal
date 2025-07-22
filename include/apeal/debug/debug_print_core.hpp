/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../str/sysapi.hpp"
#include "../lang.hpp"

#include <iostream>


#if APEAL_OS_MAC
#include <sys/stat.h>
#include <unistd.h>
#endif

#if APEAL_OS_ANDROID
#include <android/log.h>
#include <jni.h>
#endif

#if APEAL_OS_WINDOWS
#include <io.h>
#endif


//
// Low leve debugging stuff
//

namespace apl {

#if APEAL_OS_WINDOWS
void debug_windows_console_create();
void debug_windows_print_console(FILE* output, std::string_view msg) noexcept;
void debug_windows_print_console_prefix(FILE* output, std::string_view prefix, std::string_view msg) noexcept;
#endif

//template <class O>


template <class O>
void debug_print_console(const O& output, std::string_view msg) noexcept
 {
     try { // Make sure no exception is thrown

 #if APEAL_OS_WINDOWS
         debug_windows_print_console(output, msg);

         //static auto isTerminal = [&] {
         //    auto cwnd = GetConsoleWindow();
         //    auto currPID = GetCurrentProcessId();
         //    DWORD wndPID{};
         //    GetWindowThreadProcessId(cwnd, &wndPID);
         //    return currPID == wndPID;
         //}();

         //static auto hasTerminal = _isatty(_fileno(output));

         //if (IsDebuggerPresent() && !hasTerminal /*&& !isTerminal*/) {
         //    //ApiString sapi{msg};
         //    OutputDebugStringW(ApiString{msg}.win());
         //    OutputDebugStringW(L"\n");
         //} else {
         //    fwrite(msg.data(), sizeof(msg.front()), msg.size(), output);
         //    fputc('\n', output);
         //}

 #else  // Not Windows
         //auto s = sf.c_str();

 #if APEAL_OS_ANDROID
         ApiString s{msg};
         if (output == stdout) {
             __android_log_print(ANDROID_LOG_INFO, "Debug", "%s", s.c_str());
         } else {
             __android_log_print(ANDROID_LOG_ERROR, "Error", "%s", s.c_str());
         }
 #else  // Not Android
         fwrite(msg.data(), sizeof(msg.front()), msg.size(), output);
         //fputs(std::string(sf).c_str(), output);
         fputc('\n', output);
 #endif  // Not Android
 #endif  // Not Windows
     }
     catch (...) {}
 }




//template <class O>
//void debug_print_console(const O& output, const std::string &s) noexcept {
//    debug_print_console(output, std::string_view{s.data(), s.size()});
//}



template <class O>
void debug_print_console_prefix(const O& output, std::string_view prefix, std::string_view msg) noexcept
 {
     try { // Make sure no exception is thrown
 #if APEAL_OS_WINDOWS
         // TODO: for windows console. Create console if no debugger present
         debug_windows_print_console_prefix(output, prefix, msg);
        /* if (IsDebuggerPresent()) {
             ApiString apiPrefix{prefix}, apiMsg{msg};
             OutputDebugStringW(apiPrefix.utf16());
             OutputDebugStringW(apiMsg.utf16());
             OutputDebugStringW(L"\n");
         }*/
 #else  // Not Windows
 #if APEAL_OS_ANDROID
         ApiString apiPrefix{prefix}, apiMsg{msg};
         if (output == stdout) {
             __android_log_print(ANDROID_LOG_INFO, apiPrefix.c_str(), "%s", apiMsg.c_str());
         } else {
             __android_log_print(ANDROID_LOG_ERROR, apiPrefix.c_str(), "%s", apiMsg.c_str());
         }
 #else  // Not Android
         fwrite(prefix.data(), sizeof(prefix.front()), prefix.size(), output);
         fwrite(msg.data(), sizeof(msg.front()), msg.size(), output);
         fputc('\n', output);
 #endif  // Not Android
 #endif  //  Not Windows
     }
     catch (...) {}
 }



//template <class O>
//inline
//void debug_print_console_prefix(const O& output, std::string_view prefix, std::string_view sf) noexcept {
//    debug_print_console_prefix(output, prefix.c_str(), sf);
//}



#if _DEBUG
#define ADetailDebugPrintCore(_msg)     ::apl::debug_print_console(stdout, _msg)
#else
#define ADetailDebugPrintCore           ADebugNoop
#endif

} // namespace apl
