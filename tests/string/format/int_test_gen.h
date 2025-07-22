/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
/*
 * Automatically generated file
 */

#pragma once

#include "process_test.h"


// make all types as one string, without spaces
using long_long = long long ;
using unsigned_int = unsigned int ;
using unsigned_long = unsigned long ;
using unsigned_long_long = unsigned long long ;


const FormatElement<int> fmt1[] = {
	{ 0, "%d", -0x69 },
	{ 1, "%d", 0x74 },
	{ 2, "%d", 0x35 },
	{ 3, "%d", -0x1F },
	{ 4, "%d", -0x40 },
	{ 5, "%d", 0x64 },
	{ 6, "%d", 0x4B },
	{ 7, "%d", 0x74 },
	{ 8, "%d", 0x28 },
	{ 9, "%d", -0x4A },
} ;
const FormatElement<long> fmt2[] = {
	{ 11, "%hd", 0x2842 },
	{ 12, "%hd", 0x1ECF },
	{ 13, "%hd", 0x1EB2 },
	{ 14, "%hd", 0x5726 },
	{ 15, "%hd", 0x6BBC },
	{ 16, "%hd", 0x7B7B },
	{ 17, "%hd", -0x742F },
	{ 18, "%hd", 0x375F },
	{ 19, "%hd", 0x229F },
	{ 20, "%hd", -0x1D87 },
} ;
const FormatElement<long_long> fmt3[] = {
	{ 22, "%ld", 0x7B2215ACL },
	{ 23, "%ld", -0x3C6842D9L },
	{ 24, "%ld", -0x27AF7EF2L },
	{ 25, "%ld", -0x372132C9L },
	{ 26, "%ld", 0x6E7C189FL },
	{ 27, "%ld", -0x28093A49L },
	{ 28, "%ld", -0x09102CD4L },
	{ 29, "%ld", -0x7656792AL },
	{ 30, "%ld", -0x07677962L },
	{ 31, "%ld", 0x6CE922E5L },
} ;
const FormatElement<unsigned_int> fmt4[] = {
	{ 33, "%u", 0xBF },
	{ 34, "%u", 0xF3 },
	{ 35, "%u", 0x98 },
	{ 36, "%u", 0x77 },
	{ 37, "%u", 0x97 },
	{ 38, "%u", 0x4C },
	{ 39, "%u", 0x13 },
	{ 40, "%u", 0xA8 },
	{ 41, "%u", 0xEC },
	{ 42, "%u", 0x0A },
} ;
const FormatElement<unsigned_long> fmt5[] = {
	{ 44, "%hu", 0x2000 },
	{ 45, "%hu", 0x78DD },
	{ 46, "%hu", 0x962C },
	{ 47, "%hu", 0x477D },
	{ 48, "%hu", 0x8DDD },
	{ 49, "%hu", 0x3F96 },
	{ 50, "%hu", 0xA2EA },
	{ 51, "%hu", 0xFED2 },
	{ 52, "%hu", 0x42B8 },
	{ 53, "%hu", 0x6F26 },
} ;
const FormatElement<unsigned_long_long> fmt6[] = {
	{ 55, "%lu", 0x9FD96825UL },
	{ 56, "%lu", 0xAE9C5B4CUL },
	{ 57, "%lu", 0xB70E5AA3UL },
	{ 58, "%lu", 0x463158FFUL },
	{ 59, "%lu", 0xAE7E7F0FUL },
	{ 60, "%lu", 0xDA0C3F97UL },
	{ 61, "%lu", 0x23D80EBEUL },
	{ 62, "%lu", 0x511B26CBUL },
	{ 63, "%lu", 0x5C974416UL },
	{ 64, "%lu", 0xC6903880UL },
} ;

inline
int processAll()
{
	if ( int ret = processTest ( fmt1, sizeof(fmt1)/sizeof(fmt1[0]) ) ; ret != 0 )
		return ret ;
	if ( int ret = processTest ( fmt2, sizeof(fmt2)/sizeof(fmt2[0]) ) ; ret != 0 )
		return ret ;
	if ( int ret = processTest ( fmt3, sizeof(fmt3)/sizeof(fmt3[0]) ) ; ret != 0 )
		return ret ;
	if ( int ret = processTest ( fmt4, sizeof(fmt4)/sizeof(fmt4[0]) ) ; ret != 0 )
		return ret ;
	if ( int ret = processTest ( fmt5, sizeof(fmt5)/sizeof(fmt5[0]) ) ; ret != 0 )
		return ret ;
	if ( int ret = processTest ( fmt6, sizeof(fmt6)/sizeof(fmt6[0]) ) ; ret != 0 )
		return ret ;
	return 0 ;
}
