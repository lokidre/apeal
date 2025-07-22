/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/debug.hpp"
#include "../include/apeal/string.hpp"

//#include <iostream>
#include <fcntl.h>
#if APEAL_OS_WINDOWS
#include <io.h>
#endif
#include <cstdio>

namespace apl {

#if APEAL_OS_WINDOWS

void debug_windows_print_console(FILE* output, std::string_view msg) noexcept {
    static auto isTerminal = [&] {
        auto cwnd = GetConsoleWindow();
        auto currPID = GetCurrentProcessId();
        DWORD wndPID{};
        GetWindowThreadProcessId(cwnd, &wndPID);
        return currPID == wndPID;
    }();

    static auto hasTerminal = _isatty(_fileno(output));

    if (IsDebuggerPresent() && !hasTerminal /*&& !isTerminal*/) {
        //ApiString sapi{msg};
        OutputDebugStringW(ApiString{msg}.win());
        OutputDebugStringW(L"\n");
    } else {
        fwrite(msg.data(), sizeof(msg.front()), msg.size(), output);
        fputc('\n', output);
    }
}
#endif


#if APEAL_OS_WINDOWS

void debug_windows_print_console_prefix(FILE* output, std::string_view prefix, std::string_view msg) noexcept {
    if (IsDebuggerPresent()) {
        ApiString apiPrefix{prefix}, apiMsg{msg};
        OutputDebugStringW(apiPrefix.utf16());
        OutputDebugStringW(apiMsg.utf16());
        OutputDebugStringW(L"\n");
    }
}
#endif

} // namespace


#if 0
#if APEAL_OS_WINDOWS

#include "../include/apeal/string.hpp"


//#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <cstdio>

namespace apl {


/*
void __debug__print_ide_console ( const char *file, int line, const char *fmt, ... )
{
    std::string s ;

    char cs[4096] ;

    va_list ap ;
    va_start ( ap, fmt ) ;
    str_format ( s, "%s(%d): ", file, line ) ;
    vsnprintf_s ( cs, sizeof(cs), fmt, ap ) ;
    s += cs ;
    va_end ( ap ) ;

    OutputDebugString ( to_api_string(s).c_str() ) ;
}
*/

void __debug__windows_console_create()
{
    static bool console_created_ = false ;

    if ( console_created_ )
        return ;

    FreeConsole() ;

    AllocConsole() ;
    AttachConsole( ATTACH_PARENT_PROCESS);

    CONSOLE_SCREEN_BUFFER_INFO console_info ;
    GetConsoleScreenBufferInfo ( GetStdHandle(STD_OUTPUT_HANDLE), &console_info ) ;

    console_info.dwSize.Y = 200 ;

    SetConsoleScreenBufferSize ( GetStdHandle(STD_OUTPUT_HANDLE), console_info.dwSize ) ;

#pragma warning (disable:4311)

    int stdoutHandle = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT) ;
    int stdinHandle = _open_osfhandle((intptr_t)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT) ;
    int stderrHandle = _open_osfhandle((intptr_t)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT) ;

    FILE *stdout_file = _fdopen( stdoutHandle, "w" ) ;
    FILE *stdin_file = _fdopen( stdinHandle, "r" ) ;
    FILE *stderr_file = _fdopen( stderrHandle, "w" );

    *stdout = *stdout_file ;
    *stdin = *stdin_file ;
    *stderr = *stderr_file ;

    setvbuf( stdout, 0, _IONBF, 0 );
    setvbuf( stdin, 0, _IONBF, 0 );
    setvbuf ( stderr, 0, _IONBF, 0 ) ;


    FILE *dummyStream ;
    freopen_s ( &dummyStream, "CONOUT$", "w", stdout);
    freopen_s ( &dummyStream, "CONOUT$", "w", stderr);

    std::ios_base::sync_with_stdio() ;

    console_created_ = true ;
}

void __debug__print_console ( const char *file, int line, const char *fmt, ... )
{
    va_list ap ;
    va_start ( ap, fmt ) ;

    if ( IsDebuggerPresent() ) {
        char s[4096], cs[4096] ;
        int i = (int)strlen(file) ;
        while ( i > 0 ) {
            if ( file[i-1] == '/' || file[i-1] == '\\' )
                break ;
            --i ;
        }
        sprintf_s ( s, sizeof(s), "%s(%d):", file + i, line ) ;
        vsnprintf_s ( cs, sizeof(cs), fmt, ap ) ;
        strcat_s ( s, 4095, cs ) ;
        OutputDebugString(s) ;
        OutputDebugString("\n") ;
    } else {
        __debug__windows_console_create() ;
        vfprintf ( stdout, fmt, ap ) ;
        fprintf ( stdout, "\n" ) ;
        //fprintf(stdout,fmt,__VA_ARGS__) ;
    }

    va_end ( ap ) ;
}







} // namespace

#endif
#endif  // comment out

