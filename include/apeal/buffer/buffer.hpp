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


template <class T, class AllocatorT = std::allocator<T>>
class Buffer: public impl::BufferBase<T, 0, AllocatorT> {
    using Base = impl::BufferBase<T>;

    using Self = Buffer<T>;
    using Ptr = T*;

public:
    using Arg = const Self&;


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
    //using Index = Base::Index;

    using typename Base::usize_type;
    using typename Base::ssize_type;



    //
    // Common methods
    //
    using Base::empty;
    using Base::size;
    using Base::ssize;
    using Base::operator bool;

    using Base::get;
    using Base::data;
    using Base::begin;
    using Base::end;
    using Base::cbegin;
    using Base::cend;

    using Base::span;


private:

    using typename Base::Allocator;

    //
    // Common data
    //
    using Base::buffer_;
    using Base::count_;
    using Base::empty_;
    using Base::allocatedSize_;
    using Base::allocator_;

    using Base::freeBufferData;
    using Base::allocBufferData;
    using Base::createBufferObjects;
    using Base::createBufferObjectsCopy;
    using Base::destroyBufferObjects;



private:
    constexpr static Index defaultAllocChunkSize{10};  // by default start with 4K buffers
    Index allocChunkSize_{defaultAllocChunkSize};
    //Allocator allocator_{};
    bool bufferAllocated_{};


public:
    //
    // Instantiation
    //
    constexpr Buffer() noexcept {}

    Buffer(Index size, Index allocChunkSize = defaultAllocChunkSize)
    {
        allocChunkSize_ = allocChunkSize;
        if (size)
            alloc(size);
    }

    Buffer(T* bufferPtr, Index bufferSize, Index count) noexcept
    {
        manage(bufferPtr, bufferSize, count);
    }

    // copy constructor/assignment
    Buffer(const Self& other) { copy(other); }

    auto& operator=(const Self& other)
    {
        copy(other);
        return *this;
    }

    // move constructor/assignment
    Buffer(Self&& other) noexcept { swap(other); }

    auto& operator=(Self&& other) noexcept {
        swap(other);
        return *this;
    }

    ~Buffer() noexcept
    {
        try {
            freeBuffer();
        } catch (...) {
            ADebugTrap();
        }
    }


private:

    void freeBuffer() {
        if (count_)
            destroyBufferObjects(buffer_, count_);

        if (bufferAllocated_)
            freeBufferData(buffer_, allocatedSize_);

        buffer_ = 0;
        bufferAllocated_ = false;
        count_ = 0;
        allocatedSize_ = 0;
        empty_ = true;
    }



    // Reserves buffer memory.
    // Moves the objects if necessary
    Ptr expandBufferData(Ptr buffer, Index bufferSize, bool& bufferAllocated, Index allocatedSize, Index newSize)
    {
        ADebugTrapAssert(newSize > allocatedSize);

        if (bufferSize == 0) {
            auto b = allocBufferData(newSize);
            bufferAllocated = true;
            return b;
        }

        auto b = allocBufferData(newSize);

        if (bufferSize) {
            moveBuffer(buffer, bufferSize, b);
        }

        if (bufferAllocated && allocatedSize)
            freeBufferData(buffer, allocatedSize);

        bufferAllocated = true;

        return b;
    }

    Ptr shrinkBufferData(Ptr buffer, Index bufferSize, bool& bufferAllocated, Index allocatedSize, Index newSize)
    {
        if (newSize == allocatedSize)
            return buffer;

        if (newSize == 0) {
            if (bufferSize)
                destroyBufferObjects(buffer, bufferSize);
            freeBufferData(buffer, bufferSize);
            bufferAllocated = false;
            return 0;
        }


        if (newSize < bufferSize) {
            destroyBufferObjects(buffer + newSize, bufferSize - newSize);
        }

        auto b = allocBufferData(newSize);

        if (bufferSize)
            moveBuffer(buffer, std::min(bufferSize, newSize), b);

        if (bufferAllocated && allocatedSize)
            freeBufferData(buffer, allocatedSize);

        bufferAllocated = true;

        return b;
    }

    void moveBuffer(Ptr from, Index count, Ptr to) noexcept
    {
        if constexpr (std::is_trivially_copyable_v<T>)
            // works only on trivially copyable types, need to check
            std::memcpy(to, from, count * sizeof(T));
        else
            std::uninitialized_move_n(from, count, to);
    }

