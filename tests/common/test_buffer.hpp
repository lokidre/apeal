/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "test_main.h"

#include <apeal/buffer.hpp>

template <class T>
void dumpBufferInfo(const apl::Buffer<T>& buffer) {
    con_.print("e:%X", buffer.empty());
    con_.print(", sz:%d", buffer.size());
}


template <class T, std::size_t N>
void dumpBufferInfo(const apl::RingBuffer<T, N>& buffer) {
    con_.print("mx:%d", buffer.max_size());
    con_.print(", e:%X", buffer.empty());
    con_.print(", f:%X", buffer.full());
    con_.print(", sz:%d", buffer.size());
    con_.print(", ei:%d", buffer.endIndex());
    con_.print(", bi:%d", buffer.beginIndex());
}

template <class T>
void dumpBufferInfo(const apl::LinkedRingBuffer<T>& buffer) {
    // con_.print("mx:%d", buffer.max_size());
    // con_.print(", e:%X", buffer.empty());
    con_.print("sz:%d", buffer.size());
}

template <class T, int N>
void dumpBufferInfo(const apl::FixedBuffer<T, N>& buffer) {
    con_.print("mx:%d", buffer.max_size());
    con_.print(", e:%X", buffer.empty());
    con_.print(", f:%X", buffer.full());
    con_.print(", sz:%d", buffer.size());
}

template <class T, int N>
void dumpBufferInfo(const apl::PoolBuffer<T, N>& buffer) {
    con_.print("mx:%d", buffer.max_size());
    con_.print(", ai:%d", buffer.availIndex());
}

template <class BufferT, class V>
void fillBuffer(BufferT& buffer, const V& v) {
    for (auto& d: buffer)
        d = v;
}



template <class BufferT>
void dumpBuffer(const BufferT& buffer, const apl::StrView& comment = {})
{
    if (!comment.empty())
        con_("-- %s:", comment);

    dumpBufferInfo(buffer);
    con_.print(": [");
    
    auto first = true;

    for (auto& d: buffer) {
        if (first)
            first = false;
        else
            con_.print(", ");
        con_.print("%z", d);
    }

    con_("]");
}
