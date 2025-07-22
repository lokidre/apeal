/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../noncopyable.hpp"

namespace apl {

template <class UnknownType>
class OleIUnknown: public NonCopyable {
public :
    UnknownType* dispatch{};

    ~OleIUnknown() noexcept {
        release();
    }

    void release() noexcept
    {
        if (dispatch ) {
            dispatch->Release();
            dispatch = nullptr;
        }
    }
};


}  // namespace apl
