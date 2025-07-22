/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/error.hpp"

#if APEAL_OS_WINDOWS
#include <Winsock2.h>
#pragma comment (lib,"Ws2_32.lib")
#endif


namespace apl {

#if APEAL_OS_WINDOWS

String getErrorStringWindows(DWORD errorCode){

    const DWORD bufferSize = 1024;
    wchar_t buffer[bufferSize];

    auto res = ::FormatMessageW(
        FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, 0,
        errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer,
        bufferSize - 1, 0);

    if (res == 0)
        return sformat("Unknown Windows error: %d (%X)", errorCode, errorCode);

    auto str = apiFromUtf16(buffer);

    // Lose the newline
    while (!str.empty() && (str.back() == '\r' || str.back() == '\n'))
        str.pop_back();

    return str;
}

int getErrorCodeSocketWindows() {
 return ::WSAGetLastError() ;
}

#endif




} // namespace


