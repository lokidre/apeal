/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../def.hpp"

#if APEAL_OS_WINDOWS

#include <oleauto.h>
#include <objbase.h>

#pragma comment (lib,"oleaut32.lib")

#include "../noncopyable.hpp"


namespace apl {

template <class Interface>
class ComInterface: public NonCopyable {
public:
    Interface* disp{};

    constexpr ComInterface() noexcept {}

    ComInterface(REFCLSID rclsid, const LPUNKNOWN& pUnkOuter, DWORD dwClsContext, REFIID riid) {
        CreateInstance(rclsid, pUnkOuter, dwClsContext, riid);
    }

    template <class ParentInterface>
    ComInterface(REFIID iid, ComInterface<ParentInterface>& ifc) {
        ifc.QueryInterface(iid, *this);
    }

    ~ComInterface() noexcept { release(); }


    HRESULT CreateInstance(REFCLSID rclsid, const LPUNKNOWN& pUnkOuter, const DWORD dwClsContext, REFIID riid)
    {
        HRESULT hr = S_OK;

        static HRESULT co_initialize_result_ = S_FALSE;

        if (co_initialize_result_ != S_OK) {
            co_initialize_result_ = CoInitializeEx(NULL, COINIT_MULTITHREADED);
            //co_initialize_result_= CoInitialize ( NULL ) ;

            // this is strange logic, but when the return value is S_FALSE 
            // it means that COM was already successfuly initialized before
            if (co_initialize_result_ == S_FALSE)
                co_initialize_result_ = S_OK;
        }

        hr = CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, (void**)&disp);

        if (hr != S_OK)
            throw std::runtime_error("com.cxx: unable to CoCreateInstance");

        return hr;
    }

    template <class QueryInterface>
    HRESULT QueryInterface(REFIID iid, ComInterface<QueryInterface>& iface)
    {
        HRESULT hr = S_OK;

        hr = disp->QueryInterface(iid, (void**)&iface.disp);

        if (hr != S_OK)
            throw std::runtime_error("com:cxx: unable to QueryInterface");

        return hr;
    }

    void release() noexcept
    {
        if (disp) {
            disp->Release();
            disp = nullptr;
        }
    }


    Interface* operator->()
    {
        if (!disp)
            throw std::runtime_error("com.hpp: null interface called");

        return disp;
    }

    bool valid() const noexcept { return disp ? true : false; }
};

}  // namespace apl

#endif // target os window

