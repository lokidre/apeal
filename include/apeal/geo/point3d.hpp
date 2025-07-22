/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "point.hpp"

namespace apl {

template <class T_>
struct Point3D {
public:
    using T = T_;
    using Arg = const Point3D<T>&;

private:
    using Self = Point3D<T>;

    using RSelf = Self&;

    using Ptr = T*;
    using CPtr = const T*;
    using Ref = T&;
    using CRef = const T&;

    RSelf ret() { return *this; }


public:
    using value_type = T;
    using pointer = Ptr;
    using const_pointer = CPtr;
    using reference = Ref;
    using const_reference = CRef;


    T x, y, z;


    //
    // Initalization and assignments
    //

    // from lower dimansional vector
    template <class P>
    auto& assign2D(const P& u, T _z = {}) noexcept {
        x = u.x, y = u.y;  
        z = _z;
        return *this; 
    }

    // Conversion to/from other data types
    // It will just initialize with {x, y, z}
    template <class P>
    constexpr P convert() const {
        using U = typename P::value_type;
        return {U(x), U(y), U(z)}; 
    }

    //template <class P>
    //Self &convert(const P &u) {  return { x = T(u.x), y = T(u.y), z = T(z) } ;  }


    // to lower dimensional vector
    template <class P>
    P point2D() const noexcept { return {x, y}; }


    //
    // Accessing
    //
    constexpr Ptr get() { return Ptr(this); }
    constexpr CPtr get() const { return CPtr(this); }

    constexpr Ptr data() { return Ptr(this); }
    constexpr CPtr data() const { return CPtr(this); }


    T norm() const { return std::hypot(x, y, z); }
    T distance(const Self& u) const { return std::hypot(x-u.x, y-u.y, z-u.z); }

    Self& normalize() {
        auto l = std::hypot(x, y, z);
        x /= l, y /= l, z /= l;
        return *this;
    }
    Self normalized() const {
        auto l = std::hypot(x, y, z);
        return {x / l, y / l, z / l};
    }


    Self& cross(const Self& q) {
        Self p{x, y, z};
        x = p.y * q.z - p.z * q.y;
        y = p.z * q.x - p.x * q.z;
        z = p.x * q.y - p.y * q.x;
        return *this;
    }

    Self crossed(const Self& q) const {
        return {y * q.z - z * q.y, z * q.x - x * q.z, x * q.y - y * q.x};
    }


    Self& scale(const T& s) noexcept { x *= s, y *= s, z *= s;  return *this; }
    Self scaled(const T& s) const noexcept { return {x * s, y * s, z * s}; }


    Self& sub(const Self& u) { x -= u.x, y -= u.y, z -= u.z;  return *this; }
    Self& add(const Self& u) { x += u.x, y += u.y, z += u.z;  return *this; }


    //
    // Operator overloading
    //
    explicit operator bool() const noexcept { return x || y || z; }

    T& operator[](Index i) { return get()[i]; }

    constexpr auto& operator *= (std::type_identity_t<T> s) noexcept { x *= s, y *= s, z *= s;  return *this; }
    constexpr auto& operator /= (std::type_identity_t<T> s) noexcept { x /= s, y /= s, z /= s;  return *this; }
    //constexpr auto& operator += (std::type_identity_t<T> s) noexcept { x += s, y += s, z += s;  return *this; }
    //constexpr auto& operator -= (std::type_identity_t<T> s) noexcept { x -= s, y -= s, z -= s;  return *this; }

    constexpr auto& operator += (Arg u) noexcept { x += u.x, y += u.y, z += u.z; return *this; }
    constexpr auto& operator -= (Arg u) noexcept { x -= u.x, y -= u.y, z -= u.z; return *this; }

    Self operator - (const Self& u) const { return {x - u.x, y - u.y, z - u.z}; }
    Self operator + (const Self& u) const { return {x + u.x, y + u.y, z + u.z}; }


    friend constexpr Self operator - (Arg a) noexcept { return {-a.x, -a.y, -a.z}; }
    friend constexpr Self operator + (Arg a) noexcept { return a; }


    friend constexpr bool operator == (Arg a, Arg b) noexcept { return a.x == b.x && a.y == b.y && a.z == b.z; }
    friend constexpr bool operator != (Arg a, Arg b) noexcept { return !(a == b); }

};

template <class T>
Point3D<T> cross(Point3D<T> p, Point3D<T> q) noexcept
{
    return {
        p.y * q.z - p.z * q.y, 
        p.z * q.x - p.x * q.z, 
        p.x * q.y - p.y * q.x
    };
}


}  // namespace apl

