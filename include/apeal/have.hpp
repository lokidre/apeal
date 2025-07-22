/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

// global "have" definitions

//
// have secured versions of POSIX functions
// #define __HAVE_SECURE_POSIX
//
#if _MSC_VER >= 1400
//#define __HAVE_SECURE_POSIX   1
#endif


//
// have ISO C++ equivalents of POSIX functions
// #define __HAVE_ISO_POSIX
//
//#if _MSC_VER >= 1400
//#define __HAVE_ISO_POSIX      1
//
//#define __APEAL_HAVE_MBSTOWCS_S  1
//#define __APEAL_HAVE_WCSTOMBS_S  1
//#define __APEAL_HAVE_WCSCPY_S    1
//#define __APEAL_HAVE_WCSTOMBS_S  1
//#define __APEAL_HAVE_WCSCAT_S    1
//#endif


//#ifdef __HAVE_SECURE_POSIX
//#define __HAVE_SPRINTF_S      1
//#define __HAVE_SSCANF_S       1
//#define __HAVE_STRERROR_S     1
//#define __HAVE_STRCPY_S       1
//#define __HAVE_STRNCPY_S      1
//#define __HAVE_FOPEN_S        1
//#define __HAVE_GETENV_S       1
//#endif
//
//
//#ifdef __HAVE_ISO_POSIX
//#define __HAVE__WRITE         1
//#define __HAVE__READ          1
//#define __HAVE__OPEN          1
//#define __HAVE__WOPEN         1
//#define __HAVE__STRICMP       1
//#endif
