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
#include "policy.hpp"


#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>


namespace apl {

template <class policy_T=ADefaultPolicy>
class AOpenSSL {
public :
	using policy_type = policy_T ;
	using ErrorPolicy = typename typemap::find<KErrorPolicy,policy_T>::X::Value ;
	using string_type = typename typemap::find<KStringPolicy,policy_T>::X::Value::string_type ;



private :
	Buffer<char,policy_T> buffer_ ;
	int buffer_size_policy_ ;

	int write_buffer_size_ = 4096, read_buffer_size_ = 4096 ;

	int timeout_recv_, timeout_recv_msec_ ;
	bool partial_ = false ;  // read and write might return partial packets
	bool timeout_disabled_ = false ;
	string_type host_ ;


private :
	SSL_CTX *ctx_{} ;
	BIO *bio_{} ;
	SSL *ssl_{} ;


public :
	void timeout_disable() {  timeout_disabled_ = true ;  }
	void timeout_enable() {  timeout_disabled_ = false ;  }

	void set_timeout_recv_sec ( const int timeout_sec ) {  timeout_recv_ = timeout_sec, timeout_recv_msec_ = 0, timeout_enable() ;  }
	void set_timeout_recv_min ( const int timeout_min ) {  timeout_recv_ = timeout_min*60, timeout_recv_msec_ = 0, timeout_enable() ;  }

	void set_partial ( const bool b ) {  partial_ = b ;  }


	bool connect ( const string_type &host, const int &port )
	{

		auto conn_str = host ;
		if ( port )
			conn_str += ':', conn_str += std::to_string(port) ;

		host_ = conn_str ;

		if ( !bio_ ) {
			bio_ = BIO_new_connect((char *)conn_str.c_str()) ;
			if ( !bio_ )
				return handle_error_() ;
		} else
			BIO_set_conn_hostname ( bio_, (char *)conn_str.c_str() ) ;

		if ( BIO_do_connect(bio_) <= 0 )
			return handle_error_() ;

		if ( !ctx_ ) {
			ctx_ = SSL_CTX_new(SSLv23_method()) ;
			SSL_CTX_set_mode ( ctx_, SSL_MODE_AUTO_RETRY ) ;
		}

		if ( !ssl_ ) {
			ssl_ = SSL_new(ctx_) ;

			if ( !ssl_ )
				return handle_error_() ;
		}

		SSL_set_bio ( ssl_, bio_, bio_ ) ;

		int sslCode = SSL_connect(ssl_) ;
		if ( sslCode <= 0 )
			return handle_error_(sslCode) ;

		return true ;
	}

	void close()
	{
		buffer_.release() ;

		if ( ssl_ ) {
			if ( SSL_shutdown(ssl_) == 0 )
				SSL_shutdown(ssl_) ;
			SSL_clear(ssl_) ;
			SSL_free(ssl_) ;
			ssl_ = 0 ;
			bio_ = 0 ;
		}

		if ( bio_ ) {
			BIO_free(bio_) ;
			bio_ = 0 ;
		}

		//if ( ctx_ ) {
		//	SSL_CTX_free(ctx_) ;
		//	ctx_ = 0 ;
		//}
	}

	int send ( const void *data, const int size )
	{
		int total = 0 ;
		const char *d = (const char *)data ;
		int count = 0 ;

		while ( total < size ) {  // cycle while we have something to send

			count = SSL_write ( ssl_, d, size - total ) ;

			if ( count <= 0 ) {
				handle_error_() ;
				return count ;
			}

			total += count ;
			d += count ;

			if ( partial_ )
				break ;
		}

		return total ;
	}

	int send ( const string_type &s ) {  return this->send(s.data(),(int)s.size()) ;  }


	int recv ( void *data, const int size )
	{
		int total = 0 ;
		char *d = (char *)data ;
		int count = 0 ;

		while ( total < size ) {  // cycle while we have something to send

			count = SSL_read ( ssl_, d, size - total ) ;

			if ( count <= 0 ) {
				handle_error_() ;
				return count ;
			}

			total += count ;
			d += count ;
		}

		return total ;
	}

