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

class OleVariant {
public :
    VARIANT v;

public :
    OleVariant() noexcept
    {
        VariantInit(&v) ;
    }

    ~OleVariant() noexcept
    {
        release() ;
    }


    void Assign_BSTR ( const std::wstring &str )
    {
        release() ;

        v.bstrVal = SysAllocString(str.c_str()) ;

        if ( v.bstrVal == 0 )
            throw 1 ;
        v.vt = VT_BSTR ;
    }

    void Assign_BSTR ( const std::string &str )
    {
      release() ;

      wchar_t *wstr = NULL ;
      size_t slen = str.length() ;

      if ( (wstr = new wchar_t[slen+1]) == NULL )
        throw 0 ;

#if __APEAL_HAVE_MBSTOWCS_S
        size_t charactersConverted ;
        mbstowcs_s ( &charactersConverted, wstr, slen+1, str.c_str(), slen+1 ) ;
#else
        mbstowcs ( wstr, str.c_str(), slen+1 ) ;
#endif

      if ( (v.bstrVal=SysAllocString(wstr)) == NULL )
        throw this ;

      v.vt = VT_BSTR ;

      delete wstr ;
    }

    void Assign_UI4 ( long val )
    {
      release() ;
      v.vt = VT_UI4 ;
      v.lVal = val ;
    }

    void Assign_BOOL ( bool val )
    {
      release() ;
      v.vt = VT_BOOL ;
      v.boolVal = val ;
    }


public :
    void release()
    {
        if ( v.vt == VT_BSTR && v.bstrVal )
            ::SysFreeString(v.bstrVal), v.bstrVal = 0 ;

        VariantClear(&v) ;
    }
} ;

}
