/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

// TODO: obsolete, remove


#include "def.hpp"
#include "typemap.hpp"
#include "error.hpp"


#include <stdexcept>
#include <string>
#include <vector>


#if defined(__CYGWIN) || defined(ANDROID)

namespace std {

typedef basic_string<wchar_t> wstring ;

}

#endif


namespace apl {

inline
std::string &to_string ( const wchar_t *ws_data, const size_t ws_size, std::string &s )
{
    #if APEAL_OS_WINDOWS

    size_t count = WideCharToMultiByte ( CP_UTF8, 0, ws_data, (int)ws_size, 0,0,0,0) ;
    if ( count == 0 )
        s.clear() ;
    else {
        s.resize(count) ;
        WideCharToMultiByte ( CP_UTF8, 0, ws_data, (int)ws_size, const_cast<char*>(s.data()), (int)count, 0, 0 ) ;
    }

    #else

    std::wstring w ( ws_data, ws_size ) ;

    s.assign ( w.begin(), w.end() ) ;

    #endif

    return s ;
}

inline
std::string &to_string ( const std::wstring &ws, std::string &s ) {  return to_string(ws.c_str(),ws.length(),s) ;  }



inline
std::wstring &to_wstring ( const std::string &s, std::wstring &ws )
{
    #if APEAL_OS_WINDOWS
        int count = MultiByteToWideChar ( CP_UTF8, 0, s.c_str(), (int)s.size(), 0, 0 ) ;

    //--count ;
    if ( count == 0 )
        ws.clear() ;
        // return (wchar_t *)("") ;
    else {

        ws.resize(count) ;
        MultiByteToWideChar ( CP_UTF8, 0, s.c_str(), (int)s.size(), (wchar_t *)ws.data(), count ) ;
    }
        #else

    ws.assign ( s.begin(), s.end() ) ;

    #endif

    return ws ;
}

inline
std::wstring &from_wstring ( const std::wstring &in, std::wstring &out )
{
    out = in ;
    return out ;
}

inline
std::string &from_wstring ( const std::wstring &in, std::string &out )
{
    return to_string(in,out) ;
}


}




namespace std {

// extention of runtime_error for using wide strings
class wruntime_error : public runtime_error {
public:
    wruntime_error( const wstring& ws) : runtime_error(apl::to_string(ws,s_)) {  msg_ = ws ;  }
    wruntime_error( const wruntime_error &rhs ) : runtime_error(apl::to_string(rhs.msg_,s_)) {  msg_ = rhs.msg_ ;  }
    wruntime_error &operator= ( const wruntime_error& rhs ) { runtime_error::operator=(rhs) ; msg_ = rhs.msg_ ; return *this ;  }

    virtual ~wruntime_error() throw() {}

    wstring msg_ ;
    string s_ ;

    //virtual const char *what( ) const throw()
    //{
    //  return msg_.c_str() ;
    //}

} ;

}


namespace apl {



class KErrorPolicy {} ;

class KMutexPolicy {
public :
    void lock(){}
    void unlock(){}
} ;

class KStringPolicy{} ;
class KArrayPolicy{} ;
class KLargeSizePolicy{} ;





struct AThrowErrorPolicy {
    bool operator()() const { throw std::runtime_error("") ; }
    bool operator()(const std::string &s) const { throw std::runtime_error(s) ; }
    bool operator()(const std::wstring &s) const { throw std::wruntime_error(s.c_str()) ; }
} ;

struct AReturnErrorPolicy {
    bool operator()() const {  return false ;  }
    bool operator()(const std::string &) const {  return false ;  }
    bool operator()(const std::wstring &) const {  return false ;  }
} ;




class ALockMutexPolicy {
public :
  void lock(){}
  void unlock(){}
} ;

class ANoLockPolicy {
public :
  void lock(){}
  void unlock(){}
} ;




class ASize64Policy {
public :
  typedef unsigned long long size_type ;
} ;

class AAutoSizePolicy {
public :
  typedef size_t size_type ;
} ;






class AStdStringPolicy {
public :
    typedef std::string string_type ;
} ;


class AWStdStringPolicy {
public :
    typedef std::wstring string_type ;
} ;




template <class T>
class AArrayPolicy {} ;

//template <class T>
//class AStdVectorPolicy : AArrayPolicy {
//  typedef std::vector<T> ArrayPolicy ;
//} ;




typedef
typemap::insert<KErrorPolicy,AThrowErrorPolicy,
typemap::insert<KMutexPolicy,ANoLockPolicy,
typemap::insert<KStringPolicy,AStdStringPolicy,
typemap::insert<KLargeSizePolicy,ASize64Policy
>::X
>::X
>::X
>::X ADefaultPolicy ;


typedef
typemap::insert<KErrorPolicy,AReturnErrorPolicy,
typemap::insert<KMutexPolicy,ANoLockPolicy,
typemap::insert<KStringPolicy,AStdStringPolicy,
typemap::insert<KLargeSizePolicy,ASize64Policy
>::X
>::X
>::X
>::X ANoThrowDefaultPolicy ;






//template <class T1=ANull, class T2=ANull, class T3=ANull>
//struct APolicyContainer {
//  typedef typename AErrorPolicySelector<T1,T2,T3>::ErrorPolicy ErrorPolicy ;
//  typedef std::string StringPolicy ;
//  typedef size_t SizePolicy ;
//} ;


//typedef APolicyContainer<AExceptionPolicy> ADefaultPolicyContainer ;


}