	int recv_partial ( void *data, const int size )
	{
		int count = SSL_read ( ssl_, data, size ) ;

		if ( count <= 0 )
			handle_error_() ;

		return count ;
	}

	template <class C, class D>
	int recv_packet ( C &packet, const D &end_of_packet )
	{
		int read_size ;

		packet.clear() ;
		if ( !buffer_.resize(read_buffer_size_) )
			return -1 ;
	
		// timeout support
		bool packet_received = false ;

		for(;;) {
			int ready_count = SSL_pending(ssl_) ;

			// read the data and analyze it
			read_size = std::min ( (int)buffer_.size(), (int)ready_count ) ;

			// peek the data
			read_size = ::SSL_peek ( ssl_, buffer_.get(), read_size ) ;

			// walk through the data and check for the end of packet 
			// copy the end-of-packet too
			int accepted_count = 0 ;
			while ( accepted_count < read_size ) {
				packet.append(&buffer_[accepted_count],1) ;
				accepted_count++ ;
		
				if ( check_end_of_packet(packet,end_of_packet) ) {
					packet_received = true ;
					break ;
				}
			}

			// read only the correct amount of data
			if ( accepted_count ) {
				read_size = accepted_count ;
	
				char *ptr = (char *)buffer_.get() ;
				int bytesleft = read_size ;
	
				do {
					int rc = SSL_read ( ssl_, ptr, bytesleft ) ;
	
					if ( rc <= 0 ) {  // the data has to be received
						handle_error_() ;
						return -1 ;
					}
	
					bytesleft -= rc;
					ptr += rc;
				} while ( bytesleft > 0 ) ;
			}
	
			if ( packet_received )
				break ;
		}

		return (int)packet.size() ;
	}

	int ready()
	{
		return SSL_pending(ssl_) ;
	}





public :
	AOpenSSL()
	{
		bool static openSSLInitialized = false ;

		if ( openSSLInitialized )
			return ;
		
		if ( !SSL_library_init() )
			throw std::runtime_error("OpenSSL init error") ;

		SSL_load_error_strings() ;

		openSSLInitialized = true ;
	}

	~AOpenSSL() {
		close() ;
		if ( ssl_ )
			SSL_free(ssl_) ;
		if ( bio_ )
			BIO_free(bio_) ;
		if ( ctx_ )
			SSL_CTX_free(ctx_) ;
	}



private :
	template <class P, class E, size_t N>
	bool check_end_of_packet ( const P &packet, const E (&eop)[N] )
	{
		if ( packet.length() < N )
			return false ;

		int nn = N ;
		if ( eop[nn-1] == 0 )
			nn-- ;

		if ( memcmp ( packet.data() + packet.length() - nn, eop, nn ) == 0 )
			return true ;
		//if ( packet.substr(packet.length()-N) == eop )
		//	return true ;

		return false ;
	}



protected :
	bool handle_error_(const string_type &error_string)
	{
		ErrorPolicy error_policy ;

		if ( host_.empty() )
			return error_policy(error_string) ;

		string_type es = "ssl:" ;
		es += host_ ;
		es += ":" ;
		es += error_string ;

		return error_policy(es) ;
	}

	//bool handle_error_(const int error_code)
	//{
	//	string_type error_string ;
	//	get_error_string_socket(error_code,error_string) ;
	//	return handle_error_(error_string) ;
	//}

	bool handle_error_(int returnCode = 0)
	{
		string_type errorString ;
		int errorBufferSize = 255 ;
		char errorBuffer[256] ;

		if ( ssl_ ) {
			for ( int errorCode = SSL_get_error(ssl_,returnCode) ; errorCode ; errorCode = SSL_get_error(ssl_,returnCode) ) {
				errorString += ERR_error_string ( errorCode, errorBuffer ) ;
			}
		}

		for ( int errorCode = ERR_get_error() ; errorCode ; errorCode = ERR_get_error() ) {
			errorString += ERR_error_string ( errorCode, errorBuffer ) ;
		}

		return handle_error_(errorString) ;
	}


} ;

}

