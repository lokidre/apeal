/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif


//
// Windows and everything related to it
//
#if defined(_Windows) || defined(_WINDOWS) || defined(_WIN32)
#define APEAL_OS_WINDOWS 1
#define TARGET_OS_WINDOWS 1
#define ATARGET_OS_WINDOWS 1
#endif


//
// Mac, iOS, MacOS
//
#if TARGET_OS_MAC
#define APEAL_OS_MAC 1
#define APEAL_OS_POSIX 1
#define TARGET_OS_POSIX 1
#define ATARGET_OS_POSIX 1
#endif

#if TARGET_OS_IPHONE
#define APEAL_OS_IPHONE 1
#define APEAL_OS_POSIX 1
#define TARGET_OS_POSIX 1
#define ATARGET_OS_POSIX 1
#endif

#if APEAL_OS_MAC

#if TARGET_OS_IPHONE
#else // If MacOS but not iOS
#define TARGET_OS_MAC_DESKTOP 1
#define APEAL_OS_MAC_DESKTOP 1
#endif
#endif


//
// Unix in General
//

#ifndef APEAL_OS_POSIX
#if defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION)
#define APEAL_OS_POSIX 1
#define TARGET_OS_POSIX 1
#define ATARGET_OS_POSIX 1
#endif
#endif

#ifndef APEAL_OS_UNIX
#if !defined(APEAL_OS_MAC) && defined(APEAL_OS_POSIX)
#define APEAL_OS_UNIX 1
#define TARGET_OS_UNIX 1
#define ATARGET_OS_UNIX 1
#endif
#endif

#ifndef APEAL_OS_LINUX
#if defined(__linux__)
#define APEAL_OS_LINUX 1
#define TARGET_OS_LINUX 1
#endif
#endif



//
// XAOS
//
#if APEAL_OS_XAOS
#if HOST_OS_WINDOWS
#define WINVER 0x0500
#define NOMINMAX
#include <winresrc.h>
#include <windows.h>
#elif HOST_OS_UNIX
#include <memory.h>
#endif
#endif

#if APEAL_OS_XAOS
#include "xaos/xsapi.h"
#endif



//
// Android
//
#if defined(ANDROID) && !defined(TARGET_OS_ANDROID)
#define APEAL_OS_ANDROID 1
#define TARGET_OS_ANDROID 1
#define ATARGET_OS_ANDROID 1
#endif



//
// Desktop/Phone
//
#if defined(APEAL_OS_ANDROID) || defined(APEAL_OS_IPHONE)
#define APEAL_OS_PHONE 1
#else
#define TARGET_OS_DESKTOP 1
#define APEAL_OS_DESKTOP 1
#endif

