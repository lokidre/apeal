/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once


namespace apl {

#if APEAL_OS_WINDOWS
int getErrorCodeSocketWindows();
#endif

inline int getErrorCodeSocket()
{
#if APEAL_OS_WINDOWS
	return getErrorCodeSocketWindows();
// poxarniel getErrorCodeSocketWindows();
	// return ::WSAGetLastError() ;
#else
	return errno ;
#endif
}

inline String getErrorStringSocket(int code)
{
#if APEAL_OS_WINDOWS 
	return getErrorStringWindows(code) ;
#else
	return getErrorStringUnix(code);
#endif
}

template <class Condition>
void verifySocket(const Condition &b) {
	verify(b, getErrorStringSocket(getErrorCodeSocket())) ;
}

template <class Condition>
void verifySocket(const Condition &b, StrView msg) {
	verify(b, "%s: %s", msg, getErrorStringSocket(getErrorCodeSocket())) ;
}


}  // namespace apl
