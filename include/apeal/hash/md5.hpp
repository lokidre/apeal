/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../string.hpp"
#include "../buffer.hpp"
#include "../file.hpp"
#include "../endian.hpp"

namespace apl {

class HashMD5 {
public:
    static constexpr int ValueBits = 128;
    static constexpr int ValueBytes = 16;

private:
    using U8 = uint8_t;
    using U32 = uint32_t;
    using U64 = uint64_t;

    struct Ctx {
        U32 A_, B_, C_, D_;
        U64 N_;
        U8 buffer_[64];
    } ctx_;


public:
    static constexpr int hashSize = 64;  // Hash size in bytes

    HashMD5()
    {
        init();
    }


    void init()
    {
        // 3.3 Step 3. Initialize MD Buffer
        ctx_.A_ = 0x67452301L;
        ctx_.B_ = 0xEFCDAB89L;
        ctx_.C_ = 0x98BADCFEL;
        ctx_.D_ = 0x10325476L;

        ctx_.N_ = 0;
    }

    void update(const void *data, std::size_t len)
    {
        size_t i;
        auto idx = (std::size_t)((ctx_.N_>>3)&0x3F);

        ctx_.N_ += len<<3;

        size_t part_len = 64 - idx;
        auto in = (const U8 *)data;

        if (len >= part_len) {
            memcpy(&ctx_.buffer_[idx], data, part_len);
            transform(ctx_.buffer_);
            for (i = part_len; i + 63 < len; i += 64)
                transform(&in[i]);
            idx = 0;
        } else {
            i = 0;
        }

        // bufferize the remaining input data
        memcpy(&ctx_.buffer_[idx], &in[i], len - i);
    }


    void final(void *digest)
    {
        // 3.5 Step 5. Output

        size_t idx, pad_len;
        uint8_t padding_data[64];
        uint8_t bits_buffer[8];

        memset(padding_data, 0, 64), padding_data[0] = 0x80;

        // add bit count
        encode64(ctx_.N_, bits_buffer);


        idx = size_t(ctx_.N_>>3)&0x3F;

        pad_len = (idx < 56) ? 56 - idx : 120 - idx;

        update(padding_data, pad_len);
        update(bits_buffer, 8);

        encode32(ctx_.A_, &((uint8_t *)digest)[0]);
        encode32(ctx_.B_, &((uint8_t *)digest)[4]);
        encode32(ctx_.C_, &((uint8_t *)digest)[8]);
        encode32(ctx_.D_, &((uint8_t *)digest)[12]);

        //memset ( &ctx_, 0, sizeof(ctx_) ) ;  // clear the information from memory
    }

private:
    constexpr U32 rotl(U32 x, int n) const noexcept { return (x<<n) | ((x>>(32-n))); }

    constexpr U32 F(U32 X, U32 Y, U32 Z) const noexcept { return ((X&Y) | ((~X)&Z)); }
    constexpr U32 G(U32 X, U32 Y, U32 Z) const noexcept { return ((X&Z) | (Y&(~Z))); }
    constexpr U32 H(U32 X, U32 Y, U32 Z) const noexcept { return (X^Y^Z); }
    constexpr U32 I(U32 X, U32 Y, U32 Z) const noexcept { return (Y^(X|(~Z))); }


    // a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s).
    template <int k, int s>
    constexpr void R0(U32 &a, U32 b, U32 c, U32 d, const U32 *X, U32 Ti) const noexcept
    {
        a = b + rotl(a + F(b, c, d) + X[k] + Ti, s);
    }


    // a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s).
    template <int k, int s>
    constexpr void R1(U32 &a, U32 b, U32 c, U32 d, const U32 *X, U32 Ti)
    {
        a = b + rotl(a + G(b, c, d) + X[k] + Ti, s);
    }


    // a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s)
    template <int k, int s>
    constexpr void R2(U32 &a, U32 b, U32 c, U32 d, const U32 *X, U32 Ti)
    {
        a = b + rotl(a + H(b, c, d) + X[k] + Ti, s);
    }


    // a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s)
    template <int k, int s>
    constexpr void R3(U32 &a, U32 b, U32 c, U32 d, const U32 *X, U32 Ti)
    {
        a = b + rotl(a + I(b, c, d) + X[k] + Ti, s);
    }


