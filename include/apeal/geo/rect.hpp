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
#include "size.hpp"
#include "rect.hpp"

namespace apl {

template <class Val>
struct Rect {
    using Self = Rect<Val>;
    using T = Val;
    using Arg = StructParamN<T, 4, Self>;

private:
    using P = IntegParam<T>;
    using A = IntegParam<T>;
    //using FP = StructParamN<T, 4, Self>;
    using RSelf = Self &;
    using SelfRef = Self& ;

    using TP = T *;
    using TR = T &;
    using TCP = const T *;
    using TCR = P ;

    using Ptr = T *;
    using CPtr = const T *;
    using Ref = T &;
    using CRef = P ;

    SelfRef ret() { return *this; }

    using Pt = Point<T>;
    using PointA = typename Point<T>::Arg;

    using Sz = Size<T>;
    using SizeA = typename Size<T>::Arg;

    using RectA = Arg; //IntegParam<typename ARect<T>>;


public:
    using value_type = T;
    using pointer = Ptr;
    using const_pointer = CPtr;
    using reference = Ref;
    using const_reference = CRef;
    using size_type = std::size_t;

    T x, y, w, h;

    //
    // Creating (with type conversion)
    //
    template <class U>
    [[nodiscard]]
    constexpr static Self make(std::span<U> i) noexcept {
        Self r{};
        if (i.size() >= 1) r.x = static_cast<T>(i[0]);
        if (i.size() >= 2) r.y = static_cast<T>(i[1]);
        if (i.size() >= 3) r.w = static_cast<T>(i[2]);
        if (i.size() >= 4) r.h = static_cast<T>(i[3]);
        return r;
    }

    template <class U>
    [[nodiscard]]
    constexpr static Self make(std::initializer_list<U> i) noexcept {
        return make(i.data(), i.size());
    }


    template <class X, class Y, class W, class H>
    [[nodiscard]]
    constexpr static Self make(X ux, Y uy, W uw, H uh) noexcept {
        return {T(ux), T(uy), T(uw), T(uh)};
    }


    [[nodiscard]]
    constexpr static Self make(PointA p, SizeA s) noexcept { return {p.x, p.y, s.w, s.h}; }

    [[nodiscard]]
    constexpr static Self makeCentered(PointA p, SizeA s) noexcept {
        return {p.x - s.w / 2, p.y - s.h / 2, s.w, s.h};
    }

    template <class V>
    [[nodiscard]]
    constexpr static Self makeSquare(PointA p, V s) noexcept { return {p.x, p.y, s, s}; }


    //
    // Testing
    //
    constexpr bool empty() const noexcept { return w == 0 && h == 0; }
    constexpr bool valid() const noexcept { return w != 0 && h != 0; }



    // Conversion to/from other data types
    // It will just initialize with {x, y, w, h}
    template <class P>
    [[nodiscard]]
    constexpr P convert() const noexcept {
        using U = typename P::value_type;
        return {static_cast<U>(x), static_cast<U>(y), static_cast<U>(w), static_cast<U>(h)};
    }

    constexpr std::vector<T> toVector() const noexcept {
        return convert<std::vector<T>>();
    }


    //
    // Modification
    //
    Self& set(PointA p, SizeA sz) {
        x = p.x;
        y = p.y;
        w = sz.w;
        h = sz.h;
        return *this;
    }

    auto& setX(P v) noexcept { x = v; return *this; }
    auto& setY(P v) noexcept { y = v; return *this; }
    auto& setW(P v) noexcept { w = v; return *this; }
    auto& setH(P v) noexcept { h = v; return *this; }

    auto& setPosition(PointA p) noexcept {
        x = p.x;
        y = p.y;
        return *this;
    }

    auto& setSize(SizeA sz) noexcept {
        w = sz.w;
        h = sz.h;
        return *this;
    }


