/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

//
// Detect all debugging modes
//
// If either DEBUG or _DEBUG defined have them both defined
//
#ifdef DEBUG  // xcode defines DEBUG
    // Check if it has value
    #if ~(~DEBUG + 0) == 0 && ~(~DEBUG + 1) == 1
        #undef DEBUG
        #define DEBUG 1
    #endif

    #ifndef _DEBUG
        #define _DEBUG 1
    #endif
#endif  // if DEBUG

#ifdef _DEBUG // Visual Studio defines _DEBUG
    // Check if it has value
    #if ~(~_DEBUG + 0) == 0 && ~(~_DEBUG + 1) == 1
        #undef _DEBUG
        #define _DEBUG 1
    #endif
#endif  // if _DEBUG

