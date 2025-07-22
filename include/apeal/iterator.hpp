/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "def.hpp"

#include <iterator>

namespace apl {

//
// V - std:: container

template <class V>
class RangeIterator: public std::iterator_traits<typename V::iterator> {
    using Self = RangeIterator<V>;
    using It = std::iterator_traits<typename V::iterator>;

public:
    using value_type = typename V::value_type;

    using pointer = typename V::pointer;
    using const_pointer = typename V::const_pointer;

    using reference = typename V::reference;
    using const_reference = typename V::const_reference;

    using iterator = typename V::iterator;
    using const_iterator = typename V::const_iterator;


private:
    const_iterator begin_, end_, it_;

public:
    constexpr RangeIterator(const V& v) noexcept: begin_{v.begin()}, end_{v.end()}, it_{begin_} {}

    constexpr RangeIterator(const V& v, iterator it) noexcept
        : begin_{v.begin()}, end_{v.end()}, it_{it} {}

    constexpr RangeIterator(const V& v, iterator it, iterator end) noexcept
        : begin_{v.begin()}, end_{end}, it_{it} {}

    // RangeIterator(const_iterator begin, const_iterator end): begin_(begin), end_(end) {
    //    it_ = begin_;
    //}

    // RangeIterator(const_iterator begin, const_iterator end, const_iterator it): begin_(begin),
    // end_(end) {
    //    it_ = it;
    //}

    void reset() noexcept { it_ = begin_; }


    constexpr explicit operator bool() const noexcept { return it_ != end_; }

    constexpr reference operator*() { return const_cast<reference>(it_.operator*()); }
    constexpr const_reference operator*() const { return it_.operator*(); }

    constexpr pointer operator->() { return const_cast<pointer>(it_.operator->()); }
    constexpr const_pointer operator->() const { return it_.operator->(); }


    // Postfix
    constexpr Self operator++(int) noexcept {
        auto it = it_;
        ++it_;
        return {begin_, end_, it};
    }

    // Prefix increment
    constexpr Self& operator++() noexcept {
        ++it_;
        return *this;
    }
};

template <class V>
constexpr RangeIterator<V> beginRangeIterator(const V& v) {
    return {v};
}

} // namespace apl
