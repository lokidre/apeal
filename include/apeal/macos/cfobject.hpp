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

#include <CoreFoundation/CoreFoundation.h>

#ifdef __OBJC__
#include <Foundation/Foundation.h>
#endif

namespace apl {


template <class T>
class CFObject {
public:
    CFObject() {}
    CFObject(T* p) : ptr_(p) {}
    ~CFObject() { release(); }

    void release() {
        if (ptr_) {
            CFRelease(ptr_);
            ptr_ = nullptr;
        }
    }

    T* operator()() { return ptr_; }

private:
    T* ptr_{};
};

// Specialization for pointer
template <class T>
class CFObject<T*> {
public:
    CFObject() {}
    CFObject(T p) : ptr_(p) {}
    ~CFObject() { release(); }

    void release() {
        if (ptr_) {
            CFRelease(ptr_);
            ptr_ = nullptr;
        }
    }

    T operator()() { return ptr_; }

private:
    T ptr_{};
};

} // namespace apl
