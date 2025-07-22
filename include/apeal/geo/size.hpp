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

template <class Val>
struct Size {
    using T = Val;

private:
    using Self = Size<Val>;
    using SelfRef = Self&;
    using Self_cr = const Self&;

    using Sz = Size<T>;
    using Pt = Point<T>;

    using Ptr = T*;
    using CPtr = const T*;
    using Ref = T&;
    using CRef = const T&;


public:
    using P = IntegParam<T>;
    using FP = StructParamN<T, 2, Self>;

    // Self function argument
    using Arg = StructParamN<T, 2, Self>;
    using SA = StructParamN<T, 2, Self>;


    using value_type = T;
    using pointer = Pt;
    using const_pointer = CPtr;
    using reference = Ref;
    using const_reference = CRef;
    using size_type = std::size_t;


    T w, h;

    constexpr bool empty() const noexcept { return w == 0 && h == 0; }
    constexpr bool valid() const noexcept { return w != 0 && h != 0; }



    // Convenience assignments from various types
    // in order to avoid annoying conversions
    template <class U>
    Size& assign(U u, U v) noexcept{
        w = T(u), h = T(v);
        return *this;
    }

    template <class U>
    constexpr Size& assign(std::initializer_list<U> i) noexcept {
        w = i.size() >= 1 ? static_cast<T>(i[0]) : 0;
        h = i.size() >= 2 ? static_cast<T>(i[1]) : 0;
        return *this;
    }

    template <typename U, typename V>
    constexpr static Size make(U u, V v) noexcept {
        return {static_cast<T>(u), static_cast<T>(v)};
    }


    //
    // Accessing Data
    //
    constexpr auto get() noexcept { return Ptr(this); }
    constexpr auto get() const noexcept { return CPtr(this); }

    constexpr auto data() noexcept { return Ptr(this); }
    constexpr auto data() const noexcept { return CPtr(this); }

    constexpr auto span() noexcept {
        static_assert(sizeof(Size)==sizeof(T[2]), "Size is not equal to T[2]");
        return std::span<T, 2>{Ptr(this), 2};
    }
    constexpr std::span<T, 2> span() const noexcept {
        static_assert(sizeof(Size)==sizeof(T[2]), "Size is not equal to T[2]");
        return {CPtr(this), 2};
    }


    // Conversion to/from other data types
    // It will just initialize with {w, h}
    template <class PointT, typename U = typename PointT::value_type>
    constexpr PointT convert() const noexcept {
        //using U = typename Point::value_type;
        return {U(w), U(h)}; 
    }

    //template <class Point>
    //auto convert(const Point &u) { 
    //    using U = typename P::value_type;  
    //    u = {U(w), U(h)};  return ret(); 
    //}




    // Sets the height of the size based on given ratio
    Self &setRatioHeight(FP ratio)  noexcept { h = w * ratio.h / ratio.w;  return *this; }
    Self &setRatioWidth(FP ratio)  noexcept { w = h * ratio.w / ratio.h;  return *this; }


    Self &scale(P s)  noexcept { w *= s, h *= s; return *this; }
    constexpr Self scaled(P s)  noexcept { return {w * s, h * s}; }

    Self &resize(P sw, P sh) { w = sw, h = sh; return *this; }

    constexpr Self scaled(FP s) const  noexcept {
        if (s.w == 0 && s.h == 0)
            return *this;
        if (s.w == 0)
            return {s.h * w / h, s.h};
        if (s.h == 0)
            return {s.w, s.w * h / w};
        return s;
    }

    template <class Rect>
    constexpr Rect rect() const  noexcept { 
        using U = typename Rect::value_type; 
        return {0, 0, (U)w, (U)h}; 
    }

    //template <class Point>
    constexpr Point<T> point() const  noexcept { return {w, h}; }
    constexpr Point<T> center() const  noexcept { return {w/2, h/2}; }
    constexpr Size<T> half() const  noexcept { return {w/2, h/2}; }

    constexpr Self off(SA p) const noexcept { return { w + p.w, h + p.h } ;  }


    //
    // Operator overloading
    //
    constexpr explicit operator bool() const { return w || h; }

    auto operator<=>(const Size&) const noexcept = default;


    constexpr Self& operator[](size_type i) { return data()[i]; }
    constexpr const Self& operator[](size_type i) const { return data()[i]; }


