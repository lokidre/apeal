/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <cmath>

#include "../types.hpp"

namespace apl {


template <class Value>
struct Point2D {
    using Self = Point2D<Value>;

public:
    using T = Value;
    using P = IntegParam<T>;
    using Arg = StructParamN<T, 2, Self>;

private:
    using RSelf = Self &;

    using TP = T *;
    using TCR = const T &;

    using Ptr = T *;
    using CPtr = const T *;
    using Ref = T &;
    using CRef = const T &;

    //RSelf ret() { return *this; }

public:
    using value_type = T;
    using pointer = Ptr;
    using const_pointer = CPtr;
    using reference = Ref;
    using const_reference = CRef;
    using size_type = std::size_t;

public:

    T x, y;


    //
    // Initalization and assignments
    //

    // Building
    template <typename U, typename V>
    constexpr static auto make(U u, V v) noexcept {
        return Self{static_cast<T>(u), static_cast<T>(v)};
    }

    // from higher dimensional vector
    template <class Point>
    Self& assign(const Point &u) noexcept {
        x = u.x, y = u.y;
        return *this;
    }

    // Conversion to/from other data types
    // It will just initialize with {x, y}
    template <class P>
    constexpr auto convert() const noexcept { 
        using U = typename P::value_type;
        return P{static_cast<U>(x), static_cast<U>(y)};
    }

    // Conersion with parameters, they will just passed 
    // TODO: variadic templates
    template <class P, class V>
    constexpr P convert(V p1) const noexcept {
        using U = typename P::value_type;
        return {static_cast<U>(x), static_cast<U>(y), static_cast<U>(p1)};
    }

    template <class P, class V>
    constexpr P convert(V p1, V p2) const noexcept {
        using U = typename P::value_type;
        return {(U)x, (U)y, (U)p1, (U)p2};
    }


    // Extract zero sized rectangle positioned at point
    template <class R>
    constexpr auto rect() const noexcept { return convert<R>(); }

    template <class R, class Sz>
    constexpr auto rect(Sz s) const noexcept { return convert<R>().resize(s); }


    template <class Sz>
    constexpr auto size() const noexcept { return convert<Sz>(); }


    //
    // Accessing Data
    //
    constexpr auto get() noexcept { return Ptr(this); }
    constexpr auto get() const noexcept { return CPtr(this); }

    constexpr auto data() noexcept { return Ptr(this); }
    constexpr auto data() const noexcept { return CPtr(this); }

    constexpr auto span() noexcept { return std::span<T, 2>{Ptr(this), 2}; }
    constexpr auto span() const noexcept { return std::span<T, 2>{CPtr(this), 2}; }



    //
    // Updating Data
    //
    Self &setX(P v) noexcept { x = v;  return *this; }
    Self &setY(P v) noexcept { y = v;  return *this; }

    constexpr Self updatedX(P v) const noexcept { return {v, y}; }
    constexpr Self updatedY(P v) const noexcept { return {x, v}; }
    constexpr Self updX(P v) const noexcept { return {v, y}; }
    constexpr Self updY(P v) const noexcept { return {x, v}; }

    constexpr Self upX(P v) const noexcept { return {v, y}; }
    constexpr Self upY(P v) const noexcept { return {x, v}; }

    Self &offset(Arg v) noexcept { x += v.x, y += v.y;  return *this; }
    //Self &offset(P vx, P vy) noexcept { x += vx, y += vy;  return *this; }
    //constexpr Self offsetted(FP v) noexcept { return {x + v.x, y + v.y}; }
    //constexpr Self offsetted(P vx, P vy) noexcept { return {x + vx, y + vy}; }
    constexpr Self off(Arg v) const noexcept { return {x + v.x, y + v.y}; }
    //constexpr Self off(P vx, P vy) const noexcept { return {x + vx, y + vy}; }

    Self &move(T px, T py) { x = px, y = py; return *this; }


    Self& scale(P s) noexcept { x *= s, y *= s; return *this; }
    constexpr Self scaled(P s) const noexcept { return {x * s, y * s}; }


    T norm() const noexcept { return std::hypot(x, y); }
    T distance(Point2D<T> u) const noexcept { return std::hypot(x - u.x, y - u.y); }

    // yield returns the point with size being in the center
    template <class Sz>
    Self yield(Sz s) const noexcept {  return { x - s.w/2, y - s.h/2 } ; }

    template <class R, class Sz>
    R centered(Sz s) const noexcept {  return { x - s.w/2, y - s.h/2, s.w, s.h } ; }

