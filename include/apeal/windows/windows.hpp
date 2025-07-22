/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../def.hpp"

#if APEAL_OS_WINDOWS

// Include windows.h only if it wasn't already included
#ifndef _WINDOWS_

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#ifndef WINVER
#define WINVER _WIN32_WINNT_WIN10
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN10
#endif

#ifndef STRICT
#define STRICT
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#pragma warning(push)
#pragma warning(disable:5105)  // Some preprocessor warning when using /Zc:preprocessor
#include <windows.h>
#pragma warning(pop)

#endif  // Check if windows.h was already included
#endif  // if APEAL_OS_WINDOWS
