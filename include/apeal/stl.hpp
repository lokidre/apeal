/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

//
// Some fixes and improvements that could have been implemented in std::
//

#include <algorithm>
#include <type_traits>



//#ifdef TARGET_OS_ANDROID
//// Quick workaround until NDK is updated
//#include <concepts>
//
//namespace std {
//
//template <class T>
//concept integral = std::is_integral_v<T>;
//
//template <class T>
//concept floating_point = std::is_floating_point_v<T>;
//
//template <class T>
//concept signed_integral = std::integral<T> && std::is_signed_v<T>;
//
//
//template <class T>
//concept unsigned_integral = std::integral<T> && std::is_unsigned_v<T>;
//
//}  // namespace stl
//
//#endif  // TARGET_OS_ANDROID


namespace apl {

// Char concept
template <typename T>
concept CharacterT = std::same_as<T, char> || std::same_as<T, unsigned char>;

}



namespace apl {


//
// Adding range to std loops
//
template <class Container, class Unary>
constexpr auto forEach(Container &c, Unary f) {
    auto&& range = c;
    return std::for_each(std::begin(range), std::end(range), f);
}

template <class Container, class Unary>
constexpr auto forEach(const Container &v, Unary f) {
    auto&& range = v;
    return std::for_each(std::cbegin(range), std::end(range), f);
}



template <class Container, class Unary>
constexpr auto anyOf(const Container& c, Unary f) {
    auto&& range = c;
    return std::any_of(std::begin(range), std::end(range), f);
}



template <class Container, class Unary>
constexpr auto findIf(Container &c, Unary f) {
    auto&& range = c;
    return std::find_if(std::begin(range), std::end(range), f);
}



template <class C, class Unary>
constexpr auto findIf(const C &c, Unary f) {
    auto &&range = c;
    return std::find_if(std::cbegin(range), std::cend(range), f);
}



template <class C, class Unary>
auto countIf(const C &c, Unary f) //-> typename std::iterator_traits<typename V::iterator>::difference_type
{
    auto &&range = c;
    return std::count_if(std::cbegin(range), std::cend(range), f);
}



//
// Using int as return value for "size()" and "length()" operators
//

// Determine is the type is container by checking if it has data() and size() members
template <class T, typename C = void>
struct THasSize: std::false_type {};

template <class T>
struct THasSize<T, std::void_t<decltype(std::declval<T>().size())>>: std::true_type {};


template <class C, std::enable_if_t<THasSize<C>::value, int> = 0>
constexpr int count(const C &c) noexcept { return static_cast<int>(std::ssize(c)); }



template <class V, class K>
bool contains(const V& v, K&& k) noexcept { 
    return v.find(std::forward<K>(k)) != v.end(); 
}


//template <class V, class K>
////typename std::enable_if<THasFind<C,K>::value,bool>::type 
//bool contains(V& v, K&& k) noexcept {
//    //return c.find(k) != c.end();
//    return std::find(std::cbegin(v), std::cend(v), std::forward<K>(k)) != std::cend(v);
//}

template <class V, class Sep>
String join(const V& v, Sep sep) {
    String s;
    for (auto&&i: v) {
        if (!s.empty())
            s += sep; //// .append(1, sep);
        s += i;
    }
    return s;
}


template <class S, class C, class Sep, class F>
void joinAppend(S &s, const C &c, const Sep &sep, F f) noexcept
{
    for (auto &v: c) {
        if (!s.empty())
            s.append(1, sep);
        s += f(v);
    }
}

template <class C, class Sep, class F>
auto join(const C &c, const Sep &sep, F f) noexcept 
{
    String s;
    joinAppend(s, c, sep, f);
    return s;
}


template <class T, class = void>
struct AHasMemberEmptyT: std::false_type{};

template <class T>
struct AHasMemberEmptyT<T, std::void_t<decltype(std::declval<T>().empty())>> : std::true_type {};



template <class T, class = void>
struct AHasOperatorBoolT: std::false_type{};



template <class T>
struct AHasOperatorBoolT<T, decltype(static_cast<bool>(std::declval<T>()))>: std::true_type {};
//struct AHasOperatorBoolT<T, std::void_t<decltype(std::declval<T>().operator!())>> : std::true_type {};
//struct AHasOperatorBoolT<T, std::void_t<decltype(std::declval<T>().operator bool())>> : std::true_type {};

template <class T>
inline constexpr auto AHasOperatorBoolTV = AHasOperatorBoolT<T>::value;


// IntelliSense bug: crashed on the following test
//template <typename T>
//struct AHasOperatorBoolT<T, std::void_t<decltype(std::declval<T>().operator bool())>> : std::true_type {};





//template <class T, typename std::enable_if_t<std::is_convertible_v<T, bool> && !std::is_same_v<T, bool>, int> = 0>
//constexpr bool isFalse(T &&x) noexcept { return !x; }

//template <class T, typename std::enable_if_t<std::is_integral_v<T>, int> = 0>
//constexpr bool isFalse(T x) noexcept { return !x; }


//template <class T, typename std::enable_if_t<std::is_same_v<T, bool>, int> = 0>
//constexpr bool isFalse(T x) noexcept { return !x; }
//
//template <class T, typename std::enable_if_t<std::is_pointer_v<T>, int> = 0>
//constexpr bool isFalse(T x) noexcept { return !x; }
//
//
//template <class T, 
//    typename std::enable_if_t<
//        std::conjunction_v<
//            std::negation<std::is_same<T, bool>>, 
//            std::is_integral<T>
//        >,
//        int> = 0>
//constexpr bool isFalse(T x) noexcept { return !x; }


template <class T, typename std::enable_if_t<std::is_convertible_v<T, bool>, int> = 0>
constexpr bool isFalse(T x) noexcept { return !x; }

template <class T, typename std::enable_if_t<AHasOperatorBoolTV<T>, int> = 0>
constexpr bool isFalse(const T &x) noexcept { return !x; }



//template <class T, typename std::enable_if_t<AHasMemberEmptyT<T>::value, int> = 0>

template <class T, 
    typename std::enable_if_t<
        std::conjunction_v<
            std::negation<AHasOperatorBoolT<T>>, 
            AHasMemberEmptyT<T>
        >,
        int> = 0>
constexpr bool isFalse(const T &x) noexcept { return x.empty(); }




//template <class T, typename std::enable_if_t<std::is_integral_v<T>, int> = 0>
//constexpr bool isFalse(T x) noexcept { return !x; }


//template <class T, typename std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
//constexpr bool isFalse(const T &x) noexcept { return x.empty(); }
//
//template <class T, typename std::enable_if_t<std::is_same_v<T, std::string_view>, int> = 0>
//constexpr bool isFalse(T x) noexcept { return x.empty(); }



//// std::string enhancements
//inline bool operator !(const std::string &s) { return s.empty(); }
//inline bool operator !(const std::string_view &s) { return s.empty(); }


}  // namespace apl

