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
#include "noncopyable.hpp"

#include <cstdlib>
#include <vector>
#include <stack>

namespace apl {


struct SingletonBase: NonCopyable {
    virtual ~SingletonBase() noexcept {};
};

// Singleton of singletons
// NOTE: Not thread safe
class SingletonManager {
    using Self = SingletonManager;

public:
    static void atexit_handler() {
        instance()->shutdown();
    }

    static Self *instance() {

        static Self *instance_ = [] {
            auto i = new Self;
            //std::atexit(atexit_handler) ;
            return i;
        }();

        return instance_;
    };

    void add(SingletonBase *s) noexcept {
        singletons_.push_back(s);
        //singletons_.push(s);
    }

    void remove(SingletonBase* s) noexcept {
        std::erase(singletons_, s);
    }

    void shutdown() noexcept {
        while (!singletons_.empty()) {
            auto s = singletons_.front();
            singletons_.erase(singletons_.begin() + singletons_.size() - 1);
            delete s;
        }

        //for (auto rit = singletons_.rbegin(); rit != singletons_.rend(); ++rit) {
        //    delete *rit;
        //}

        //singletons_.clear();

        //while (!singletons_.empty()) {
        //    auto it = singletons_.top();
        //    singletons_.pop();
        //    delete it;
        //    
        //    //auto it = singletons_.begin();
        //    //it += singletons_.size() - 1;
        //    //delete *it;
        //    //singletons_.erase(it);
        //}

        //for (auto& i: singletons_) {
        //    delete i;
        //    i = nullptr;
        //}
        //singletons_.clear();
    }

private:
    std::vector<SingletonBase*> singletons_;
    //std::stack<SingletonBase*> singletons_;
};


template <class T, bool Reg = true>
class Singleton: public SingletonBase {
public:
    friend T;
    using instance_type = T*;

private:
    Singleton() {}

public:
    static T *instance(bool doRelease = false) {
        // Using the C++11 thread safety guarantees
        // for static variables initialization
        static T *instance_ = []() -> T* {
            auto i = new T;
            if (Reg) SingletonManager::instance()->add(i);
            return i;
        }();

        if (!doRelease && !instance_) {
            instance_ = new T;
            if (Reg) SingletonManager::instance()->add(instance_);
        }

        if (doRelease) {
            SingletonManager::instance()->remove(instance_);
            instance_ = nullptr;
        }

        return instance_;
    }

    //static auto get() {  return instance();  }

    virtual ~Singleton() noexcept {
        instance(true);
    }

    T* release() noexcept {
        //instance(true);
        delete this;
        return nullptr;
    }
};

}  // namespace apl
