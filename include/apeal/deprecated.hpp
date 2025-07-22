/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "def.hpp"

#if defined(__GNUC__) || defined(__clang__)
#define APEAL_DEPRECATED(__func) __attribute__ ((deprecated)) __func
#elif defined(_MSC_VER)
#define APEAL_DEPRECATED(__func) __declspec(deprecated) __func
#else
#pragma message("WARNING: You need to implement APEAL_DEPRECATED for this compiler!!!")
#define APEAL_DEPRECATED(__func) __func
#endif