    constexpr bool operator == (Arg b) const  noexcept { return w == b.w && h == b.h; }
    constexpr bool operator != (Arg b) const  noexcept { return !operator==(b); }




    //
    // Binary operators
    //
    friend constexpr Self operator + (Arg a, Arg b) noexcept { 
        return {a.w + b.w, a.h + b.h}; 
    }
    friend constexpr Self operator - (Arg a, Arg b) noexcept { 
        return {a.w - b.w, a.h - b.h}; 
    }
    friend constexpr Self operator * (Arg a, Arg b) noexcept { 
        return {a.w * b.w, a.h * b.h}; 
    }
    friend constexpr Self operator / (Arg a, Arg b) noexcept { 
        return {a.w / b.w, a.h / b.h}; 
    }

    // Scalars
    friend constexpr Size operator * (Arg a, std::type_identity_t<T> b) noexcept { 
        return {a.w * b, a.h * b}; 
    }

    friend constexpr Size operator / (FP a, std::type_identity_t<T> b) noexcept { 
        return {a.w / b, a.h / b}; 
    }

    friend constexpr Size operator + (FP a, std::type_identity_t<T> b) noexcept { 
        return {a.w + b, a.h + b}; 
    }

    friend constexpr Size operator - (FP a, std::type_identity_t<T> b) noexcept { 
        return {a.w - b, a.h - b}; 
    }


    //
    // Binary assignment operators
    //
    friend constexpr auto& operator += (Self& a, Arg b) noexcept { 
        a.w += b.w, a.h += b.h; 
        return a;
    }
    friend constexpr auto& operator -= (Self& a, Arg b) noexcept {
        a.w -= b.w, a.h -= b.h; 
        return a;
    }
    friend constexpr auto& operator *= (Self& a, Arg b) noexcept { 
        a.w *= b.w, a.h *= b.h; 
        return a;
    }
    friend constexpr auto& operator /= (Self& a, Arg b) noexcept {
        a.w /= b.w, a.h /= b.h; 
        return a;
    }


    friend constexpr auto& operator += (Self& a, std::type_identity_t<T> v) noexcept { 
        a.w += v, a.h += v; 
        return a;
    }
    friend constexpr auto& operator -= (Self& a, std::type_identity_t<T> v) noexcept {
        a.w -= v, a.h -= v; 
        return a;
    }
    friend constexpr auto& operator *= (Self& a,std::type_identity_t<T> v) noexcept { 
        a.w *= v, a.h *= v; 
        return a;
    }
    friend constexpr auto& operator /= (Self& a, std::type_identity_t<T> v) noexcept {
        a.w /= v, a.h /= v; 
        return a;
    }



    //
    // Unary operators
    //
    friend constexpr Self operator + (Arg a) noexcept { 
        return a;
    }
    friend constexpr Self operator - (Arg a) noexcept { 
        return {-a.w, -a.h};
    }



    //
    // Mixing points and sizes
    //
    friend constexpr Pt operator * (Point<T> a, FP b) noexcept { return {a.x * b.w, a.y * b.h}; }
    friend constexpr Pt operator / (Point<T> a, FP b) noexcept { return {a.x / b.w, a.y / b.h}; }
    friend constexpr Pt operator + (Point<T> a, FP b) noexcept { return {a.x + b.w, a.y + b.h}; }
    friend constexpr Pt operator - (Point<T> a, FP b) noexcept { return {a.x - b.w, a.y - b.h}; }


    friend constexpr auto& operator += (Pt& a, Arg b) noexcept { 
        a.x += b.w, a.y += b.h;
        return a; 
    }

    friend constexpr auto& operator -= (Pt& a, Arg b) noexcept { 
        a.x -= b.w, a.y -= b.h;
        return a; 
    }



};


template <class T>
constexpr Size<T> lerp(Size<T> a, Size<T> b, std::type_identity_t<T> tt) noexcept {
    auto t = static_cast<T>(tt);
    auto t1 = static_cast<T>(1) - t;
    return {a.w*t1 + b.w*t, a.h*t1 + b.h*t};
}

template <class T>
constexpr Size<T> floor(Size<T> a) noexcept { 
    using std::floor;
    using apl::floor;

    return {floor(a.w), floor(a.h)};
}


} // namespace 

