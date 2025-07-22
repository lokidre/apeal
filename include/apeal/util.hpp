/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <atomic>
#include <cmath>
#include <cstring>
#include <functional>

namespace apl {

// Returns size of the static array
template <class T, size_t N>
constexpr size_t array_size(T (&)[N]) noexcept { return N ;  }

template <class T, size_t N>
constexpr size_t arraySize(T (&)[N]) noexcept { return N ;  }

// Returns size of the static array
template <class T, size_t N>
constexpr int arrayCount(T (&)[N]) noexcept { return (int)N ;  }



// Used to prevent "unused variable" warning
template <class... T>
constexpr void unused(T&&...) noexcept {}

// Zeroes object
template <class T>
void zeroObject(T& obj) noexcept {
     std::memset(&obj, 0, sizeof(obj));
}



// Allows setting up exit functions
class ScopeExit {
public:
    ScopeExit(std::function<void()> f): f_(f) {}
    ~ScopeExit() noexcept { try { f_(); }catch(...){}  }
private:
    std::function<void()> f_ ;
};

// Emulates postfix statement in ranged for loops
// int index = 0 ;
// for ( it: range ) {
//   Postfix p ( [&]() { ++index ; }) ;
//   // ... statements ...
// }
class Postfix {
public :
    Postfix(std::function<void()> f): f_(f) {
    }
    ~Postfix() {
        f_() ;
    }
private:
    std::function<void()> f_ ;
} ;

// Simplified version, just with one increment
// int index = 0 ;
// for ( it: range ) {
//   APostfixInc inc(index) ;
//   // ... statements ...
// }
class PostfixInc {
public:
    PostfixInc(int& idx) noexcept: idx_{idx} {}
    ~PostfixInc() noexcept { ++idx_; }
private:
    int& idx_;
};



// Creates wrapper around std::atomic to make atomic be part of std::vector
template <typename T>
struct AtomicWrapper {
    std::atomic<T> v ;

    AtomicWrapper():v(){}
    AtomicWrapper(const std::atomic<T> &a):v(a.load()){}
    AtomicWrapper(const AtomicWrapper &x):v(x.v.load()){}
    AtomicWrapper &operator=(const AtomicWrapper &x){v.store(x.v.load());}
} ;

template<class T, class U, class V>
constexpr T aclamp(const T &x, const U &mn, const V &mx) noexcept {
    return x < T(mn) ? T(mn) : ( x > T(mx) ? T(mx) : x ) ;
}

template <typename T>
constexpr int isgn(T val) noexcept {
    return (T(0) < val) - (val < T(0));
}

//template <class T, typename U>
//constexpr T mix ( const T &a, const T &b, const U &l ) {
//  return a*(1-l) + b*l ;
//}

template <class T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
constexpr T lerp(T a, T b, T t) noexcept { return a*(1-t) + b*t; }


// TODO: change to std::string_view
constexpr uint32_t fourcc(char const* p) noexcept
{
    return (uint32_t(p[0]) * 16777216) + (uint32_t(p[1]) * 65536) + (uint32_t(p[2]) * 256) + uint32_t(p[3]);
}

constexpr uint32_t fourcc_le(char const* p) noexcept
{
    return (uint32_t(p[3]) * 16777216) + (uint32_t(p[2]) * 65536) + (uint32_t(p[1]) * 256) + uint32_t(p[0]);
}


constexpr char* from_fourcc(uint32_t fcc, char* p) noexcept
{
    p[3] = char(fcc);
    p[2] = char(fcc>>8);
    p[1] = char(fcc>>16);
    p[0] = char(fcc>>24);
    p[4] = 0;
    return p;
}

constexpr char* from_fourcc_le(uint32_t fcc, char* p) noexcept
{
    p[0] = char(fcc);
    p[1] = char(fcc>>8);
    p[2] = char(fcc>>16);
    p[3] = char(fcc>>24);
    p[4] = 0;
    return p;
}

}  // namespace apl