    void copyBuffer(const Ptr from, Index count, Ptr to) noexcept
    {
        if constexpr (std::is_trivially_copyable_v<T>)
            // works only on trivially copyable types, need to check
            std::memcpy(to, from, count * sizeof(T));
        else
            std::uninitialized_copy_n(from, count, to);
    }

public:
    void copy(const Self& b) noexcept
    {
        if (this == std::addressof(b))
            return;

        if (b.count_ == 0) {
            release();
            return;
        }

        // Create the exact copy, including sizes of allocated buffers
        clear();

        if (b.count_ > allocatedSize_)
            buffer_ = expandBufferData(buffer_, count_, bufferAllocated_, allocatedSize_, b.count_);
        else if (b.count_ < allocatedSize_) {
            buffer_ = shrinkBufferData(buffer_, count_, bufferAllocated_, allocatedSize_, b.count_);
        }

        copyBuffer(b.buffer_, b.count_, buffer_);
        allocatedSize_ = b.count_;
        count_ = b.count_;
        allocChunkSize_ = b.allocChunkSize_;
        empty_ = count_ == 0;
    }

    void swap(Self& other) noexcept
    {
        if (this == std::addressof(other))
            return;

        Base::swapContents(&other);


        //std::swap(buffer_, other.buffer_);
        //std::swap(count_, other.count_);
        //
        //auto empty = other.empty_.get();
        //other.empty_.set(empty_.get());
        //empty_.set(empty);
        ////std::swap(empty_, other.empty_);

        //std::swap(bufferAllocated_, other.bufferAllocated_);
        //std::swap(allocatedSize_, other.allocatedSize_);
        std::swap(allocChunkSize_, other.allocChunkSize_);
        //std::swap(allocator_, other.allocator_);
    }

    void move(Self&& other) noexcept { swap(std::move(other)); }

    void assign(const T* from, Index newSize)
    {
        clear();  // Clear all the existing objects
        if (newSize > allocatedSize_) {
            buffer_ = expandBufferData(buffer_, count_, bufferAllocated_, allocatedSize_, newSize);
            allocatedSize_ = newSize;
        }
        copyBuffer(from, newSize, buffer_);
        count_ = newSize;
        empty_ = count_ == 0;
    }

    // Assigns managed buffer, this class must not free the buffer
    void manage(Ptr data, Index managedMemorySize, Index elementsCount)
    {
        freeBuffer();
        buffer_ = data;
        count_ = elementsCount;
        empty_ = count_ == 0;
        allocatedSize_ = managedMemorySize;
        bufferAllocated_ = false;
    }

    //
    // Setup
    //
    void setAllocChunkSize(Index size) noexcept { allocChunkSize_ = size; }



    //
    // Information
    //
    //constexpr auto empty() const noexcept { return !count_; }
    //constexpr explicit operator bool() const noexcept { return count_ != 0; }

    //constexpr auto size() const noexcept { return (usize_type)count_; }
    //constexpr auto ssize() const noexcept { return (ssize_type)count_;  }
    //constexpr auto count() const noexcept { return (int)count_; }

    constexpr auto capacity() const noexcept { return (usize_type)allocatedSize_; }
    constexpr auto scapacity() const noexcept { return (ssize_type)allocatedSize_; }



    //
    // Accessing
    //
public:  // accessing
    //
    // Allocation, resizing
    //

    // Allocates required memory
    T* reserve(Index newSize) {

        if (newSize <= allocatedSize_)
            return buffer_;
        buffer_ = expandBufferData(buffer_, count_, bufferAllocated_, allocatedSize_, newSize);
        allocatedSize_ = newSize;
        return buffer_;
    }

    T* alloc(Index size, bool zeroMemory = false) {

        if (size <= allocatedSize_) {
            if (size <= count_)
                return buffer_;
            createBufferObjects(buffer_ + count_, size - count_);
            count_ = size;
            return buffer_;
        }

        buffer_ = expandBufferData(buffer_, count_, bufferAllocated_, allocatedSize_, size);
        createBufferObjects(buffer_ + count_, size - count_);
        count_ = size;
        empty_ = count_ == 0;
        allocatedSize_ = size;

        if (zeroMemory)
            zero();
        return buffer_;
    }


    T* resize(Index newSize) noexcept
    {
        if (newSize == count_)
            return buffer_;

        // Just call the destructors if any
        if (newSize < count_) {
            destroyBufferObjects(buffer_ + newSize, count_ - newSize);
            return buffer_;
        }

        // Expanding
        if (newSize > allocatedSize_) {
            buffer_ = expandBufferData(
                buffer_, count_, bufferAllocated_, allocatedSize_, newSize + allocChunkSize_);
            allocatedSize_ = newSize + allocChunkSize_;
            allocChunkSize_ *= 2;
        }

        createBufferObjects(buffer_ + count_, newSize - count_);

        count_ = newSize;
        empty_ = count_ == 0;
        return buffer_;
    }


    void shrink_to_fit() noexcept
    {
        if (count_ == 0) {
            freeBuffer();  // free all the memory
            return;
        }

        if (allocatedSize_ > count_) {
            buffer_ = shrinkBufferData(buffer_, count_, bufferAllocated_, allocatedSize_, count_);
            allocatedSize_ = count_;
        }
    }


