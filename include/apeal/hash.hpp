/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#if 0

Parser hasher. 
---

Hashes the strings usually occuring while parsing sources
The assumption that the text contains only ASCII characters: 0x21 to 0x7F
This leaves only 5D (93) characters.

    log(18446744073709551615,93) ~ 19.2659197225

This gives 19 byte strings. Maybe we should think what to do if 
the string is > 19


#endif


#include <cstdint>
#include <cstring>
#include <functional>
#include <span>


namespace apl {




// TODO: do string_view

using Hash64 = std::uint64_t;


template <typename T>
constexpr void hashCombine(Hash64& seed, const T& val) noexcept {
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


constexpr Hash64 hashAsciiHorner(std::string_view str) noexcept {
    Hash64 h{1125899906842597ULL};

    for(auto c: str)
        h = 0x5D * h + (c-0x21);

    return h;
}

constexpr Hash64 hashHorner(std::string_view str) noexcept {
    Hash64 h{1125899906842597ULL};

    for(auto c: str)
        h = 31 * h + c;

    return h;
}

// Hashing in 64 bit chunks for speed
constexpr Hash64 hashHorner64Begin() noexcept { return 1125899906842597ULL; }
constexpr Hash64 hashHorner64Val64(Hash64 h, std::uint64_t data) noexcept { 
    return h * 13 + data; 
}
constexpr Hash64 hashHorner64Block64(Hash64 h, std::span<const std::uint64_t> data) noexcept {
    for(auto c: data)
        h = 31 * h + c;

    return h;
}

constexpr Hash64 hashFnv64Begin() noexcept { return 14695981039346656037ULL;  }

//template <class It>
//constexpr Hash64 hashFnv64Block64It(Hash64 h, It begin, It end) noexcept {
//    std::uint64_t fnvPrime{1099511628211ULL};
//    
//    for(auto c = begin; c != end; ++c) {
//        h ^= *c;
//        h *= fnvPrime;
//    }
//    
//    return h;
//}

constexpr Hash64 hashFnv64Block64(Hash64 h, std::span<const std::uint64_t> data) noexcept {
//    return hashFnv64Block64It(h, data.begin(), data.end());
    std::uint64_t fnvPrime{1099511628211ULL};

    for(auto c: data) {
        h ^= c;
        h *= fnvPrime;
    }

    return h;
}

constexpr Hash64 hashFnv64Val64(Hash64 h, std::uint64_t data) noexcept
{
    std::uint64_t fnvPrime{1099511628211ULL};

    h ^= data;
    h *= fnvPrime;

    return h;
}

struct HasherFnv64 {
    using Hash = Hash64;
    static constexpr Hash begin() noexcept {
        return hashFnv64Begin();
    }
    static constexpr Hash block(Hash64 h, std::span<const std::uint64_t> data) noexcept {
        return hashFnv64Block64(h, data);
    }
//    template <class It>
//    static constexpr Hash blockIt(Hash64 h, It begin, It end) noexcept {
//        return hashFnv64Block64It(h, begin, end);
//    }
    static constexpr Hash val(Hash64 h, std::uint64_t val) noexcept {
        return hashFnv64Val64(h, val);
    }
};



/*
// Computes hash of any object
template <class T>
constexpr Hash64 hashObject(const T &obj) noexcept
{
    constexpr auto SizeOfT = sizeof(T);
    constexpr auto SizeOfH = sizeof(Hash64);
    
    constexpr auto count = SizeOfT / SizeOfH;
    constexpr auto rem = SizeOfT % SizeOfH;

    auto ptr = reinterpret_cast<const std::uint64_t *>(std::addressof(obj));
    using Hasher = HasherFnv64;

    //auto h = hashHorner64Begin();
    auto h = Hasher::begin();

    if constexpr (count != 0) {
        //h = hashHorner64Block64(h, {ptr, count});
        h = Hasher::block(h, {ptr, count});
    }
        
    if constexpr (rem != 0) {
      
        Hash64 pad{};
        std::memcpy(&pad, ptr + count, rem);

        //h = hashHorner64Val64(h, pad);
        h = Hasher::val(h, pad);
    }

    return h;
}
*/

template <class Data>
constexpr Hash64 hashDataFnv(Data data) noexcept {
    constexpr auto SizeOfH = sizeof(Hash64);
    auto count = data.size() / SizeOfH;
    auto rem = data.size() % SizeOfH;

    using Hasher = HasherFnv64;

    auto ptr = reinterpret_cast<const std::uint64_t *>(data.data());

    auto h = Hasher::begin();

    if (count != 0) {
        h = Hasher::block(h, {ptr, count});
    }
        
    if (rem != 0) {
        Hash64 pad{};
        std::memcpy(&pad, ptr + count, rem);

        h = Hasher::val(h, pad);
    }

    return h;
}

struct ParserHasher {
    using H = Hash64;
    using value_type = Hash64;

    constexpr H operator()(std::string_view s) const noexcept { 
        return hashAsciiHorner(s); 
    }
};

constexpr Hash64 pshHash(std::string_view str) { return hashAsciiHorner(str); }

inline namespace literals {
constexpr uint64_t operator"" _psh(const char *p, std::size_t s) noexcept { 
    return hashAsciiHorner(std::string_view(p,s)); 
}
}  // namespace literals



struct StringHasher {
    using H = Hash64;
    using value_type = Hash64;

    constexpr value_type operator()(const char *s) const noexcept { 
        return hashHorner(s); 
    }

// #ifdef __GNUC__  // temporary until constexpr std::string is implemented
//     value_type operator()(const std::string &s) const noexcept {
//         return operator()(s.c_str());
//     }
// #else
    constexpr value_type operator()(const std::string &s) const noexcept { 
        return operator()(s.c_str()); 
    }
// #endif
};



inline namespace literals {
constexpr Hash64 operator"" _shash(const char *s, std::size_t) noexcept { 
    return StringHasher()(s); 
}
}   // namespace literals

}   // namespace apl
