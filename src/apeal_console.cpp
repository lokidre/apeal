/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/console.hpp>

#if APEAL_OS_WINDOWS
#include "../include/apeal/windows/windows.hpp"

#include <cstdio>

#include <fcntl.h>
#include <io.h>
#include <ios>

#endif


namespace apl {

FILE* Console::out_{stdout};

int Console::carret_;

char Console::spaceChar_{' '};

char Console::indentChar_{' '};
int Console::indentCount_{2};

char Console::tabChar_{'\t'};
int Console::tabCount_{1};

char Console::lfChar_{'\n'};  // Line Feed (new line) char
char Console::crChar_{'\r'};  // Carriage return char



std::mutex Console::mutex_{};
bool Console::atomic_{false};


#if APEAL_OS_WINDOWS

static void createWindowsConsole()
{
    static bool consoleCreated_ = false;

    static FILE* output = stdout;

    if (consoleCreated_)
        return; //output;


    static auto isTerminal = [&] {
        auto cwnd = GetConsoleWindow();
        auto currPID = GetCurrentProcessId();
        DWORD wndPID{};
        GetWindowThreadProcessId(cwnd, &wndPID);
        return currPID == wndPID;
    }();

    static auto hasTerminal = _isatty(_fileno(stdout));

    if (hasTerminal)
        return; // output;


    FreeConsole();

    AllocConsole();
    AttachConsole(ATTACH_PARENT_PROCESS);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    csbi.dwSize.Y = 200;

    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), csbi.dwSize);

#pragma warning (disable:4311)

    int stdoutHandle = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    int stdinHandle = _open_osfhandle((intptr_t)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
    int stderrHandle = _open_osfhandle((intptr_t)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);

    FILE* stdoutFile = _fdopen(stdoutHandle, "w");
    FILE* stdinFile = _fdopen(stdinHandle, "r");
    FILE* stderrFile = _fdopen(stderrHandle, "w");

    *stdout = *stdoutFile;
    *stdin = *stdinFile;
    *stderr = *stderrFile;


    setvbuf(stdout, 0, _IONBF, 0);
    setvbuf(stdin, 0, _IONBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);

    std::ios_base::sync_with_stdio();

    consoleCreated_ = true;

    output = stdoutFile;
    return; //output;
}

static void printWindowsConsole(std::string_view sf) noexcept {

    static auto isTerminal = [&] {
        auto cwnd = GetConsoleWindow();
        auto currPID = GetCurrentProcessId();
        DWORD wndPID{};
        GetWindowThreadProcessId(cwnd, &wndPID);
        return currPID == wndPID;
        }();

    static auto hasTerminal = _isatty(_fileno(stdout));

    if (IsDebuggerPresent() && !hasTerminal /*&& !isTerminal*/) {
        ApiString apiStr{ sf };
        OutputDebugStringW(apiStr.win());
    } else {  // Running under debugger
        //if (isTerminal) {
        std::fwrite(sf.data(), sizeof(sf.front()), sf.size(), stdout);
        //}
    }
}

#endif


int Console::print_core(std::string_view text) noexcept {
#if APEAL_OS_WINDOWS
    printWindowsConsole(text);
#elif APEAL_OS_ANDROID
    ApiString apiStr{text};
    if (out_ == stdout) {
        __android_log_print(ANDROID_LOG_INFO, "Debug", "%s", apiStr.c_str());
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "Error", "%s", apiStr.c_str());
    }
#else  // Other OS
    std::fwrite(text.data(), sizeof(text.front()), text.size(), out_);
#endif  // OS
    return static_cast<int>(std::ssize(text));
}

// Core text string and character printing
int Console::str_core(StrView text) noexcept {
    if (text.empty())
        return 0;

    carret_ += print_core(text);

    // Reset the carret position if necessary
    auto last = text.back();
    if (last == crChar_ || last == lfChar_)
        carret_ = 0;

    return static_cast<int>(std::ssize(text));
}

// Character printing
int Console::ch_core(Char c, int count) noexcept {
    StrView text{std::addressof(c), 1};
    for (Index i = 0; i < count; ++i) {
        carret_ += print_core(text);
    }
    if (c == crChar_ || c == lfChar_)
        carret_ = 0;

    return count;
}



}  // namespace apl

