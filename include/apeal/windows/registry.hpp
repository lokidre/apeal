/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../buffer.hpp"

#if APEAL_OS_WINDOWS

#include "../error.hpp"
#include "../policy.hpp"

#include <string>


namespace apl {

class WinRegistryKey : public AErrorProducer {
public :
    using string_type = String ;
    using ErrorPolicy = AErrorPolicy ;

private :
    HKEY handle_{} ;

public :
    WinRegistryKey() noexcept {}

    ~WinRegistryKey() noexcept
    {
        if ( handle_ )
            close() ;
    }

public :

    //enum HKey {
    //  hkey_classes_root = HKEY_CLASSES_ROOT,
    //  hkey_current_user = HKEY_CURRENT_USER,
    //  hkey_local_machine = HKEY_LOCAL_MACHINE,
    //  hkey_users = HKEY_USERS
    //} ;

    enum Access {
        access_read = KEY_READ,
        access_all = KEY_ALL_ACCESS
    } ;


    bool open ( HKEY hkey, StrView skey, Access acc = access_read )
    {
        if ( ( this->errorCode = ::RegOpenKeyExW ( hkey, ApiString(skey).win(), 0, acc, &handle_ ) ) != ERROR_SUCCESS )
            return this->raiseError("RegOpenKeyEx") ;
        return true ;
    }


    bool create ( HKEY hClassKey, StrView sKey, uint32_t Access = KEY_ALL_ACCESS )
    {
        DWORD disposition ;
        if ( ( this->errorCode = RegCreateKeyExW ( hClassKey,
                        ApiString{sKey}.win(),
                        0,   // Reserved
                        0,  // Class name
                        REG_OPTION_NON_VOLATILE,  // options
                        Access,
                        0,  // Security attributes
                        &handle_,  // New Key value
                        &disposition  // opened or created
                        ) ) != ERROR_SUCCESS )
            return this->raiseError("RegCreateKeyEx") ;
        return true ;
    }


    void close()
    {
        if ( handle_ ) {
            ::RegCloseKey ( handle_ ) ;
            handle_ = 0 ;
        }
    }


    /*

    bool DeleteKey ( const char *Name )
    {
      if ( ::RegDeleteKey ( KeyHandle, Name ) != ERROR_SUCCESS )
        return false ;
      return true ;
    }


    bool DeleteVal ( char *Name )
    {
      if ( ::RegDeleteValue ( KeyHandle, Name ) != ERROR_SUCCESS )
        return false ;
      return true ;
    }

    */

    bool get ( const string_type &name, string_type &value )
    {
        Buffer<BYTE> buffer ;
        buffer.resize(256) ;
        DWORD buffer_size ;

        for (;;) {
            buffer_size = (DWORD)buffer.size() ;
            this->errorCode = ::RegQueryValueExW ( handle_, ApiString{name}.win(), 0, 0, buffer.get(), &buffer_size ) ;

            if ( this->errorCode != ERROR_MORE_DATA )
                break ;

            buffer.resize(buffer_size*2) ;
        }

        if ( this->errorCode == ERROR_FILE_NOT_FOUND ) {
            value.clear() ;
            return false ;
        }

        if ( this->errorCode != ERROR_SUCCESS )
            return this->raiseError("ReqQueryValueEx") ;

        value = (char *)buffer.get() ;

        return true ;
    }

    //string_type value ( const string_type &name, const )
    //{
    //}

    int int_value ( StrView name, int default_value = 0 ) { return intValue(name,default_value) ;  }


    int intValue ( StrView name, int default_value = 0 )
    {
        int value ;
        DWORD size = sizeof(value) ;
        DWORD type ;
        int error_code = ::RegQueryValueExW ( handle_, ApiString{name}.win(), 0, &type, (BYTE *)&value, &size ) ;
        if ( error_code == ERROR_SUCCESS )
            return value ;
        //if ( error_code == ERROR_MORE_DATA )
        //  ::Reg
        return default_value ;
    }

    bool set_value ( StrView name, StrView value ) {  return setValue(name,value) ;  }

    bool setValue ( StrView name, StrView value )
    {
        this->errorCode = ::RegSetValueExW (
            handle_,
            ApiString{name}.win(),
            0,  // Reserved
            REG_SZ,
            (LPBYTE)value.c_str(),
            (DWORD)value.length()
        ) ;

        if ( this->errorCode != ERROR_SUCCESS )
            return this->raiseError("ReqSetValueEx") ;

        return true ;
    }

    bool set_value ( const string_type &name, int value ) { return setValue(name,value) ;  }

    bool setValue ( const string_type &name, int value )
    {
        this->errorCode = ::RegSetValueExW (
            handle_,
            ApiString{name}.win(),
            0,  // Reserved
            REG_DWORD,
            (LPBYTE)&value,
            (DWORD)sizeof(value)
        ) ;

        if ( this->errorCode != ERROR_SUCCESS )
            return this->raiseError("ReqSetValueEx") ;

        return true ;
    }


} ;

}  // namespace apl

#endif
