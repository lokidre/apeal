/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

namespace apl {

template <class Impl>
class CrcBase {
};


template <class Impl>
class Crc: public Impl {
public:
    using typename Impl::Value;

    using typename Impl::iterator;
    using typename Impl::const_iterator;

    using Impl::ValueBits;
    using Impl::ValueBytes;


public:
    static Value start() noexcept {
        return Impl::onStart();
    }

    static auto update_it(Value crc, const_iterator begin, const_iterator end) noexcept {
        return Impl::onUpdate(crc, begin, end); 
    }

    static Value finish(Value crc) noexcept {  
        return Impl::onFinish(crc); 
    }



    template <class T>
    static Value update(Value crc, std::span<T> data) noexcept {
        auto beginIt = (const std::uint8_t *)data.data();
        auto endIt = beginIt + data.size() * sizeof(T);
        return update_it(crc, beginIt, endIt);
    }


    static auto compute_it(const_iterator begin, const_iterator end)
    {
        auto crc = start();
        crc = update_it(crc, begin, end);
        return finish(crc);
    }



    static auto compute_n(const void* data, std::size_t size) noexcept {
        auto ptr = (const_iterator)data;
        return compute_it(ptr, ptr + size);
    }

    template <class Container>
    static Value compute_data(const Container& data) noexcept {
        auto beginIt = (const std::uint8_t *)data.data();
        auto endIt = beginIt + data.size() * sizeof(typename Container::value_type);
        return compute_it(beginIt, endIt);
    }


    template <class Object>
    static Value compute_obj(const Object& v) noexcept {
        auto begin = (const std::uint8_t *)&v;
        auto end = begin + sizeof(v);
        return compute_it(begin, end);
    }
};

}  // namespace