    constexpr void encode32(U32 in, U8 *out)
    {
#if APEAL_BYTE_ORDER == APEAL_BIG_ENDIAN
        *(uint32_t *)out = (in>>24)|((in>>8)&0xFF00)|((in<<8)&0xFF0000)|(in<<24);
#elif APEAL_BYTE_ORDER == APEAL_LITTLE_ENDIAN
        *(uint32_t *)out = in;
#else
#error Endianness unknown !
#endif
    }

    constexpr uint32_t decode32(const U8 *in)
    {
#if APEAL_BYTE_ORDER == APEAL_BIG_ENDIAN
        // big endian
        return 0xFFFFFFFFL&((in[0]<<24)|(in[1]<<16)|(in[2]<<8)|(in[3]));
#elif APEAL_BYTE_ORDER == APEAL_LITTLE_ENDIAN
        // little endian (straightforward)
        return *(uint32_t *)in;
#else
#error Endianness unknown !
#endif
    }

    constexpr void encode64(U64 in, U8 *out) noexcept
    {
#if APEAL_BYTE_ORDER == APEAL_BIG_ENDIAN
        encode32(uint32_t(in>>32), out);
        encode32(uint32_t(in), &out[4]);
#elif APEAL_BYTE_ORDER == APEAL_LITTLE_ENDIAN
        // little endian goes straightforward
        *(uint64_t *)out = in;
#else
#error Endianness unknown !
#endif
    }



