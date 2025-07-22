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
#include "types.hpp"

// First try to detect the endianness

#if __has_include(<sys/endian.h>)
#include <sys/endian.h>
#endif

// For some reason clang has intrin.h but produces compile error for iOS target
//#if __has_include(<intrin.h>)
#if APEAL_OS_WINDOWS
#include <intrin.h>
#endif


#ifdef __BYTE_ORDER__
#define APEAL_BYTE_ORDER    __BYTE_ORDER__
#endif

#ifdef __ORDER_LITTLE_ENDIAN__
#define APEAL_LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#endif

#ifdef __ORDER_BIG_ENDIAN__
#define APEAL_BIG_ENDIAN    __ORDER_BIG_ENDIAN__
#endif


#ifndef APEAL_BYTE_ORDER
#if defined(__i386__) || defined(_M_IX86) || defined(_M_IX64) || defined(_M_IA64) || defined(_M_I86) || defined(_M_X64) || defined(_MSC_VER)
#define APEAL_LITTLE_ENDIAN  1
#define APEAL_BYTE_ORDER     APEAL_LITTLE_ENDIAN
#endif
#endif



namespace apl {

// Runtime machine endianness detection
inline bool is_big_endian() noexcept
{
    volatile long a = 1 ;
    volatile char b = *(char *)&a;
    return b == 1;
}

//#if __has_include(<intrin.h>)
#if APEAL_OS_WINDOWS
inline uint16_t byte_swap_16(uint16_t x) noexcept {  return _byteswap_ushort(x);  }
inline uint32_t byte_swap_32(uint32_t x) noexcept {  return _byteswap_ulong(x);  }
inline uint64_t byte_swap_64(uint64_t x) noexcept {  return _byteswap_uint64(x);  }
#elif __has_builtin(__builtin_bswap16)
inline uint16_t byte_swap_16(uint16_t x) noexcept {  return __builtin_bswap16(x);  }
inline uint32_t byte_swap_32(uint32_t x) noexcept {  return __builtin_bswap32(x);  }
inline uint64_t byte_swap_64(uint64_t x) noexcept {  return __builtin_bswap64(x);  }
#else
constexpr uint16_t byte_swap_16(uint16_t x) noexcept { return (x<<8) | (x>>8) ; }
constexpr uint32_t byte_swap_32(uint32_t x) noexcept {
    x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0xFF00FF);
    x = (x << 16) | (x >> 16);
    return x;
}

constexpr uint64_t byte_swap_64(uint64_t x) noexcept {
    x = ((x & 0x00000000FFFFFFFFull) << 32) | ((x & 0xFFFFFFFF00000000ull) >> 32);
    x = ((x & 0x0000FFFF0000FFFFull) << 16) | ((x & 0xFFFF0000FFFF0000ull) >> 16);
    x = ((x & 0x00FF00FF00FF00FFull) << 8)  | ((x & 0xFF00FF00FF00FF00ull) >> 8);
    return x;
}

#endif


//
// define btoh, htob, ntoh, hton functions
//
// htob - Host to Bluetooth (little endian)
// hton - Host to Network (big endian)
// only if they are not defined
// we use C++, so if they will be defined later as macros our definition will not be used
//
//#ifndef htobs
//#if __BYTE_ORDER == __LITTLE_ENDIAN
//inline uint16_t htobs(uint16_t b) { return b ; }
//inline uint32_t htobl(uint32_t b) { return b ; }
//inline uint16_t btohs(uint16_t b) { return b ; }
//inline uint32_t btohl(uint32_t b) { return b ; }
//#elif __BYTE_ORDER == __BIG_ENDIAN
//inline uint16_t htobs(uint16_t b) { return byte_swap_16(b) ;  }
//inline uint32_t htobl(uint32_t b) { return byte_swap_32(b) ;  }
//inline uint16_t stobh(uint16_t b) { return byte_swap_16(b) ;  }
//inline uint32_t ltobh(uint32_t b) { return byte_swap_32(b) ;  }
//#else
//#error "Unknown byte order"
//#endif
//#endif
//
//#ifndef ntobs
//#if __BYTE_ORDER == __LITTLE_ENDIAN
//inline uint16_t ntobs(uint16_t b) { return byte_swap_16(b) ;  }
//inline uint32_t ntobl(uint32_t b) { return byte_swap_32(b) ;  }
//inline uint16_t stobn(uint16_t b) { return byte_swap_16(b) ;  }
//inline uint32_t ltobn(uint32_t b) { return byte_swap_32(b) ;  }
//#elif __BYTE_ORDER == __BIG_ENDIAN
//inline uint16_t ntobs(uint16_t b) { return b ; }
//inline uint32_t ntobl(uint32_t b) { return b ; }
//inline uint16_t btons(uint16_t b) { return b ; }
//inline uint32_t btonl(uint32_t b) { return b ; }
//#else
//#error "Unknown byte order"
//#endif
//#endif


//inline uint16_t lendian(uint16_t x)
//{
//	return lendian16(x);
//}
//
//inline uint32_t lendian(uint32_t x)
//{
//    return lendian32(x);
//}



#if APEAL_BYTE_ORDER == APEAL_LITTLE_ENDIAN

constexpr uint16_t from_le_16(uint16_t x) noexcept { return x; }
constexpr uint32_t from_le_32(uint32_t x) noexcept { return x; }
constexpr uint64_t from_le_64(uint64_t x) noexcept { return x; }

template <class T>
constexpr uint16_t to_le_16(T x) noexcept { return static_cast<uint16_t>(x); }

template <class T>
constexpr uint32_t to_le_32(T x) noexcept { return static_cast<uint32_t>(x); }

template <class T>
constexpr uint64_t to_le_64(T x) noexcept { return static_cast<uint64_t>(x); }

inline uint16_t from_be_16(uint16_t x) noexcept { return byte_swap_16(x); }
inline uint32_t from_be_32(uint32_t x) noexcept { return byte_swap_32(x); }
inline uint64_t from_be_64(uint64_t x) noexcept { return byte_swap_64(x); }
inline uint16_t to_be_16(uint16_t x) noexcept { return byte_swap_16(x); }
inline uint32_t to_be_32(uint32_t x) noexcept { return byte_swap_32(x); }
inline uint64_t to_be_64(uint64_t x) noexcept { return byte_swap_64(x); }


#elif APEAL_BYTE_ORDER == APEAL_BIG_ENDIAN

constexpr uint16_t from_be_16(uint16_t x) noexcept { return x; }
constexpr uint32_t from_be_32(uint32_t x) noexcept { return x; }
constexpr uint64_t from_be_64(uint64_t x) noexcept { return x; }
constexpr uint16_t to_be_16(uint16_t x) noexcept { return x; }
constexpr uint32_t to_be_32(uint32_t x) noexcept { return x; }
constexpr uint64_t to_be_64(uint64_t x) noexcept { return x; }

inline uint16_t from_le_16(uint16_t x) noexcept { return byte_swap_16(x); }
inline uint32_t from_le_32(uint32_t x) noexcept { return byte_swap_32(x); }
inline uint64_t from_le_64(uint64_t x) noexcept { return byte_swap_64(x); }
inline uint16_t to_le_16(uint16_t x) noexcept { return byte_swap_16(x); }
inline uint32_t to_le_32(uint32_t x) noexcept { return byte_swap_32(x); }
inline uint64_t to_le_64(uint64_t x) noexcept { return byte_swap_64(x); }

#else

#error "Unknown or undetected CPU Arch endianness!"

#endif


}  // namespace apl

