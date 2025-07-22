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

#include "../string.hpp"

#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>

namespace apl {

#if __OBJC__
	
inline
NSString *toNSString(const char *s) {
    NSString *ns = s ? [NSString stringWithUTF8String:s] : nil;
    return ns ? ns : @"";
}
	
inline
NSString *toNSString(const std::string &s) {
    auto ns = [NSString stringWithUTF8String:s.c_str()];
    return ns ? ns : @"";
}
	
inline NSString *toNSString(CFStringRef s) {
    return (__bridge NSString *)s;
}
	
inline
NSURL *toNSURL(const std::string& urlString) {
    return [NSURL URLWithString:[NSString stringWithUTF8String:urlString.c_str()]];
}
	
	
inline
NSNumber *toNSNumber(int x) {  return [NSNumber numberWithInt:x];  }
	
inline
std::string toString(CFStringRef x) {
    std::string s;
    if (x)
        s = ((__bridge NSString *)x).UTF8String;
    return s;
}

inline
std::string toString ( NSString *x ) {
	std::string s ;
	if ( x )
		s = x.UTF8String ;
	return s ;
}

inline
std::string toStringFromData ( NSData *d ) {
	if ( d && d.length ) {
		//return toString([[NSString alloc] initWithData:d encoding:NSUTF8StringEncoding]) ;
		//auto s = std::string ( (const char *)d.bytes, d.length ) ;
		
		return std::string ( (const char *)d.bytes, d.length ) ;
	}
	return "" ;
}

inline
std::string toHexFromData ( NSData *d ) {
	std::string s ;
	
	if ( d && d.length ) {
		auto l = d.length ;
		auto p = (unsigned char *)d.bytes ;
		
		for (int i = 0; i < l ; ++i ) {
			apl::str_format_append ( s, "%02x", ((unsigned int)p[i]) ) ;
		}
	}
	return s ;
}
	
#endif

}

