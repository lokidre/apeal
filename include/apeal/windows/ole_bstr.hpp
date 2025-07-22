/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <string>

#include "../have.hpp"
#include "../noncopyable.hpp"

namespace apl {


class OleBSTR: public NonCopyable {

public :
	BSTR bstr_ ;

protected :
	bool allocated_by_me_ ;

public :
	OleBSTR()
	{
		bstr_ = 0 ;
		allocated_by_me_ = false ;
	}

	OleBSTR ( const std::string &str )
	{
		bstr_ = 0 ;
		allocated_by_me_ = false ;
		assign(str) ;
	}

	~OleBSTR()
	{
		free() ;
	}

	void free()
	{
		if ( bstr_ )
			::SysFreeString(bstr_), bstr_ = 0 ;
	}

	/*
	void resize ( const size_t newsize )
	{
		if ( newsize == 0 )
			return ;

		wchar_t *wstr = new wchar_t[newsize+1] ;

		if ( wstr == 0 )
			throw 0 ;

		*wstr = '\0' ;

		if ( bstr ) {
			wcscpy(wstr,bstr_) ;
			if ( allocated_by_me_ )
				::SysFreeString(bstr), bstr = 0 ;
		}

		bstr_ = ::SysAllocString(wstr) ;
		allocated_by_me_ = true ;
		delete wstr ;
	}
	*/


	void assign ( const char *str )
	{
		size_t slen = strlen(str) ;

		if ( slen == 0 ) {
			clear() ;
			return ;
		}
			
		wchar_t *wstr = new wchar_t[slen+1] ;
		if ( wstr == 0 )
			throw 0 ;

#if __APEAL_HAVE_MBSTOWCS_S
		size_t charactersConverted ;
		mbstowcs_s ( &charactersConverted, wstr, slen+1, str, slen+1 ) ;
#else
		mbstowcs ( wstr, str, slen+1 ) ;
#endif


		assign(wstr) ;
		
		delete wstr ;
	}

	void assign ( const std::string &str ) { assign(str.c_str()) ;  }

	void assign ( const wchar_t *str )
	{
		if ( bstr_ && allocated_by_me_ )
			::SysFreeString(bstr_), bstr_ = 0 ;

		bstr_ = ::SysAllocString(str) ;
		allocated_by_me_ = true ;
	}

	void assign ( const wchar_t *str, const size_t l )
	{
		if ( bstr_ && allocated_by_me_ )
			::SysFreeString(bstr_), bstr_ = 0 ;

		bstr_ = ::SysAllocStringLen(str,UINT(l)) ;
		allocated_by_me_ = true ;
	}

	const char *get ( std::string &str ) const 
	{
		str.clear() ;
		if ( bstr_ ) {
			size_t ilen = wcslen(bstr_) ;

			char *buffer = new char[ilen+1] ;
			if ( !buffer )
				throw this ;

#if __APEAL_HAVE_WCSTOMBS_S
			size_t charactersConverted ;
			wcstombs_s ( &charactersConverted, buffer, ilen+1, bstr_, ilen+1 ) ;
#else
			wcstombs ( buffer, bstr_, ilen+1 ) ;
#endif
			str = buffer ;
			delete buffer ;
		}
		return str.c_str() ;
	}


	OleBSTR &operator = ( const std::string &str )
	{
		assign(str) ;
		return *this ;
	}

	OleBSTR &operator = ( const char *str )
	{
		assign(str) ;
		return *this ;
	}


public :  // mimic STL
	//typedef wchar_t *iterator ;
	typedef const wchar_t *const_iterator ;

	const size_t size() const {  return bstr_ ? wcslen(bstr_) : 0 ;  }
	const size_t length() const {  return bstr_ ? wcslen(bstr_) : 0 ;  }
	const bool empty() const {  return size() == 0 ;  }
	void clear() 
	{  
		if ( bstr_ )
			::SysFreeString(bstr_), bstr_ = 0 ;
	}

	//iterator begin() const {  return bstr_ ;  }
	//iterator end() const {  return bstr_ ? bstr_ + wcslen(bstr_) : 0 ;  }
	                                                       
	const_iterator begin() const {  return bstr_ ;  }
	const_iterator end() const {  return bstr_ ? bstr_ + wcslen(bstr_) : 0 ;  }

	//void operator += ( const char c )
	//{
	//	resize(length()+1) ;
	//	wcscat(bstr,c) ;
	//}

	void operator += ( const char *c )
	{
		OleBSTR s ;
		s.assign(c) ;

		operator += (s) ;
	}


	template <class string_type>
	void operator += ( const string_type &s )
	{
		wchar_t *wstr = new wchar_t[length()+s.length()+1] ;

		wstr[0] = '\0' ;

		if ( bstr_ ) {
#if __APEAL_HAVE_WCSCPY_S
			wcscpy_s(wstr,length(),bstr_) ;
#else
			wcscpy(wstr,bstr_) ;
#endif
		}

		if ( s.bstr_ ) {
#if __APEAL_HAVE_WCSCAT_S
			wcscat_s(wstr,s.length(),s.bstr_) ;
#else
			wcscat(wstr,s.bstr_) ;
#endif
		}

		assign(wstr) ;

		delete wstr ;
	}


	//const BSTR c_str() const {  return bstr_ ;  }
	BSTR *bstr() {  return &bstr_ ;  } 
} ;



class OleBSTRByRef: public OleBSTR {
public :
	AOleBstrByRef(): OleBSTR()
	{
		allocated_by_me_ = true;
	}
};



}  // namespace apl