    // Returns the index as if this point was pointing to an array with width of 'w'
    constexpr T index(P w) const noexcept { return y * w + x; }

    Self &sub(Arg u) noexcept { x -= u.x, y -= u.y;  return *this; }
    //Self subbed(FP u) const noexcept { return {x - u.x, y - u.y}; }

    Self &add(Arg u) noexcept { x += u.x, y += u.y;  return *this; }
    //Self added(const Self &u) const noexcept { return {x + u.x, y + u.y}; }

    //Self off(const Self &u) const noexcept { return {x + u.x, y + u.y}; }

    constexpr bool empty() const noexcept { return !x && !y; }

    //Self &floor() { x = std::floor(x), y = std::floor(y);  return *this; }
    //constexpr Self floor() const noexcept { return {std::floor(x), std::floor(y)}; }

    //
    // Operator overloading
    //
    constexpr explicit operator bool() const { return x || y; }

    constexpr Ref operator[](size_type i) { return data()[i]; }
    constexpr CRef operator[](size_type i) const { return data()[i]; }

    auto operator<=>(const Point2D&) const noexcept = default;

    // constexpr bool operator == (Arg b) const noexcept { return x == b.x && y == b.y; }
    // constexpr bool operator != (Arg b) const noexcept { return !operator==(b); }


    //
    // Arithmetic operators
    // TODO: rewrite as friend free functions
    //

    //
    // Binary operators
    //
    friend constexpr Self operator + (Arg a, Arg b) noexcept { 
        return {a.x + b.x, a.y + b.y}; 
    }
    friend constexpr Self operator - (Arg a, Arg b) noexcept { 
        return {a.x - b.x, a.y - b.y}; 
    }
    friend constexpr Self operator * (Arg a, Arg b) noexcept { 
        return {a.x * b.x, a.y * b.y}; 
    }
    friend constexpr Self operator / (Arg a, Arg b) noexcept { 
        return {a.x / b.x, a.y / b.y}; 
    }


    friend constexpr Self operator + (Arg a, std::type_identity_t<T> v) noexcept { 
        return {a.x + v, a.y + v}; 
    }
    friend constexpr Self operator - (Arg a, std::type_identity_t<T> v) noexcept { 
        return {a.x - v, a.y - v};
    }
    friend constexpr Self operator * (Arg a, std::type_identity_t<T> v) noexcept { 
        return {a.x * v, a.y * v}; 
    }
    friend constexpr Self operator / (Arg a, std::type_identity_t<T> v) noexcept { 
        return {a.x / v, a.y / v};
    }

    //
    // Binary assignment operators
    //
    friend constexpr auto& operator += (Self& a, Arg b) noexcept { 
        a.x += b.x, a.y += b.y; 
        return a;
    }
    friend constexpr auto& operator -= (Self& a, Arg b) noexcept {
        a.x -= b.x, a.y -= b.y; 
        return a;
    }
    friend constexpr auto& operator *= (Self& a, Arg b) noexcept { 
        a.x *= b.x, a.y *= b.y; 
        return a;
    }
    friend constexpr auto& operator /= (Self& a, Arg b) noexcept {
        a.x /= b.x, a.y /= b.y; 
        return a;
    }


    friend constexpr auto& operator += (Self& a, std::type_identity_t<T> v) noexcept { 
        a.x += v, a.y += v; 
        return a;
    }
    friend constexpr auto& operator -= (Self& a, std::type_identity_t<T> v) noexcept {
        a.x -= v, a.y -= v; 
        return a;
    }
    friend constexpr auto& operator *= (Self& a,std::type_identity_t<T> v) noexcept { 
        a.x *= v, a.y *= v; 
        return a;
    }
    friend constexpr auto& operator /= (Self& a, std::type_identity_t<T> v) noexcept {
        a.x /= v, a.y /= v; 
        return a;
    }



    //
    // Unary operators
    //
    friend constexpr Self operator + (Arg a) noexcept { 
        return a;
    }
    friend constexpr Self operator - (Arg a) noexcept { 
        return {-a.x, -a.y};
    }

};

template <class T>
using Point = Point2D<T>;


template <class T>
constexpr Point<T> lerp(Point<T> a, Point<T> b, std::type_identity_t<T> tt) noexcept {
    auto t = static_cast<T>(tt);
    auto t1 = static_cast<T>(1) - t;
    return {a.x*t1 + b.x*t, a.y*t1 + b.y*t};
}

template <class T>
constexpr Point<T> floor(Point<T> a) noexcept { 
    using std::floor;
    using apl::floor;

    return {floor(a.x), floor(a.y)};
}


}  // namespace apl