    [[deprecated]] constexpr Rect updatedX(P v) const noexcept { return {v, y, w, h}; }
    [[deprecated]] constexpr Rect updatedY(P v) const noexcept { return {x, v, w, h}; }
    [[deprecated]] constexpr Rect updatedW(P v) const noexcept { return {x, y, v, h}; }
    [[deprecated]] constexpr Rect updatedH(P v) const noexcept { return {x, y, w, v}; }


    constexpr Self updX(P v) const noexcept { return {v, y, w, h}; }
    constexpr Self updY(P v) const noexcept { return {x, v, w, h}; }
    constexpr Self updW(P v) const noexcept { return {x, y, v, h}; }
    constexpr Self updH(P v) const noexcept { return {x, y, w, v}; }

    SelfRef expandX(A v) noexcept { x -= v, w += v;  return *this; }
    SelfRef expandY(A v) noexcept { y -= v, h += v;  return *this; }
    SelfRef expandW(A v) noexcept { w += v;  return *this; }
    SelfRef expandH(A v) noexcept { h += v;  return *this; }

    SelfRef shrinkX(P v) noexcept { x += v, w -= v;  return *this; }
    SelfRef shrinkY(P v) noexcept { y += v, h -= v;  return *this; }
    SelfRef shrinkW(P v) noexcept { w -= v;  return *this; }
    SelfRef shrinkH(P v) noexcept { h -= v;  return *this; }

    template <class Margins>
    SelfRef shrinkMargins(const Margins& m) noexcept {
        w -= m.horz2();
        x += m.left();
        h -= m.vert2();
        y += m.bottom();
        return *this;
    }

    Rect& expand(P v) noexcept {
        auto z = v / 2;
        x -= z, y -= z;
        w += v, h += v;
        return *this;
    }

    Rect& shrink(P v) noexcept {
        auto z = v / 2;
        x += z, y += z;
        w -= v, h -= v;
        return *this;
    }

    [[nodiscard]] constexpr Rect expanded(P v) noexcept {
        auto z = v / 2;
        return {x - z, y - z, w + v, h + v};
    }

    [[nodiscard]] constexpr Rect shrank(P v) noexcept {
        auto z = v / 2;
        return {x + z, y + z, w - v, h - v};
    }


    Rect& offsetX(P v) noexcept { x += v;  return *this; }
    Rect& offsetY(P v) noexcept { y += v;  return *this; }
    Rect& offset(PointA p) { x += p.x, y += p.y; return *this; }

    constexpr Rect &move(PointA p) noexcept { x = p.x, y = p.y; return *this; }
    constexpr Rect &resize(SizeA s) noexcept { w = s.w, h = s.h; return *this; }

    constexpr Rect& scale(A s) noexcept { x *= s, y *= s, w *= s, h *= s; return *this; }
    constexpr Rect& mul(Arg r) noexcept { x *= r.x, y *= r.y, w *= r.w, h *= r.h; return *this; }


    Self &zoomAdd(P v) noexcept { x -= v, y -= v, w += v*2, h += v*2;  return *this; }
    Self &zoomAdd(PointA v) noexcept { x -= v.x, y -= v.y, w += v.x*2, h += v.y*2;  return *this; }
    //Self &zoomSub(P v) noexcept { x += v, y += v, w -= v*2, h -= v*2;  return *this; }
    //Self &zoomSub(PointA v) noexcept { x += v.x, y += v.y, w -= v.x*2, h -= v.y*2;  return *this; }

    constexpr Self zoomedAdd(P v) const noexcept { return {x - v, y - v, w + v*2, h + v*2}; }
    constexpr Self zoomedAdd(PointA v) const noexcept { return {x - v.x, y - v.y, w + v.x*2, h + v.y*2}; }
    constexpr Self zoomedSub(P v) const noexcept { return {x + v, y + v, w - v*2, h - v*2}; }
    constexpr Self zoomedSub(PointA v) const noexcept { return {x + v.x, y + v.y, w - v.x*2, h - v.y*2}; }

