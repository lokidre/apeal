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
#include "../verify.hpp"

#include <memory>
#include <limits>

namespace apl {

namespace impl_ {

template <class T>
struct LinkedRingBufferItem {
    LinkedRingBufferItem* next{};
    T data{};
};

template <class T>
class LinkedRingBufferIterator {
    using Item = LinkedRingBufferItem<T>;
    using ItemP = Item*;
    using ItemCP = const Item*;

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    ItemP current_{};

public:
    explicit LinkedRingBufferIterator(ItemP item): current_{item}{}

    auto operator<=>(const LinkedRingBufferIterator&) const = default;

    // pointer operator->() noexcept { return current_; };
    // const_pointer operator->() const noexcept { return current_; };
    reference operator*() noexcept { return current_->data; };
    const_reference operator*() const noexcept { return current_->data; };

    auto &operator++() noexcept {
        current_ = current_->next;
        return *this;
    }

    auto operator++(int) noexcept {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

};

}  // namespace impl_

template <class T, class AllocatorT = std::allocator<T>>
class LinkedRingBuffer {
    using Item = impl_::LinkedRingBufferItem<T>;
    using ItemP = Item*;
    using Allocator = AllocatorT;


    Allocator allocator_{};
    Index maxSize_{std::numeric_limits<Index>::max() - 1};

    ItemP allocatedHead_{}, allocatedTail_{};
    Index allocatedCount_{};

    ItemP head_{};
    ItemP tail_{};
    Index count_{};


    ItemP allocItem() {
        auto item = new Item();
        // auto item = allocator_.allocate(1);
        // std::construct(allocator_, item);

        // if (!allocatedHead_) {
        //     allocatedHead_ = allocatedTail_ = item;
        // } else {
        //     allocatedTail_->next = item;
        //     allocatedTail_ = item;
        // }
        //++allocatedCount_;

        return item;
    }

    void destroyItem(ItemP item) {
        delete item;
    }

    ItemP acquireAllocatedItem() {
        ItemP item;
        if (allocatedHead_) {
            item = allocatedHead_;
            allocatedHead_ = allocatedHead_->next;
            --allocatedCount_;
        } else {
            item = allocItem();
        }
        return item;
    }

    void releaseAllocatedItem(ItemP item) {
        if (!allocatedHead_) {
            allocatedHead_ = allocatedTail_ = item;
        } else {
            allocatedTail_->next = item;
            allocatedTail_ = item;
        }
        allocatedTail_->next = nullptr;

        ++allocatedCount_;
    }


public:
    using value_type = T;
    using difference_type = ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;

    using size_type = std::size_t;
    using usize_type = std::size_t;
    using ssize_type = std::ptrdiff_t;

    using iterator = impl_::LinkedRingBufferIterator<T>;
    using const_iterator = impl_::LinkedRingBufferIterator<T>;



    LinkedRingBuffer() = default;
    ~LinkedRingBuffer() noexcept {
        for (auto item = allocatedHead_; item; ) {
            auto next = item->next;
            destroyItem(item);
            item = next;
        }
        allocatedHead_ = allocatedTail_ = nullptr;
        allocatedCount_ = 0;

        for (auto item = head_; item; ) {
            auto next = item->next;
            destroyItem(item);
            item = next;
        }
        head_ = tail_ = nullptr;
        count_ = 0;
    }

    void setMaxSize(Index maxSize) { maxSize_ = maxSize; }
    [[nodiscard]] auto allocated_size() const noexcept { return static_cast<size_type>(allocatedCount_); }
    [[nodiscard]] auto allocated_ssize() const noexcept { return static_cast<ssize_type>(allocatedCount_); }


    [[nodiscard]] auto size() const noexcept { return static_cast<size_type>(count_); }
    [[nodiscard]] auto ssize() const noexcept { return static_cast<ssize_type>(count_); }
    [[nodiscard]] auto empty() const noexcept { return count_ == 0; }
    [[nodiscard]] auto max_size() const noexcept { return static_cast<size_type>(maxSize_); }
    [[nodiscard]] auto full() const noexcept { return count_ >= maxSize_; }

    void reserve(size_type count) noexcept {}
    void clear() noexcept {
        while (!empty()) pop();
    }

    auto begin() noexcept { return iterator(head_); }
    auto begin() const noexcept { return iterator(head_); }
    auto cbegin() const noexcept { return iterator(head_); }
    auto end() noexcept { return iterator(nullptr); }
    auto end() const noexcept { return iterator(nullptr); }
    auto cend() const noexcept { return iterator(nullptr); }


    T& emplace_back() {
        verify(count_ < maxSize_, "LinkedRingBuffer overflow");

        ItemP item = acquireAllocatedItem();

        if (!head_) {
            head_ = tail_ = item;
        } else {
            tail_->next = item;
            tail_ = item;
        }

        tail_->next = nullptr;
        count_++;

        return item->data;
    }

    T& emplace() {
        return emplace_back();
    }

    T& emplace(T&& value) {
        auto& val = emplace_back();
        val = std::move(value);
        return val;
    }



    T& push(const T& value) {
        auto& val = emplace_back();
        val = value;
        return val;
    }

    auto& push_back(const T& value) { return push(value); }

    [[nodiscard]] const_reference top() const {
        verify(count_ != 0, "LinkedRingBuffer::pop() on empty");
        return head_->data;
    }
    [[nodiscard]] reference top() {
        verify(count_ != 0, "LinkedRingBuffer::pop() on empty");
        return head_->data;
    }

    [[nodiscard]] const_reference front() const { return top(); }
    [[nodiscard]] reference front() { return top(); }


    void pop() {
        verify(count_ != 0, "LinkedRingBuffer::pop() on empty");

        auto item = head_;
        head_ = head_->next;
        --count_;

        releaseAllocatedItem(item);
    }

    void pop_front() { return pop(); }

};



}  // namespace apl

