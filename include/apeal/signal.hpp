/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "debug.hpp"

#include <memory>
#include <functional>

namespace apl {

namespace impl {
// Have to use virtual calls
template <class ...Args>
struct SlotBase {
    //virtual void invoke(Args&&...) = 0 ;
    virtual void invoke(Args...) = 0;
    virtual void *slot() const = 0;

    virtual ~SlotBase() {}
};


template <class C, class ...Args>
struct SlotItem: public SlotBase<Args...> {

    using M = void (C:: *)(Args...);

    M m{};  // slot method
    C *c{};  // slot class

    SlotItem(M im, C *ic): m{im}, c{ic} {}

protected:
    //void invoke(Args&&... args) override { (c->*m)(args...) ;  }
    void invoke(Args... args) override { (c->*m)(args...); }
    void *slot() const override { return (void *)c; }
};

}  // namespace apl::impl

//
// Class is not copyable but movable
//
template <class ...Args>
struct Signal {
    using Self = Signal;

    //
    // Initialization
    //
    constexpr Signal() noexcept {}

    // Just in case somebody wants to have a signal as a parameter like this: 
    //    auto f(Signal<T> = 0)
    constexpr Signal(std::nullptr_t) noexcept{}  

    // Make it Noncopyable
    Signal(const Signal&) = delete;
    Signal &operator=(const Signal&) = delete;

    // Move constructor
    Signal(Signal&& rhs) noexcept { swap(*this, rhs); }

    // Move assignment operator
    Signal &operator = (Signal&& rhs) noexcept {
        swap(*this, rhs);
        return *this;
    }

    ~Signal() noexcept {}


    //
    // Assignment operators
    //
    std::nullptr_t operator=(std::nullptr_t) noexcept { 
        disconnect();  
        return nullptr; 
    }


    // There isn't really any need for the destructor. Things get releaseed automatically
    // 
    //~Signal() noexcept { disconnect() ; }


    friend void swap(Signal &a, Signal &b) noexcept {
        using std::swap;
        swap(a.slot, b.slot);
        swap(a.func, b.func);
    }


    //
    // Connecting
    //

    // Connecting Class::Method
    // Follow the std::bind to avoid confusion
    template <class C>
    void connect(typename impl::SlotItem<C, Args...>::M&& m, C* c)
    {
        slot = std::make_unique<impl::SlotItem<C, Args...>>(m, c);
        func = nullptr;
    }

    // Connecting lambda or function
    // convenient but might be less efficient
    using Func = std::function<void(Args...)>;
    Func func{};

    template <class F>
    void connect(F f) noexcept {
        func = f;
        slot.reset();
    }

    // Assignment operator of the functional
    void operator=(Func f) noexcept { connect(f); }

    //
    // Disconnecting
    //
    void disconnect() noexcept {
        slot.reset();
        func = nullptr;
    }

    //
    // Disconnection through assignment
    // assigning zero or false disconnects the signal
    //


    //
    // Testing, Status
    //
    bool empty() const noexcept { return !!func && !!slot; }
    explicit operator bool() const noexcept { return !func || !slot; }


    //
    // Invoking, emitting
    //
    //template <class ...Parms>
    //void emit(Parms && ...args) {
    //    if (slot_)
    //        slot_->invoke(std::forward<Parms>(args)...);
    //    else
    //        if (f)
    //            f(std::forward<Parms>(args)...);
    //}

    void emit(Args ...args) {
        if (slot)
            slot->invoke(args...);
        else if (func)
            func(args...);
    }



    // Operators
    //template <class ...Parms>
    //void operator()(Parms&& ...args){  emit(std::forward<Parms>(args)...) ;  }

    void operator()(Args ...args) { emit(args...); }

private:
    std::unique_ptr<impl::SlotBase<Args...>> slot;

};


template <class ...Args>
using Slot = typename Signal<Args...>::Func;

}  // namespace apl

