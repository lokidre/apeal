/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <atomic>


namespace apl {

namespace impl {

template <class T, std::size_t N = 0, class AllocatorT = std::allocator<T>>
class BufferBase {
public:
    using Index = std::ptrdiff_t;


    using value_type = T;
    using difference_type = ptrdiff_t;
    using reference = T&;
    using pointer = T*;

    using usize_type = std::size_t;
    using ssize_type = std::ptrdiff_t;

    using size_type = std::size_t;


    using iterator = T*;
    using const_iterator = T const*;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

protected:
    using Allocator = AllocatorT;



protected:
    T* buffer_{};  // The actual data buffer
    Index count_{};  // Element count
    std::atomic_bool empty_{true};
    Index allocatedSize_{};  // Count of the actual allocated items
    Allocator allocator_{};


    T* freeBufferData(T* buffer, Index count) noexcept { 
        allocator_.deallocate(buffer, count); 
        return nullptr;
    }

    T* allocBufferData(Index count) {
        auto b = allocator_.allocate(count);
        ADebugTrapAssert(b);
        verify(b, "Buffer::Not enough memory::Allocating: %d", count * sizeof(T));
        return b;
    }


    void createBufferObjects(T* buffer, Index count) {
        std::uninitialized_default_construct_n(buffer, count);
    }


    void createBufferObjectsCopy(T* buffer, Index count, const T* ini) {
        auto p = buffer;
        if constexpr (std::is_trivially_constructible_v<T>) {
            for (Index i = 0; i < count; ++i, ++p, ++ini)
                *p = *ini;
        } else {
            for (Index i = 0; i < count; ++i, ++p, ++ini)
                new (p) T(*ini);
        }
    }

    void destroyBufferObjects(T* buffer, Index count) noexcept { 
        std::destroy(buffer, buffer + count); 
    }

    void destroyBuffer(T* buffer, Index count) noexcept {
        destroyBufferObjects(buffer, count);
        freeBufferData(buffer, count);
    }


    void swapContents(BufferBase* o) noexcept {
        std::swap(buffer_, o->buffer_);

        std::swap(count_, o->count_);

        auto temp = o->empty_.load();
        o->empty_ = empty_.load();
        empty_ = temp;


        std::swap(allocatedSize_, o->allocatedSize_);
        std::swap(allocator_, o->allocator_);
    }


public:
    //
    // Information inquiry
    //
    constexpr int count() const noexcept { return (int)count_; }
    constexpr int icount() const noexcept { return (int)count_; }

    constexpr std::size_t size() const noexcept { return (std::size_t)count_; }
    constexpr Index ssize() const noexcept { return count_; }
    //constexpr bool empty() const noexcept { return count_ == 0; }
    //constexpr explicit operator bool() const noexcept { return count_ != 0; }
    constexpr bool empty() const noexcept { return empty_; }
    constexpr explicit operator bool() const noexcept { return !empty_; }


    //
    // Data access and iterators
    //
    constexpr T* get() noexcept { return buffer_; }  // access for modification
    constexpr const T* get() const noexcept { return buffer_; }  // const access

    constexpr T* data() noexcept { return buffer_; }
    constexpr const T* data() const noexcept { return buffer_; }

    constexpr iterator begin() noexcept { return buffer_; }
    constexpr iterator end() noexcept { return buffer_ + count_; }

    constexpr const_iterator cbegin() const noexcept { return buffer_; }
    constexpr const_iterator cend() const noexcept { return buffer_ + count_; }

    constexpr const_iterator begin() const noexcept { return cbegin(); }
    constexpr const_iterator end() const noexcept { return cend(); }

    constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(buffer_ + count_ - 1); }
    constexpr reverse_iterator rend() noexcept { return reverse_iterator(buffer_ - 1); }

    constexpr auto crbegin() const noexcept { return const_reverse_iterator(buffer_ + count_ - 1); }
    constexpr auto crend() const noexcept { return const_reverse_iterator(buffer_ - 1); }

    constexpr auto rbegin() const noexcept { return crbegin(); }
    constexpr auto rend() const noexcept { return crend(); }


    constexpr auto span() noexcept { return std::span{buffer_, size()}; }
    constexpr auto span() const noexcept { return std::span{buffer_, size()}; }


};

}  // impl namespace

}  // namespace apl
