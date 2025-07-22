/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <cstdint>
#include "assert.hpp"
#include "buffer/buffer.hpp"
#include "buffer/fixed_buffer.hpp"
#include "buffer/pool_buffer.hpp"
#include "buffer/ring_buffer.hpp"
#include "buffer/linked_ring_buffer.hpp"


namespace apl {

template <class T, class A>
void swap(Buffer<T, A>& a, Buffer<T, A>& b) noexcept {
    a.swap(b);
}


using ByteBuffer = Buffer<std::uint8_t>;


} // namespace apl
