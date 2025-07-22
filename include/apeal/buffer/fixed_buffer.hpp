/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "./buffer_base.hpp"

namespace apl {

template <class T, int N>
class FixedBuffer: public impl::BufferBase<T> {
    using Base = impl::BufferBase<T>;
    using Self = FixedBuffer<T, N>;

public:
    using Arg = const FixedBuffer<T, N>&;

    //
    // Standard types
    //
    using typename Base::value_type;
    using typename Base::difference_type;
    using typename Base::reference;
    using typename Base::pointer;


    using typename Base::iterator;
    using typename Base::const_iterator;
    using typename Base::reverse_iterator;
    using typename Base::const_reverse_iterator;


    //
    // Custom additional types
    //
    using typename Base::Index;
    using typename Base::usize_type;
    using typename Base::ssize_type;

    //
    // Common data
    //
    using Base::buffer_;
    using Base::count_;
    using Base::allocatedSize_;


    //
    // Common methods
    //
    using Base::empty;
    using Base::size;

    using Base::get;
    using Base::data;
    using Base::begin;
    using Base::end;
    using Base::cbegin;
    using Base::cend;

    using Base::span;


    //using allocator_type = std::allocator<void> ;



private:  // Data
    uint8_t dataD_[N * sizeof(T)];  // Just a memory buffer

    //T* buffer_{(T*)dataD_};

    //Index count_{};
    //const Index allocatedSize_{N};

    //
    // Construction and initialization
    //
private:
    constexpr void constr() noexcept {
        buffer_ = (T*)dataD_;
        allocatedSize_ = N;
    }

public:
    constexpr FixedBuffer() noexcept {
        constr();
    }

    // Copy constructor
    FixedBuffer(Arg other) noexcept { 
        constr();
        copy(other); 
    }

    // move constructor
    FixedBuffer(Self&& other) noexcept {
        constr();

        if (count_)
            destroyBufferObjects(buffer_, count_);

        if (other.count_)
            std::uninitialized_move_n(other.buffer_, other.count_, buffer_);

        count_ = other.count_;

        other.count_ = 0;
    }

    // Destructor
    ~FixedBuffer() noexcept { release(); }

    // Assignment
    Self& operator = (Arg other) noexcept { 
        copy(other);  
        return *this; 
    }


    //
    // Information
    //

    constexpr Index capacity() const noexcept { return N; }
    constexpr Index max_size() const noexcept { return N; }


    bool full() const noexcept { return count_ == N; }


    //
    // Accessing
    //
public:  // accessing


    // Allocates required memory
    void reserve(Index) noexcept {}  // No need
    void shrink_to_fit() noexcept {}  // nothing to do

    void resize(Index newSize) {
        if (newSize == count_)
            return;
        verify(newSize <= allocatedSize_, "Cannot expand fixed buffer");

        if (newSize < count_) {
            destroyBufferObjects(buffer_ + newSize, count_ - newSize);
            return;
        }

        createBufferObjects(buffer_ + count_, newSize - count_);

        count_ = newSize;
    }

    //
    // Accessing
    //
    constexpr T& back() noexcept {
        ADebugTrapAssert(count_);
        return buffer_[count_ - 1];
    }
    constexpr const T& back() const noexcept {
        ADebugTrapAssert(count_);
        return buffer_[count_ - 1];
    }

    constexpr T& front() noexcept {
        ADebugTrapAssert(count_);
        return buffer_[0];
    }
    constexpr const T& front() const noexcept {
        ADebugTrapAssert(count_);
        return buffer_[0];
    }


    void clear() noexcept {
        // Just call the destructors
        // No memory gets freed
        if (count_) {
            destroyBufferObjects(buffer_, count_);
            count_ = 0;
        }
    }

    void release() noexcept { clear(); }

    //
    // Modification

    void push_back(const T& v) {
        const auto total = 1;

        // Allocate more memory if necessary
        verify(count_ + total <= allocatedSize_, "Cannot expand fixed buffer");

        // We need to use copy constructor
        createBufferObjectsCopy(buffer_ + count_, total, std::addressof(v));
        count_ = count_ + total;
    }

    // TODO: implement the proper way
    reference emplace_back() {
        resize(size() + 1);
        return back();
    }

    iterator erase(const_iterator it) noexcept { 
        return erase(it, it + 1); 
    }

    iterator erase(const_iterator it, const_iterator end) noexcept
    {
        auto beginIndex = it - buffer_;
        auto eraseEnd = end;

        auto eraseBegin = buffer_ + beginIndex;
        auto bufferEnd = buffer_ + count_;
        auto newCount = Index(it - buffer_);
        auto ret = buffer_;

        for (;;) {
            if (eraseEnd >= bufferEnd) {
                count_ = eraseBegin - buffer_;
                ret = eraseBegin;
                break;
            }

            newCount++;
            *eraseBegin++ = *eraseEnd++;
        }

        return ret;
    }

    void pop_back() noexcept {
        ADebugTrapAssert(count_);
        erase(begin() + count_ - 1);
    }

    //
    // Operator overloading
    //

    constexpr explicit operator bool() const noexcept { return count_; }

    constexpr T& operator*() const noexcept { return *buffer_; }
    constexpr T* operator->() const noexcept { return buffer_; }

    constexpr T& operator[](Index idx) noexcept {
        ADebugTrapAssert(idx < count_);
        return buffer_[idx];
    }

    constexpr const T& operator[](Index idx) const {
        ADebugTrapAssert(idx < count_);
        return buffer_[idx];
    }


    void zero() noexcept {
        if (!count_)
            return;
        if (std::is_scalar<T>::value || std::is_trivially_constructible<T>::value) {
            memset(buffer_, 0, sizeof(T) * count_);
        }
    }

    void fill(const T& v) noexcept {
        auto p = buffer_;
        for (auto i = 0; i < count_; ++i, ++p)
            *p = v;
    }


private:

    void createBufferObjects(T* buffer, Index count) noexcept {
        std::uninitialized_default_construct_n(buffer, count);
    }

    void createBufferObjectsCopy(T* buffer, Index count, const T* ini) noexcept {
        auto p = buffer;
        if (!std::is_trivially_constructible<T>::value) {
            for (auto i = 0; i < count; ++i, ++p, ++ini)
                //Allocator::construct ( allocator_, p, *ini ) ;
                new (p)T(*ini);
        } else {
            for (auto i = 0; i < count; ++i, ++p, ++ini)
                *p = *ini;
        }
    }

    void destroyBufferObjects(T* buffer, Index count) noexcept {
        std::destroy(buffer, buffer + count);
    }

    void moveBuffer(const T* from, Index count, T* to) noexcept {
        if (std::is_trivially_copyable<T>::value)
            // works only on trivialy copyable types, need to check
            std::memcpy(to, from, count * sizeof(T));
        else
            std::uninitialized_move_n(from, count, to);
    }

    void copyBuffer(const T* from, Index count, T* to) noexcept {
        if (std::is_trivially_copyable<T>::value)
            // works only on trivialy copyable types, need to check
            std::memcpy(to, from, count * sizeof(T));
        else
            std::uninitialized_copy_n(from, count, to);
    }

    void copy(Arg b) noexcept {
        if (b.count_ == 0) {
            release();
            return;
        }

        // Create the exact copy, including sizes of allocated buffers
        clear();

        copyBuffer(b.buffer_, b.count_, buffer_);
        count_ = b.count_;
    }

    void assign(const T* from, Index newSize) noexcept {
        clear();  // Clear all the existing objects
        copyBuffer(from, newSize, buffer_);
        count_ = newSize;
    }


};

}  // namespace apl