    SelfRef mix(Arg c, P alpha) noexcept {
        const auto alpha1 = T(1) - alpha;
        x = x * alpha1 + c.x * alpha;
        y = y * alpha1 + c.y * alpha;
        w = w * alpha1 + c.w * alpha;
        h = h * alpha1 + c.h * alpha;
        return *this;
    }

    constexpr Self off(PointA v) const noexcept { return {x + v.x, y + v.y, w, h}; }
    constexpr Self offRect(Arg v) const noexcept { return {x + v.x, y + v.y, w + v.w, h + v.h}; }

    constexpr Self subframe() const noexcept { return {0, 0, w, h}; }


    // Sets the height of the size based on given ratio
    Self &setRatioHeight(Arg ratio) noexcept { h = w * ratio.h / ratio.w;  return *this; }
    Self &setRatioWidth(Arg ratio) noexcept { w = h * ratio.w / ratio.h;  return *this; }
    Self &setRatioHeight(SizeA ratio) noexcept { h = w * ratio.h / ratio.w;  return *this; }
    Self &setRatioWidth(SizeA ratio) noexcept { w = h * ratio.w / ratio.h;  return *this; }

    // For systems where coordinates start from the top
    SelfRef flip(P sh) noexcept { y = sh - h - y;  return *this; }
    [[nodiscard]] constexpr Self flipped(P sh) const noexcept { return {x, sh - h - y, w, h}; }


    [[nodiscard]] constexpr Self centered(SizeA b) const noexcept {
        return {x + (w-b.w)/2, y + (h-b.h)/2, b.w, b.h};
    }

    [[nodiscard]] constexpr Self centered(T s) const noexcept {
        return {x + (w-s) * 0.5, y + (h-s) * 0.5, s, s};
    }

    [[nodiscard]] constexpr Self positioned(PointA k) const noexcept {
        return {x + w * k.x, y + h * k.y, 0, 0};
    }
    [[nodiscard]] constexpr Self positioned(SizeA b, PointA k) const noexcept {
        return {x + (w-b.w) * k.x, y + (h-b.h) * k.y, b.w, b.h};
    }

    [[nodiscard]] constexpr bool contains(PointA p) const noexcept {
        return p.x >= x && p.x < x + w && p.y >= y && p.y < y + h;
    }
    [[nodiscard]] constexpr bool intersects(Arg b) const noexcept {
        return contains({b.x, b.y})
               || contains({b.x + b.w, b.y})
               || contains({b.x, b.y + b.h})
               || contains({b.x + b.w, b.y + b.h});
    }

    [[nodiscard]] constexpr Self intersection(Arg b) const noexcept {
        const T right = std::min(x + w, b.x + b.w);
        const T top = std::min(y + h, b.y + b.h);
        const T left = std::max(x, b.x);
        const T bottom = std::max(y, b.y);
        return {left, bottom, right-left, top-bottom};
    }

    [[nodiscard]] constexpr auto right() const noexcept { return x + w; }
    [[nodiscard]] constexpr auto top() const noexcept { return y + h; }
    [[nodiscard]] constexpr auto left() const noexcept { return x; }
    [[nodiscard]] constexpr auto bottom() const noexcept { return y; }

    [[nodiscard]] constexpr Pt leftTop() const noexcept { return {x, y + h}; }
    [[nodiscard]] constexpr Pt rightBottom() const noexcept { return {x + w, y}; }
    [[nodiscard]] constexpr Pt extent() const noexcept { return {x + w, y + h}; }


    [[nodiscard]] constexpr Pt pos() const noexcept { return {x, y}; }
    [[nodiscard]] constexpr Pt point() const noexcept { return {x, y}; }
    [[nodiscard]] constexpr Pt position() const noexcept { return {x, y}; }
    [[nodiscard]] constexpr Pt position(PointA v) const noexcept {
        return {x + w * v.x, y + h * v.y};
    }

