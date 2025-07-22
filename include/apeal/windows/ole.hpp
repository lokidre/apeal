/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once


#if APEAL_OS_WINDOWS
#include <oleauto.h>
#endif


#include <string>

#pragma comment (lib, "oleaut32.lib")



#include "ole_bstr.hpp"
#include "ole_idispatch.hpp"
#include "ole_iunknown.hpp"
//#include "../variant.hpp"
#include "com.hpp"
