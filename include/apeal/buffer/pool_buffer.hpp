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


// T class must have explicit operator bool() defined
// as this is used to determine if the item is available

template <class T, int N = 0>
class PoolBuffer: public impl::BufferBase<T> {
    using Base = impl::BufferBase<T>;
    using Self = PoolBuffer<T, N>;

    //friend class Iterator;

    using SelfRef = Self &;
    using SelfPtr = Self *;

public:
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


private:
    Index availIndex_{};



    constexpr void constr() noexcept {
        count_ = N;
    }

public:
    PoolBuffer() {
        constr();
        checkAlloc();  // allocate immediately. think about it
    }

    /*
    PoolBuffer(const Self &a) {  copy(a) ;  }
    PoolBuffer(Self &&a) { move(std::move(a)) ; }
    */

    PoolBuffer(std::initializer_list<T> l) {
        constr();
        for (auto it = l.begin(), end = l.end(); it != end; ++it)
            pick() = *it;
    }

    /*
    PoolBuffer &operator=( const RingBuffer &a) {
        copy(a) ;
        return *this ;
    }
    */

    ~PoolBuffer() noexcept {
        if (buffer_) {
            delete[] buffer_;
            buffer_ = nullptr;
        }
    }

    void resize(Index newSize) {
        if (buffer_)
            delete[] buffer_;
        count_ = newSize;
        //size_ = size;
        checkAlloc();
    }

    // Finds the available element or returns nullptr
    T *pickIfAvail() {
        checkAlloc();
        auto startIdx = availIndex_;
        Index idx;

        for (;;) {
            idx = availIndex_;
            availIndex_ = (availIndex_ == count_-1) ? 0 : availIndex_ + 1;

            if (!buffer_[idx])
                break;

            // we made a full cycle
            if (startIdx == availIndex_)
                return 0;
        }
        return buffer_ + idx;
    }

    // Find next available element
    // If no available found picks any
    T *pickAny() {
        if (auto item = pickIfAvail(); item)
            return item;

        auto idx = availIndex_;
        availIndex_ = (availIndex_ == count_ - 1) ? 0 : availIndex_ + 1;
        return buffer_ + idx;
    }

    T &pick() { return *pickAny(); }


    /*
    void copy ( const Self &a) {
        tailIndex_ = a.tailIndex_ ;
        headIndex_ = a.headIndex_ ;
        if ( empty() )
            return ;

        checkAlloc() ;

        if ( full() ) {
            if ( std::is_trivially_copyable<T>::value )
                // works only on trivialy copyable types, need to check
                std::memcpy ( buffer_, a.buffer_, TOTAL * sizeof(T) ) ;
            else
                std::copy_n ( a.buffer_, TOTAL, buffer_ ) ;
            return ;
        }

        // copy only parts
        if ( std::is_trivially_copyable<T>::value ) {
            if ( tailIndex_ < headIndex_ ) {
                std::memcpy ( buffer_ + tailIndex_, a.buffer_ + tailIndex_, ( headIndex_ - tailIndex_ ) * sizeof(T) ) ;
            } else {
                std::memcpy ( buffer_ + tailIndex_, a.buffer_ + tailIndex_, ( TOTAL - tailIndex_ ) * sizeof(T) ) ;
                std::memcpy ( buffer_, a.buffer_, headIndex_ * sizeof(T) ) ;
            }
        } else {
            if ( tailIndex_ < headIndex_ ) {
                std::copy_n ( a.buffer_ + tailIndex_, headIndex_ - tailIndex_, buffer_ + tailIndex_ ) ;
            } else {
                std::copy_n ( a.buffer_ + tailIndex_, TOTAL - tailIndex_, buffer_ + tailIndex_ ) ;
                std::copy_n ( a.buffer_, headIndex_, buffer_ ) ;
            }
        }

    }

    void move ( Self && a ) {
        tailIndex_ = a.tailIndex_ ;
        headIndex_ = a.headIndex_ ;
        buffer_ = a.buffer_ ;
        a.buffer_ = 0 ;
    }
    */

    // expensive operation
    //bool empty() const {  return headIndex_ == tailIndex_ ; }
    //explicit operator bool() const {  return !empty() ;  }

    // expensive operation
    Index size() const noexcept { return N; }
    Index max_size() const noexcept { return N; }

    // expensive operation
    //bool full() const {
    //	return (tailIndex_ == 0)? headIndex_ == TOTAL - 1 : headIndex_ + 1 == tailIndex_ ;
    //}

    //void clear() {
    //	tailIndex_ = headIndex_ = 0 ;
    //}

    //T &top() { return buffer_[beginIndex()] ;  }
    //const T &top() const {  return buffer_[beginIndex()] ;  }


    //using iterator = T *;
    //using const_iterator = const T *;

    //iterator begin() { return buffer_; }
    //const_iterator begin() const { return buffer_; }
    //iterator end() { return buffer_ + size_; }
    //const_iterator end() const { return buffer_ + size_; }



    //const T *data() const noexcept { return buffer_; }
    //T *data() noexcept { return buffer_; }


    Index availIndex() const noexcept { return availIndex_; }
    Index availHint(Index hint) noexcept { 
        availIndex_ = hint;  
        return availIndex_; 
    }


private:
    void checkAlloc() {
        if (count_ == 0)
            return;

        if (!buffer_)
            buffer_ = new T[count_];
    }

};


}  // namespace apl
