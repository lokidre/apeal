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

/*
    To enable bitmask for particular EnumType use:

    template<> struct EnumBitMask<E>: std::true_type{};
*/

template <class E>
struct EnumBitMask: std::false_type {};

template <class E>
constexpr auto EnumBitMaskV = EnumBitMask<E>::value;


template <class E, std::enable_if_t<EnumBitMaskV<E>, int> = 0>
constexpr E operator | (E a, E b) noexcept {
    using U = typename std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(a) | static_cast<U>(b));
}


template <class E, std::enable_if_t<EnumBitMaskV<E>, int> = 0>
constexpr E operator & (E a, E b) noexcept {
    using U = typename std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(a) & static_cast<U>(b));
}

template <class E, std::enable_if_t<EnumBitMaskV<E>, int> = 0>
constexpr E operator ^ (E a, E b) noexcept {
    using U = typename std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(a) ^ static_cast<U>(b));
}

template <typename E>
constexpr
typename std::enable_if<EnumBitMask<E>::value, E>::type
operator ~ (E rhs) noexcept {
    using U = typename std::underlying_type<E>::type;
    return static_cast<E>(~static_cast<U>(rhs));
}


template <typename E>
constexpr
typename std::enable_if<EnumBitMask<E>::value, E>::type &
operator |= (E &lhs, E rhs) noexcept {
    using U = typename std::underlying_type_t<E>;
    lhs = static_cast<E>(static_cast<U>(lhs) | static_cast<U>(rhs));
    return lhs;
}

template <typename E>
constexpr
typename std::enable_if<EnumBitMask<E>::value, E>::type &
operator &= (E &lhs, E rhs) noexcept {
    using U = typename std::underlying_type<E>::type;
    lhs = static_cast<E>(static_cast<U>(lhs) & static_cast<U>(rhs));
    return lhs;
}

template <typename E>
constexpr
std::enable_if_t<EnumBitMask<E>::value, E>&
operator ^= (E &lhs, E rhs) noexcept {
    using U = typename std::underlying_type_t<E>;
    lhs = static_cast<E>(static_cast<U>(lhs) ^ static_cast<U>(rhs));
    return lhs;
}


//
// Testing function
//

template <class E, std::enable_if_t<EnumBitMaskV<E>, int> = 0>
constexpr bool operator ! (E e) noexcept { return static_cast<E>(0) == e; }

template <class E, std::enable_if_t<EnumBitMaskV<E>, int> = 0>
constexpr bool isAnyBit(E e) noexcept { return static_cast<E>(0) != e; }

template <class E, std::enable_if_t<EnumBitMaskV<E>, int> = 0>
constexpr bool isZero(E e) noexcept { return static_cast<E>(0) == e; }

//template <typename E>
//constexpr
//typename std::enable_if_t<EnumBitMask<E>::value, bool>
//is_set(E lhs, E rhs) noexcept { return anybit(lhs&rhs); }

template <class E, std::enable_if_t<EnumBitMaskV<E>, int> = 0>
constexpr bool isSet(E a, E b) noexcept { return isAnyBit(a & b); }

template <class E, std::enable_if_t<EnumBitMaskV<E>, int> = 0>
constexpr bool isClear(E a, E b) noexcept { return zero(a & b); }


}  // namespace apl

