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

template <class T, std::size_t N = 0, class AllocatorT = std::allocator<T>>
class RingBuffer: public impl::BufferBase<T, N, AllocatorT> {
    friend class Iterator;

    using Base = impl::BufferBase<T, N, AllocatorT>;

    using Self = RingBuffer<T, N>;
    using SelfRef = Self&;
    using SelfPtr = Self*;

public:  // Standard type names
    using Arg = const Self&;

    //
    // Standard types
    //
    using typename Base::value_type;
    using typename Base::difference_type;
    using typename Base::reference;
    using typename Base::pointer;


    //
    // Custom additional types
    //
    using typename Base::Index;
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
    using Base::span;


protected:
    using typename Base::Allocator;

    //
    // Common data
    //
    using Base::buffer_;
    using Base::allocatedSize_;
    using Base::count_;
    using Base::empty_;

    using Base::freeBufferData;
    using Base::allocBufferData;
    using Base::createBufferObjects;
    using Base::createBufferObjectsCopy;
    using Base::destroyBufferObjects;
    using Base::destroyBuffer;



private:
    constexpr static Index defaultAllocChunkSize{10};  // by default start with 4K buffers
    Index allocChunkSize_{defaultAllocChunkSize};

    // Data
    Index headIndex_{}, tailIndex_{};
    bool bufferAllocated_{};

public:
    constexpr RingBuffer() noexcept {}

    RingBuffer(std::initializer_list<T> ini) {
        for (auto it = ini.begin(), end = ini.end(); it != end; ++it)
            push(*it);
    }

    // Copy constructor/assignment
    RingBuffer(Arg a) {  copy(a);  }

    auto& operator=(Arg a) {
        copy(a);
        return *this;
    }


    // Move constructor/assignment
    RingBuffer(Self&& other) noexcept {  this->swap(other);  }

    auto& operator = (Self&& other) noexcept {
        this->swap(other);
        return *this;
    }


    ~RingBuffer() noexcept {
        destroyBuffer(buffer_, allocatedSize_);
    }


    //expand - with data preservation

    void reserve(Index ns) {
        if (ns == 0)
            ns = allocChunkSize_;

        if (ns <= allocatedSize_)
            return;

        // Preserve data
        if (empty()) {
            if (allocatedSize_) {
                destroyBuffer(buffer_, allocatedSize_);
            }


            //size_ = ns;
            //total_ = ns;
            tailIndex_ = headIndex_ = 0;

            allocatedSize_ = ns + allocChunkSize_;
            buffer_ = allocBufferData(allocatedSize_);
            createBufferObjects(buffer_, allocatedSize_);

        } else {  // Preserve the data
            auto newAllocatedSize = ns + allocChunkSize_;
            auto newBuffer = allocBufferData(newAllocatedSize);
            createBufferObjects(newBuffer, newAllocatedSize);


            if (tailIndex_ < headIndex_) {
                std::move(buffer_ + tailIndex_, buffer_ + headIndex_ - tailIndex_, newBuffer);
                headIndex_ = headIndex_ - tailIndex_;
            } else {
                auto src = buffer_ + tailIndex_;
                auto dest = std::move(src, src + allocatedSize_ - tailIndex_, newBuffer);

                if (headIndex_) {
                    src = buffer_;
                    std::move(src, src + headIndex_, dest);
                }
                //std::move(buffer_, buffer_ + headIndex_, newBuffer + total_ - tailIndex_);
                headIndex_ = (allocatedSize_ - tailIndex_) + headIndex_;
            }


            // Free the old buffer
            destroyBuffer(buffer_, allocatedSize_);
            //destroyBufferObjects(buffer_, allocatedSize_);
            //freeBufferData(buffer_, allocatedSize_);


            // TODO: BUG!! check the head index
            buffer_ = newBuffer;
            allocatedSize_ = newAllocatedSize;

            //headIndex_ = 
            tailIndex_ = 0;

            //size_ = ns;
            //total_ = ns; // + 1;
        }


        //checkAlloc();
    }

