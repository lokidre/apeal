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

/*
    Convenience class for passing any arbitrary object to the function
    expecing a buffer in a {data,size} format.

    If the function argument is defined as DataParam buffer

    then the data can be passed as { pointer, size } or by any class that support data() and size()
   methods
*/

namespace apl {


// Until concepts implemented in clang for xcode and android ndk
// template<typename T>
// concept AParamDataContainer = requires(T t){
//    t.data();
//    t.size();
//};

template <typename _P = const uint8_t*, typename _S = std::size_t>
struct DataParam {
    using T = void;
    using Ptr = _P;
    using Size = _S;

    using Self = DataParam<_P, _S>;

    using FP = const DataParam<_P, _S>&; // Pass by value to functions
    using Arg = const Self&;             // Pass by value to functions

    Ptr data_;
    Size size_;


    using pointer = Ptr;
    using const_pointer = const Ptr;
    using size_type = Size;


    constexpr pointer data() noexcept { return data_; }
    constexpr const_pointer data() const noexcept { return data_; }

    constexpr size_type size() const noexcept { return size_; }

    constexpr std::string str() const noexcept {
        return {reinterpret_cast<const char*>(data_), size_};
    }

    constexpr std::string_view strview() const noexcept {
        return {reinterpret_cast<const char*>(data_), size_};
    }


    // constexpr void dummy(){}

    constexpr explicit operator bool() const noexcept { return size_; }

    template <class C>
    static Self make(const C& c) noexcept {
        return {c.data(), c.size()};
    }

    template <class P>
    static Self make(const P* p, std::size_t s) noexcept {
        return {p, s};
    }


    // template <class C>
    // requires requires(C t){
    //    !std::is_same<C, DataParam>::value;
    //    t.data();
    //    t.size();
    //}
    // template <AParamDataContainer C>
    // DataParam(const C &d): data_{(Ptr)d.data()}, size_{(Size)d.size()} {}


    // template <class C>
    // requires requires(C t){ !std::is_same<C, DataParam>::value && t.data() && t.size(); }
    // DataParam &operator=(const C &c)
    //{
    //    data_ = (Ptr)c.data();
    //    size_ = (Size)c.size();
    //    return *this;
    //}

    // template <class Container>
    // std::enable_if


    // Build from ( data, size ) call
    template <typename D, typename S>
    DataParam(D* d, S s) noexcept : data_{(Ptr)d}, size_{(Size)s} {}

    // Receiving ourselves, usually just passing a parameter
    template <typename D, typename S>
    DataParam(Arg p) noexcept : data_{p.data_}, size_{p.size_} {}

    // Build from any container that support .data() and .size() methods
    template <class C>
    DataParam(const C& d) noexcept : data_{(Ptr)d.data()}, size_{(Size)d.size()} {}

    // Build null data param
    DataParam() noexcept : data_{}, size_{} {}
};


template <typename TT, typename _P = const uint8_t*, typename _S = std::size_t>
struct DataParamValue {

    using T = TT;
    using Self = DataParamValue<T, _P, _S>;
    using TFP = IntegParam<T>;
    using Ptr = _P;
    using Size = _S;

    using FP = const DataParam<_P, _S>&; // Pass by const reference
    using Arg = const Self&;

    Ptr data_;
    Size size_;
    T value_;


    using pointer = Ptr;
    using const_pointer = const Ptr;
    using size_type = Size;


    constexpr const T& value() const noexcept { return value_; }

    constexpr pointer data() noexcept { return data_; }
    constexpr const_pointer data() const noexcept { return data_; }

    constexpr size_type size() const noexcept { return size_; }

    constexpr explicit operator bool() const noexcept { return size_ != 0; }


    // Build from ( data, size ) call
    template <typename D, typename S>
    DataParamValue(D* d, S s, TFP v) noexcept : data_{(Ptr)d}, size_{(Size)s}, value_{v} {}

    // Build from any container that supports .data() and .size() methods
    // TODO: need checks
    template <class C>
    DataParamValue(const C& d, TFP v) noexcept
        : data_{(Ptr)d.data()}, size_{(Size)d.size()}, value_{v} {}

    // Build null data param
    DataParamValue() noexcept : data_{}, size_{}, value_{} {}
};


} // namespace apl