    constexpr void transform(const U8 *block) noexcept
    {
        U32 X[16];
        auto A = ctx_.A_, B = ctx_.B_, C = ctx_.C_, D = ctx_.D_;

        for (int i = 0; i < 16; i++)
            X[i] = decode32(&block[i*4]);

        // round 1
        R0< 0, 7>(A, B, C, D, X, 0xD76AA478L);
        R0< 1, 12>(D, A, B, C, X, 0xE8C7B756L);
        R0< 2, 17>(C, D, A, B, X, 0x242070DBL);
        R0< 3, 22>(B, C, D, A, X, 0xC1BDCEEEL);
        R0< 4, 7>(A, B, C, D, X, 0xF57C0FAFL);
        R0< 5, 12>(D, A, B, C, X, 0x4787C62AL);
        R0< 6, 17>(C, D, A, B, X, 0xA8304613L);
        R0< 7, 22>(B, C, D, A, X, 0xFD469501L);
        R0< 8, 7>(A, B, C, D, X, 0x698098D8L);
        R0< 9, 12>(D, A, B, C, X, 0x8B44F7AFL);
        R0<10, 17>(C, D, A, B, X, 0xFFFF5BB1L);
        R0<11, 22>(B, C, D, A, X, 0x895CD7BEL);
        R0<12, 7>(A, B, C, D, X, 0x6B901122L);
        R0<13, 12>(D, A, B, C, X, 0xFD987193L);
        R0<14, 17>(C, D, A, B, X, 0xA679438EL);
        R0<15, 22>(B, C, D, A, X, 0x49B40821L);


        // round 2
        R1< 1, 5>(A, B, C, D, X, 0xf61e2562L);
        R1< 6, 9>(D, A, B, C, X, 0xc040b340L);
        R1<11, 14>(C, D, A, B, X, 0x265e5a51L);
        R1< 0, 20>(B, C, D, A, X, 0xe9b6c7aaL);
        R1< 5, 5>(A, B, C, D, X, 0xd62f105dL);
        R1<10, 9>(D, A, B, C, X, 0x02441453L);
        R1<15, 14>(C, D, A, B, X, 0xd8a1e681L);
        R1< 4, 20>(B, C, D, A, X, 0xe7d3fbc8L);
        R1< 9, 5>(A, B, C, D, X, 0x21e1cde6L);
        R1<14, 9>(D, A, B, C, X, 0xc33707d6L);
        R1< 3, 14>(C, D, A, B, X, 0xf4d50d87L);
        R1< 8, 20>(B, C, D, A, X, 0x455a14edL);
        R1<13, 5>(A, B, C, D, X, 0xa9e3e905L);
        R1< 2, 9>(D, A, B, C, X, 0xfcefa3f8L);
        R1< 7, 14>(C, D, A, B, X, 0x676f02d9L);
        R1<12, 20>(B, C, D, A, X, 0x8d2a4c8aL);


        // Round 2
        R2< 5, 4>(A, B, C, D, X, 0xfffa3942L);
        R2< 8, 11>(D, A, B, C, X, 0x8771f681L);
        R2<11, 16>(C, D, A, B, X, 0x6d9d6122L);
        R2<14, 23>(B, C, D, A, X, 0xfde5380cL);
        R2< 1, 4>(A, B, C, D, X, 0xa4beea44L);
        R2< 4, 11>(D, A, B, C, X, 0x4bdecfa9L);
        R2< 7, 16>(C, D, A, B, X, 0xf6bb4b60L);
        R2<10, 23>(B, C, D, A, X, 0xbebfbc70L);
        R2<13, 4>(A, B, C, D, X, 0x289b7ec6L);
        R2< 0, 11>(D, A, B, C, X, 0xeaa127faL);
        R2< 3, 16>(C, D, A, B, X, 0xd4ef3085L);
        R2< 6, 23>(B, C, D, A, X, 0x04881d05L);
        R2< 9, 4>(A, B, C, D, X, 0xd9d4d039L);
        R2<12, 11>(D, A, B, C, X, 0xe6db99e5L);
        R2<15, 16>(C, D, A, B, X, 0x1fa27cf8L);
        R2< 2, 23>(B, C, D, A, X, 0xc4ac5665L);

        // Round 3
        R3< 0,  6>(A, B, C, D, X, 0xf4292244L);
        R3< 7, 10>(D, A, B, C, X, 0x432aff97L);
        R3<14, 15>(C, D, A, B, X, 0xab9423a7L);
        R3< 5, 21>(B, C, D, A, X, 0xfc93a039L);
        R3<12,  6>(A, B, C, D, X, 0x655b59c3L);
        R3< 3, 10>(D, A, B, C, X, 0x8f0ccc92L);
        R3<10, 15>(C, D, A, B, X, 0xffeff47dL);
        R3< 1, 21>(B, C, D, A, X, 0x85845dd1L);
        R3< 8,  6>(A, B, C, D, X, 0x6fa87e4fL);
        R3<15, 10>(D, A, B, C, X, 0xfe2ce6e0L);
        R3< 6, 15>(C, D, A, B, X, 0xa3014314L);
        R3<13, 21>(B, C, D, A, X, 0x4e0811a1L);
        R3< 4,  6>(A, B, C, D, X, 0xf7537e82L);
        R3<11, 10>(D, A, B, C, X, 0xbd3af235L);
        R3< 2, 15>(C, D, A, B, X, 0x2ad7d2bbL);
        R3< 9, 21>(B, C, D, A, X, 0xeb86d391L);

        ctx_.A_ += A, ctx_.B_ += B, ctx_.C_ += C, ctx_.D_ += D;

        //std::memset(X, 0, 16);
    }
};


inline
void hash_MD5(const void *data, std::size_t len, void *digest)
{
    HashMD5 md5;
    md5.init();
    md5.update(data, len);
    md5.final(digest);
}

inline
void hash_MD5(const char *str, void *digest)
{
    return hash_MD5(str, strlen(str), digest);
}

inline
void hash_MD5(std::string_view str, void *digest)
{
    return hash_MD5(str.data(), str.size(), digest);
}


inline
void hash_MD5(const std::string &str, void *digest)
{
    return hash_MD5(str.data(), str.size(), digest);
}


inline
String format_MD5(const void *hash)
{
    String s;
    s.reserve(16 * 2);
    auto h = (const uint8_t *)hash;
    for (int i = 0; i < 16; ++i, ++h)
        str_format_append(s, "%02x", *h);
    return s;
}

template <class Container>
String genDataMD5(const Container &data)
{
    uint8_t digest[16];
    hash_MD5(data.data(), data.size(), digest);
    return format_MD5(digest);
}

// Can process huge files
inline
String file_MD5(StrView filename)
{
    File file;
    file.open(filename, FileAccess::Read);

    HashMD5 md5;
    uint8_t hash[HashMD5::ValueBytes];

    Buffer<char> buffer;
    buffer.alloc(9048);

    md5.init();

    for (;;) {
        auto count = file.readSome(buffer.get(), buffer.size());
        if (!count)
            break;
        md5.update(buffer.get(), size_t(count));
    }

    md5.final(hash);

    file.close();

    return format_MD5(hash);
}

}  // namespace apl
