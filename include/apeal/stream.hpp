/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once


#if APEAL_OS_WINDOWS
#ifndef __API_WINCE
#include <io.h>
#endif
#endif

#ifndef __API_WINCE
#include <errno.h>
#endif

#if APEAL_OS_POSIX
#include <unistd.h>
#endif

namespace apl {


template <class policy_T=ADefaultPolicy>
class AStream {
private :
	typedef typename typemap::find<KStringPolicy,policy_T>::X::Value StringPolicy ; // ::string_type string_type ;
	typedef typename typemap::find<KErrorPolicy,policy_T>::X::Value ErrorPolicy ; // error_policy_ ;

	typedef typename StringPolicy::string_type string_type ;

protected :
	#if APEAL_OS_WINDOWS
	typedef HANDLE handle_type ;
	#else
	typedef int handle_type ;
#endif


protected :
	bool opt_partial_ ;

public :
	bool set_partial(const bool p ) {  bool old = opt_partial_ ;  opt_partial_ = p ;  return old ;  }
	bool get_partial() const {  return opt_partial_ ;  }


public :
	AStream() : handle_(handle_type(-1)) 
	{
		buffer_strategy_ = 256 ;
		opt_partial_ = false ;
	}
	virtual ~AStream(){}


public :

	virtual int write ( const char data ) {  return write_basic ( &data, 1 ) ;  }
	virtual int write ( const string_type &s ) {  return write_basic(s.data(),int(s.size())) ;  }
	virtual int write ( const void *p, const int cnt ) {  return write_basic(p,cnt) ;  }

    virtual int read ( char &data ) {  return read_basic ( &data, 1 ) ;  }
    virtual int read ( string_type &data, const int len )
	{
		buffer_.resize(buffer_strategy_) ;
		data.clear() ;
		for ( int remain=len, count=0 ; remain>0 ; remain-=count ) {
			count = recv_basic(buffer_.get(),std::min((int)buffer_.size(),remain)) ;
			if ( count <= 0 )
				return count ;
			// here we have to process character-by-character
			for ( int i = 0 ; i < count ; ++i )
				data += buffer_.get()[count] ; //.append(buffer_,count) ;
		}
		return len ;
	}

    virtual int read ( void *data, const int len )
    {
		return read_basic(data,len) ;
    }

    virtual int recv ( void *data, const int len ) {  return recv_basic(data,len) ;  }

    virtual int ready() {  return 0 ;  }

    template <typename container_T>
    int read_packet ( container_T &data, const char *eop ) {  return read_packet_basic(data,eop,eop+strlen(eop)) ;  }

    template <typename container_T>
    int read_packet ( container_T &data, char *eop ) {  return read_packet_basic(data,eop,eop+strlen(eop)) ;  }



    int send ( const void *data, const int len ) {  return send_basic(data,len) ;  }
    int send ( const string_type &data ) {  return send_basic(data.data(),int(data.size())) ;  }

protected :
	handle_type handle_ ;

	Buffer<char> buffer_ ;
	int buffer_strategy_ ;

protected :

	virtual int send_basic ( const void *data, const int &len )
	{
		int count = 0 ;

		#if APEAL_OS_WINDOWS
		DWORD written = 0 ;
		if ( !::WriteFile ( handle_, data, len, &written, 0 ) ) {
			handle_error_(::GetLastError()) ;
			return -1 ;
		}
		count = int(written) ;
		#else
		count = ::write(handle_,data,len) ;
		if ( count < 0 ) {
			handle_error_(errno) ;
			return -1 ;
		}
		#endif
		return count ;
	}


    virtual int post_basic ( const void *data, const int &len )
    {
      return send_basic(data,len) ;
    }


    virtual int write_basic ( const void *vdata, const int len )
    {
      const unsigned char *data = static_cast<const unsigned char *>(vdata) ;

      for ( int remain=len, count=0 ; remain > 0 ; data += count, remain -= count ) {
        count = send_basic(data,remain) ;
        if ( count <= 0 )
          return count ;
      }

      return len ;
    }




    virtual int scan_basic ( void *vdata, const int len )
    {
      return 0 ;
    }


    int read_basic ( void *vdata, const int len )
    {
      int count = 0 ;
      #if APEAL_OS_WINDOWS
      DWORD Processed = 0 ;
      if ( !::ReadFile(handle_,vdata,len,&Processed,0) ) {
        handle_error_(::GetLastError()) ;
        return -1 ;
      }
      count = int(Processed) ;

      #else
      count = ::read(handle_,vdata,len) ;
      if ( count < 0 ) {
        handle_error_(errno) ;
        return -1 ;
      }
      #endif
      return count ;
    }



    virtual int recv_basic ( void *vdata, const int len )
    {
      unsigned char *data = static_cast<unsigned char *>(vdata) ;
      for ( int remain = len, count = 0 ; remain > 0 ; data += count, remain -= count ) {
        count = recv_basic(data,remain) ;
        if ( count <= 0 )
          return count ;
      }

      return len ;
    }



    template <typename container_T, typename eop_iterator_T>
    int read_packet_basic ( container_T &data, const eop_iterator_T &eop_begin, const eop_iterator_T &eop_end )
    {
      data.clear() ;

      int total_count = 0 ;

      size_t eop_size = eop_end - eop_begin ;

      // needed optimize with peek and buffer
      unsigned char buffer ;
      for(;;) {
        int count = read_basic(&buffer,1) ;
        if ( count != 1 )
          return count ;
        data.append(1,buffer) ;
        if ( data.size() >= eop_size ) {
          if ( packet_check_(data.begin()+total_count,data.end(),eop_begin,eop_end) )
            break ;
        }
        total_count++ ;
      }
      return total_count ;
    }


    template <typename data_iterator_T, typename eop_iterator_T>
    bool packet_check_ ( const data_iterator_T &data_begin, const data_iterator_T &data_end, const eop_iterator_T &eop_begin, const eop_iterator_T &eop_end )
    {
      data_iterator_T data_it = data_begin ;
      for ( eop_iterator_T eop_it = eop_begin ; eop_it != eop_end ; ++eop_it, ++data_it ) {
        if ( data_it == data_end )
          return false ;
        if ( *eop_it != *data_it )
          return false ;
      }
      return true ;
    }


	virtual bool handle_error_ ( const int error_code )
    {
      ErrorPolicy error_policy ;
      string_type error_string ;

      #if APEAL_OS_WINDOWS
      get_error_string_windows(error_code, error_string) ;
      #endif
      return error_policy(error_string) ;
    }

} ;


}