    void copy(const Self& a) {
        clear();

        if (a.empty())
            return;

        tailIndex_ = a.tailIndex_;
        headIndex_ = a.headIndex_;

        if (a.allocatedSize_ > allocatedSize_) {
            destroyBuffer(buffer_, allocatedSize_);
            //destroyBufferObjects(buffer_, allocatedSize_);
            //freeBufferData(buffer_, allocatedSize_);

            allocatedSize_ = a.allocatedSize_;
            buffer_ = allocBufferData(allocatedSize_);
            createBufferObjects(buffer_, a.ssize());
        }

        //if (a.allocatedTotal > total_)
        //    total_ = a.total_;

        //checkAlloc();

        if (full()) {
            if constexpr (std::is_trivially_copyable_v<T>)
                // works only on trivialy copyable types, need to check
                std::memcpy(buffer_, a.buffer_, a.allocatedSize_ * sizeof(T));
            else
                std::copy_n(a.buffer_, a.allocatedSize_, buffer_);
            return;
        }

        // copy only parts
        if constexpr (std::is_trivially_copyable_v<T>) {
            if (tailIndex_ < headIndex_) {
                std::memcpy(buffer_ + tailIndex_, a.buffer_ + tailIndex_, (headIndex_ - tailIndex_) * sizeof(T));
            } else {
                std::memcpy(buffer_ + tailIndex_, a.buffer_ + tailIndex_, (allocatedSize_ - tailIndex_) * sizeof(T));
                std::memcpy(buffer_, a.buffer_, headIndex_ * sizeof(T));
            }
        } else {
            if (tailIndex_ < headIndex_) {
                std::copy_n(a.buffer_ + tailIndex_, headIndex_ - tailIndex_, buffer_ + tailIndex_);
            } else {
                std::copy_n(a.buffer_ + tailIndex_, allocatedSize_ - tailIndex_, buffer_ + tailIndex_);
                std::copy_n(a.buffer_, headIndex_, buffer_);
            }
        }

        updateCount();
    }

    auto& move(Self&& other) noexcept {
        this->swap(other);
        return *this;
    }

    void swap(Self& o) noexcept {
        if (this == std::addressof(o))
            return;
        Base::swapContents(&o);

        std::swap(tailIndex_, o.tailIndex_);
        std::swap(headIndex_, o.headIndex_);
        std::swap(allocChunkSize_, o.allocChunkSize_);
    }

    void advanceHead() noexcept {
        ++headIndex_;
        if (headIndex_ == allocatedSize_)
            headIndex_ = 0;

        //headIndex_ = (headIndex_ == allocatedSize_ - 1) ? 0 : headIndex_ + 1;

        if (headIndex_ == tailIndex_) {
            ++tailIndex_;
            if (tailIndex_ == allocatedSize_)
                tailIndex_ = 0;
            //tailIndex_ = (tailIndex_ == allocatedSize_ - 1) ? 0 : tailIndex_ + 1;
        }

        updateCount();
    }

    void push(const T& v) {
        reserve(count_ + 2);
        buffer_[headIndex_] = v;
        advanceHead();
    }

    T& emplace_head() {
        reserve(count_ + 2);
        auto& v = buffer_[headIndex_];
        advanceHead();
        return v;
    }

    T& emplace_back() { return emplace_head(); }



    void emplace(T&& v) {
        reserve(count_ + 2);
        buffer_[headIndex_] = std::move(v);
        advanceHead();
    }

    T& emplace() {
        return emplace_head();
    }

    void operator += (const T& v) { push(v); }

    Index max_size() const noexcept { return allocatedSize_; }
    bool full() const noexcept { return count_ == allocatedSize_; }

    void clear() noexcept {
        tailIndex_ = headIndex_ = 0;
        count_ = 0;
        empty_ = true;
    }

    void pop() noexcept {
        if (tailIndex_ == headIndex_)
            return;

        ++tailIndex_;
        if (tailIndex_ == allocatedSize_) tailIndex_ = 0;
        updateCount();
    }

    // cuts all the elements until the beginning
    Index erase(Index index) noexcept {
        headIndex_ = index;
        updateCount();
        return rnextIndex(index);
    }
    Index cut(Index index) noexcept {
        tailIndex_ = nextIndex(index);
        updateCount();
        return tailIndex_;
    }

    Index nextIndex(Index index) const noexcept { return (index == allocatedSize_ - 1) ? 0 : index + 1; }
    Index rnextIndex(Index index) const noexcept { return (index == 0 ? allocatedSize_: index) - 1; }