    //
    // Accessing
    //
    constexpr auto& back() noexcept {
        ADebugTrapAssert(count_);
        return buffer_[count_ - 1];
    }
    constexpr const auto& back() const noexcept {
        ADebugTrapAssert(count_);
        return buffer_[count_ - 1];
    }

    constexpr auto& front() noexcept {
        ADebugTrapAssert(count_);
        return buffer_[0];
    }

    constexpr const auto& front() const noexcept {
        ADebugTrapAssert(count_);
        return buffer_[0];
    }


    void release() noexcept { freeBuffer(); }

    void clear() noexcept
    {
        // Just call the destructors
        // No memory gets freed
        if (count_) {
            destroyBufferObjects(buffer_, count_);
            count_ = 0;
            empty_ = true;
        }
    }

    void greedyBufferExpand(Index total) {
        if (count_ + total > allocatedSize_) {
            reserve(count_ + total + allocChunkSize_);
            allocChunkSize_ *= 2;
        }
    }

    void push_back(const T& v)
    {
        auto total = 1;
        greedyBufferExpand(total);
        createBufferObjectsCopy(buffer_ + count_, total, &v);  //std::addressof(v));
        count_ = count_ + total;
        empty_ = count_ == 0;
    }

    void push_back(T&& v)
    {
        auto total = 1;
        greedyBufferExpand(total);
        std::uninitialized_move_n(&v, total, buffer_ + count_);
        //buffer_[count_] = std::move(v) ;
        count_ = count_ + total;
        empty_ = count_ == 0;
    }

    // TODO: implement the proper way
    reference emplace_back()
    {
        auto total = 1;
        greedyBufferExpand(total);
        createBufferObjects(buffer_ + count_, total);
        count_ = count_ + total;
        empty_ = count_ == 0;
        return back();
    }

    auto erase(const_iterator it) noexcept { return erase(it, it + 1); }

    auto erase(const_iterator it, const_iterator end) noexcept
    {
        auto beginIndex = it - buffer_;
        auto eraseEnd = end;

        iterator eraseBegin = buffer_ + beginIndex;
        iterator bufferEnd = buffer_ + count_;
        auto newCount = std::distance(buffer_, it);  //size_t(it - buffer_);
        auto ret = iterator{buffer_};

        for (;;) {
            if (eraseEnd >= bufferEnd) {
                count_ = eraseBegin - buffer_;
                ret = eraseBegin;
                break;
            }

            newCount++;
            *eraseBegin++ = *eraseEnd++;
        }

        empty_ = count_ == 0;

        return ret;
    }


    // calculate minimum and maximum values
    void minmax(T& minval, T& maxval)
    {
        T* p = buffer_;
        minval = maxval = *p++;

        auto n = count_;

        for (auto i = 1; i < n; ++i, ++p) {
            if (*p < minval)
                minval = *p;
            if (*p > maxval)
                maxval = *p;
        }
    }

    // TODO: redo like max()
    // TODO: move to "scalar buffer" maybe
    const T& max() const
    {
        auto p = buffer_;
        auto n = count_;
        Index imax{};
        ++p;
        for (Index i = 1; i < n; ++i, ++p) {
            if (*p > buffer_[imax])
                imax = i;
        }

        return buffer_[imax];
    }

    const T min() const noexcept
    {
        auto p = buffer_;
        auto n = count_;
        T m = *p;
        ++p;
        for (int i = 1; i < n; ++i, ++p) {
            if (*p < m)
                m = *p;
        }

        return m;
    }

    void zero() noexcept
    {
        if (!count_)
            return;

        if constexpr (std::is_scalar<T>::value || std::is_trivially_constructible<T>::value) {
            //if ( std::is_trivially_constructible<T>::value && std::is_trivially_destructible<T>::value ) {
            memset(buffer_, 0, sizeof(T) * count_);
        }
        //} else {
        //  auto p = buffer_ ;
        //  for ( auto i = 0 ; i < count_ ; ++i, ++p )
        //      *p = 0 ;
        //}
    }

    void fill(const T& v) noexcept
    {
        auto p = buffer_;
        for (auto i = 0; i < count_; ++i, ++p)
            *p = v;
    }



    //
    // Operator overloading
    //

    constexpr auto& operator*() const noexcept { return *buffer_; }
    constexpr auto* operator->() const noexcept { return buffer_; }

    constexpr auto& operator[](Index idx) noexcept
    {
        ADebugTrapAssert(idx >= 0 && idx < count_);
        return buffer_[idx];
    }

    constexpr const auto& operator[](Index idx) const noexcept
    {
        ADebugTrapAssert(idx >= 0 && idx < count_);
        return buffer_[idx];
    }
};


}  // namespace apl