    [[nodiscard]] constexpr Pt center() const noexcept { return {x + w/2, y + h/2}; }

    [[nodiscard]] constexpr Sz size() const noexcept { return {w, h}; }

    constexpr Self& add(PointA p) noexcept {
        x += p.x, y += p.y;
        return *this;
    }

    [[deprecated]] constexpr Self floored() const noexcept {
        return {std::floor(x), std::floor(y), std::floor(w), std::floor(h)};
    }
    constexpr Self floor() const noexcept {
        return {std::floor(x), std::floor(y), std::floor(w), std::floor(h)};
    }


    //
    // Operator overloading
    //

    // Testing
    constexpr explicit operator bool() const noexcept { return x || y || w || h; }
    auto operator<=>(const Rect&) const noexcept = default;



    // // Accessing
    // constexpr auto& operator[](Index i) noexcept { return this->data()[i]; }
    // constexpr const auto& operator[](Index i) const noexcept { return this->data()[i]; }


    // Compound asignment
    constexpr Rect& operator *= (A b) noexcept { return scale(b); }

    constexpr Rect& operator += (RectA b) noexcept { x += b.x, y += b.y, w += b.w, h += b.h;  return *this; }
    constexpr Rect& operator -= (RectA b) noexcept { x -= b.x, y -= b.y, w -= b.w, h -= b.h;  return *this; }

    constexpr Rect& operator += (Point<T> b) noexcept { x += b.x, y += b.y;  return *this; }
    constexpr Rect& operator -= (Point<T> b) noexcept { x -= b.x, y -= b.y;  return *this; }


    // Comparison
    friend constexpr bool operator == (RectA a, RectA b) noexcept { return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h; }
    friend constexpr bool operator != (RectA a, RectA b) noexcept { return !(a == b); }


    friend constexpr Rect operator + (Rect a, RectA b) noexcept { a += b; return a; }
    friend constexpr Rect operator - (Rect a, RectA b) noexcept { a -= b; return a; }

    friend constexpr Rect operator * (RectA a, A b) noexcept { return {a.x * b, a.y * b, a.w * b, a.h * b}; }
    friend constexpr Rect operator / (RectA a, A b) noexcept { return {a.x / b, a.y / b, a.w / b, a.h / b}; }


    // Convenenient scaling overloads for Size and Point types
    constexpr Rect operator * (Size<T> s) const noexcept { return {x * s.w, y * s.h, w * s.w, h * s.h}; }
    constexpr Rect operator / (Size<T> s) const noexcept { return {x / s.w, y / s.h, w / s.w, h / s.h}; }

    constexpr Rect operator * (Point<T> s) const noexcept { return {x * s.w, y * s.h, w * s.w, h * s.h}; }
    constexpr Rect operator / (Point<T> s) const noexcept { return {x / s.x, y / s.y, w / s.x, h / s.y}; }


    // Offset
    friend constexpr Rect operator + (RectA a, PointA b) noexcept { return {a.x + b.x, a.y + b.y, a.w, a.h}; }
    friend constexpr Rect operator - (RectA a, PointA b) noexcept { return {a.x - b.x, a.y - b.y, a.w, a.h}; }


    // Unary arithmetic
    friend constexpr Rect operator + (RectA a) noexcept { return a; }
    friend constexpr Rect operator - (RectA a) noexcept { return {-a.x, -a.y, -a.w, -a.h}; }
};


template <class T>
constexpr Rect<T> lerp(const Rect<T>& a, const Rect<T>& b, std::type_identity_t<T> tt) noexcept {
    auto t = static_cast<T>(tt);
    auto t1 = T(1) - t;
    return {a.x*t1 + b.x*t, a.y*t1 + b.y*t, a.w*t1 + b.w*t, a.h*t1 + b.h*t};
}

template <class T>
constexpr auto floor(typename Rect<T>::Arg r) noexcept { return r.floor(); }



}  // namespace apl