    auto beginIndex() const noexcept { return tailIndex_; }
    auto endIndex() const noexcept { return headIndex_; }
    auto rbeginIndex() const noexcept { return rnextIndex(headIndex_); }
    auto rendIndex() const noexcept { return rnextIndex(tailIndex_); }


    auto& top() noexcept { return buffer_[beginIndex()]; }
    const auto& top() const noexcept { return buffer_[beginIndex()]; }


    // Iterator
    template <class It, class ContainerPtr>
    struct IteratorBase {
        IteratorBase(ContainerPtr buffer, Index idx): container{buffer}, index{idx} {}

        auto& operator*() noexcept { return container->buffer_[index]; }
        const auto& operator*() const noexcept { return container->buffer_[index]; }

        auto* operator->() noexcept { return std::addressof(operator*()); }
        const auto* operator->() const noexcept { return std::addressof(operator*()); }


        friend bool operator == (It a, It b) noexcept { return a.index == b.index; }
        friend bool operator != (It a, It b) noexcept { return !(a == b); }

        ContainerPtr container;
        Index index;
    };

    template <class ContainerPtr>
    struct Iterator: public IteratorBase<Iterator<ContainerPtr>, ContainerPtr> {

        Iterator(ContainerPtr buffer, Index idx): IteratorBase<Iterator, ContainerPtr>{buffer, idx} {}

        // Prefix
        friend auto& operator++(Iterator& a) noexcept {
            a.index = a.container->nextIndex(a.index);
            return a;
        }

        // Postfix
        friend auto operator++(Iterator& a, int) noexcept {
            auto b = a;
            ++a;
            return b;
        }
    };

    template <class ContainerPtr>
    struct ReverseIterator: public IteratorBase<ReverseIterator<ContainerPtr>, ContainerPtr> {
        ReverseIterator(ContainerPtr buffer, Index idx): IteratorBase<ReverseIterator, ContainerPtr>{buffer, idx} {}

        // Prefix
        friend auto& operator++(ReverseIterator& a) noexcept { 
            a.index = a.container->rnextIndex(a.index);  
            return a;
        }

        // Postfix
        friend auto operator++(ReverseIterator& a, int) noexcept {
            auto b = a;
            ++a;
            return b;
        }
    };


    using iterator = Iterator<Self*>;
    using const_iterator = Iterator<const Self*>;
    using reverse_iterator = ReverseIterator<Self*>;
    using const_reverse_iterator = ReverseIterator<const Self*>;

    auto begin() noexcept { return iterator{this, beginIndex()}; }
    auto begin() const noexcept { return const_iterator{this, beginIndex()}; }
    auto cbegin() const noexcept { return const_iterator{this, beginIndex()}; }

    auto end() noexcept { return iterator{this, endIndex()}; }
    auto end() const noexcept { return const_iterator{this, endIndex()}; }
    auto cend() const noexcept { return const_iterator{this, endIndex()}; }

    auto rbegin() noexcept { return reverse_iterator(this, rbeginIndex()); }
    auto rend() noexcept { return reverse_iterator(this, rendIndex()); }

    auto rbegin() const noexcept { return const_reverse_iterator(this, rbeginIndex()); }
    auto rend() const noexcept { return const_reverse_iterator(this, rendIndex()); }



    auto erase(iterator it) noexcept { it.index = erase(it.index);  return it; }
    auto erase(const_iterator it) noexcept { it.index = erase(it.index);  return it; }
    auto erase(reverse_iterator it) noexcept { it.index = erase(it.index);  return it; }
    auto erase(const_reverse_iterator it) noexcept { it.index = erase(it.index);  return it; }

    auto cut(iterator it) noexcept { it.index = cut(it.index);  return it; }
    auto cut(const_iterator it) noexcept { it.index = cut(it.index);  return it; }
    auto cut(reverse_iterator it) noexcept { it.index = cut(it.index);  return it; }
    auto cut(const_reverse_iterator it) noexcept { it.index = cut(it.index);  return it; }


private:
    void greedyBufferExpand(Index total) {
        if (count_ + total > allocatedSize_) {
            reserve(count_ + total + allocChunkSize_);
            allocChunkSize_ *= 2;
        }
    }

    void updateCount() noexcept {
        if (headIndex_ < tailIndex_) {
            count_ = headIndex_ + (allocatedSize_ - tailIndex_);
        } else 
            count_ = headIndex_ - tailIndex_;

        empty_ = count_ == 0;
    }

};


}  // namespace apl
