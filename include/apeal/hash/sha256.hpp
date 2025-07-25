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
#include <array>

#include <cstring>
#include <sstream>
#include <iomanip>

#include <apeal/file.hpp>
#include "apeal/string.hpp"
#include "apeal/buffer.hpp"
#include "apeal/endian.hpp"

namespace apl {

class HashSHA256 {

public:
    HashSHA256(): m_blocklen(0), m_bitlen(0) {
        init();
    }

    static constexpr int ValueBits = 256;
    static constexpr int ValueBytes = 32;

    static constexpr int hashSize = 64;  // Hash size in bytes

    void init() {
        m_state[0] = 0x6a09e667;
        m_state[1] = 0xbb67ae85;
        m_state[2] = 0x3c6ef372;
        m_state[3] = 0xa54ff53a;
        m_state[4] = 0x510e527f;
        m_state[5] = 0x9b05688c;
        m_state[6] = 0x1f83d9ab;
        m_state[7] = 0x5be0cd19;
    }

    void update(const void* data, size_t length) {
        for (size_t i = 0; i < length; i++) {
            auto in = (uint8_t*)data;
            m_data[m_blocklen++] = in[i];
            if (m_blocklen == 64) {
                transform();

                // End of the block
                m_bitlen += 512;
                m_blocklen = 0;
            }
        }
    }
    void update(const std::string& data) {
        update(reinterpret_cast<const uint8_t*> (data.c_str()), data.size());
    }
    uint8_t* digest() {
        uint8_t* hash = new uint8_t[32];

        pad();
        revert(hash);

        return hash;
    }
    void final(uint8_t* digest) {
        
        pad();
        revert(digest);
    }

    //static std::string toString(const uint8_t * digest);

private:
    uint8_t  m_data[64];
    uint32_t m_blocklen;
    uint64_t m_bitlen;
    uint32_t m_state[8]; //A, B, C, D, E, F, G, H

    static constexpr std::array<uint32_t, 64> K = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
        0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
        0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
        0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
        0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
        0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
        0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
        0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
        0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };

    constexpr static uint32_t rotr(uint32_t x, uint32_t n) noexcept {
        return (x >> n) | (x << (32 - n));
    }
    constexpr static uint32_t choose(uint32_t e, uint32_t f, uint32_t g) noexcept {
        return (e & f) ^ (~e & g);
    }
    constexpr static uint32_t majority(uint32_t a, uint32_t b, uint32_t c) noexcept {
        return (a & (b | c)) | (b & c);
    }
    constexpr static uint32_t sig0(uint32_t x) noexcept {
        return HashSHA256::rotr(x, 7) ^ HashSHA256::rotr(x, 18) ^ (x >> 3);
    }
    constexpr static uint32_t sig1(uint32_t x) noexcept {
        return HashSHA256::rotr(x, 17) ^ HashSHA256::rotr(x, 19) ^ (x >> 10);
    }
    constexpr void transform()noexcept {
        uint32_t maj, xorA, ch, xorE, sum, newA, newE, m[64];
        uint32_t state[8];

        for (uint8_t i = 0, j = 0; i < 16; i++, j += 4) { // Split data in 32 bit blocks for the 16 first words
            m[i] = (m_data[j] << 24) | (m_data[j + 1] << 16) | (m_data[j + 2] << 8) | (m_data[j + 3]);
        }

        for (uint8_t k = 16; k < 64; k++) { // Remaining 48 blocks
            m[k] = HashSHA256::sig1(m[k - 2]) + m[k - 7] + HashSHA256::sig0(m[k - 15]) + m[k - 16];
        }

        for (uint8_t i = 0; i < 8; i++) {
            state[i] = m_state[i];
        }

        for (uint8_t i = 0; i < 64; i++) {
            maj = HashSHA256::majority(state[0], state[1], state[2]);
            xorA = HashSHA256::rotr(state[0], 2) ^ HashSHA256::rotr(state[0], 13) ^ HashSHA256::rotr(state[0], 22);

            ch = choose(state[4], state[5], state[6]);

            xorE = HashSHA256::rotr(state[4], 6) ^ HashSHA256::rotr(state[4], 11) ^ HashSHA256::rotr(state[4], 25);

            sum = m[i] + K[i] + state[7] + ch + xorE;
            newA = xorA + maj + sum;
            newE = state[3] + sum;

            state[7] = state[6];
            state[6] = state[5];
            state[5] = state[4];
            state[4] = newE;
            state[3] = state[2];
            state[2] = state[1];
            state[1] = state[0];
            state[0] = newA;
        }

        for (uint8_t i = 0; i < 8; i++) {
            m_state[i] += state[i];
        }
    }
    void pad() {

        uint64_t i = m_blocklen;
        uint8_t end = m_blocklen < 56 ? 56 : 64;

        m_data[i++] = 0x80; // Append a bit 1
        while (i < end) {
            m_data[i++] = 0x00; // Pad with zeros
        }

        if (m_blocklen >= 56) {
            transform();
            memset(m_data, 0, 56);
        }

        // Append to the padding the total message's length in bits and transform.
        m_bitlen += m_blocklen * 8;
        m_data[63] = (uint8_t)(m_bitlen);
        m_data[62] = (uint8_t)(m_bitlen >> 8);
        m_data[61] = (uint8_t)(m_bitlen >> 16);
        m_data[60] = (uint8_t)(m_bitlen >> 24);
        m_data[59] = (uint8_t)(m_bitlen >> 32);
        m_data[58] = (uint8_t)(m_bitlen >> 40);
        m_data[57] = (uint8_t)(m_bitlen >> 48);
        m_data[56] = (uint8_t)(m_bitlen >> 56);
        transform();
    }
    void revert(uint8_t* hash) {
        // SHA uses big endian byte ordering
        // Revert all bytes
        for (uint8_t i = 0 ; i < 4 ; i++) {
            for(uint8_t j = 0 ; j < 8 ; j++) {
                hash[i + (j * 4)] = (m_state[j] >> (24 - i * 8)) & 0x000000ff;
            }
        }
    }
};

inline
void hash_SHA256(const void* data, std::size_t len, void* digest)
{
    HashSHA256 sha256;
    //sha256.init();
    sha256.update(data, len);
    //digest = sha256.digest();
    auto in = (uint8_t*)digest;
    sha256.final(in);
    digest = in;
}

inline
void hash_SHA256(std::string_view str, void* digest)
{
    return hash_SHA256(str.data(), str.size(), digest);
}

inline
std::string format_SHA256(const void* digest) {
    std::stringstream s;
    s << std::setfill('0') << std::hex;
    auto in = (uint8_t*)digest;
    for (uint8_t i = 0; i < 32; i++) {
        s << std::setw(2) << (unsigned int)in[i];
    }

    return s.str();
}

template <class Container>
std::string genDataSHA256(const Container& data)
{
    uint8_t digest[16];
    hash_SHA256(data.data(), data.size(), digest);
    return format_SHA256(digest);
}

inline
std::string file_SHA256(apl::StrView filename)
{
    apl::File file;
    file.open(filename, apl::FileAccess::Read);

    HashSHA256 sha256;
    uint8_t hash[HashSHA256::ValueBytes];

    apl::Buffer<char> buffer;
    buffer.alloc(9048);

    sha256.init();

    for (;;) {
        auto count = file.readSome(buffer.get(), buffer.size());
        if (!count)
            break;
        sha256.update(buffer.get(), size_t(count));
    }

    sha256.final(hash);

    file.close();

    return format_SHA256(hash);
}

}  // namespace apl

