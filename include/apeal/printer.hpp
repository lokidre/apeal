/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

// TODO: make better. Implement on Unix systems

#include "have.hpp"

#include "file.hpp"
#include "policy.hpp"

namespace apl {

class APrinter {

public :
    APrinter()
    {
        #if APEAL_OS_WINDOWS
        printer_handle_ = INVALID_HANDLE_VALUE ;
        #endif

        document_name = "noname" ;
    }

    ~APrinter()
    {
        close() ;
    }

public :
    string_type document_name ;

public :
    bool print_file(const string_type &file_name)
    {
        File<policy_T> file ;
        string_type data ;
        if ( !file.load(file_name,data) )
            return false ;
        return print(data) ;
    }

    template <class data_type>
    bool print(data_type &data)
    {
        #if APEAL_OS_WINDOWS

        string_type printer_name = printer_name_ ;

        if ( printer_name.empty() ) {
            char printer_name_buffer[256] ;
            DWORD buffer_size=255 ;

            if ( !::GetDefaultPrinterA(printer_name_buffer,&buffer_size) ) {
                return error_() ;
            }

            printer_name = printer_name_buffer ;
        }

        close() ;

        if ( !::OpenPrinter((LPTSTR)printer_name.c_str(),&printer_handle_,0) )
            return error_() ;


        DOC_INFO_1 di ;
        DWORD job ;
        TCHAR dname[256] ;

        #ifdef __HAVE_STRNCPY_S
        strncpy_s(dname,250,document_name.c_str(),250) ;
        #else
        strncpy(dname,document_name.c_str(),250) ;
        #endif

        di.pDocName = dname ;
        di.pOutputFile = 0 ;
        di.pDatatype = TEXT("RAW") ;

        if ( (job=::StartDocPrinter(printer_handle_,1,(LPBYTE)&di)) == 0 ) {
            close() ;
            return error_() ;
        }

        if ( !::StartPagePrinter(printer_handle_) ) {
            ::EndDocPrinter(printer_handle_) ;
            close() ;
            return error_() ;
        }

        DWORD BytesWritten ;

        if ( !::WritePrinter(printer_handle_,(LPVOID)data.data(),DWORD(data.size()),&BytesWritten) ) {
            ::EndPagePrinter(printer_handle_) ;
            ::EndDocPrinter(printer_handle_) ;
            close() ;
            return error_() ;
        }

        if ( !::EndPagePrinter(printer_handle_) ) {
            ::EndDocPrinter(printer_handle_) ;
            close() ;
            return error_() ;
        }

        if( !::EndDocPrinter(printer_handle_) ) {
            close() ;
            return error_() ;
        }

        close() ;

        // Check to see if correct number of bytes were written.
        if( BytesWritten != DWORD(data.size()) )
            return error_() ;

        #endif


        return true ;
    }

    void close()
    {
        #if APEAL_OS_WINDOWS
        if ( printer_handle_ != INVALID_HANDLE_VALUE ) {
            ::ClosePrinter(printer_handle_), printer_handle_ = INVALID_HANDLE_VALUE ;
        }
        #endif
    }


private :
    bool opened_ ;
    string_type printer_name_ ;
    AError error_ ;

    #if APEAL_OS_WINDOWS
    HANDLE printer_handle_ ;
    #endif
} ;

}
