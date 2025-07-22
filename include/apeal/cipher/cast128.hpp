/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#if APEAL_HEADER_DOC___

    Implementation of CAST 128 encryption

        RFC-2144


Since CAST128 algorithm works with 64 bit blocks,
the input data must be aligned to 64-bit blocks.
The application has to keep the original length of the data.


    Key Generation

The key length is 128 bits (16 bytes).

If the provided key length is less than 16, then it will padded by zeroes.
If the provided key length is more than 16, then extra bytes are ignored.


#endif



#include "../string.hpp"

#include <cstring>
#include <cstdint>

namespace apl {



class CipherCast128 {
public:
    struct Key { uint32_t Km[16], Kr[16]; };


private:
    // Table
    static const uint32_t S1[256], S2[256], S3[256], S4[256], S5[256], S6[256], S7[256], S8[256];



public:
    void generateKey(StrView data, Key* key) {
        gen_key((const uint8_t *)data.data(), data.size(), key->Km, key->Kr);
    }

    void gen_key(const void *data, size_t len, Key &key) { gen_key((const unsigned char *)data, len, key.Km, key.Kr); }
    void gen_key(const char *str, Key &key) { gen_key((const unsigned char *)str, strlen(str), key.Km, key.Kr); }
    void gen_key(const std::string &data, Key &key) { gen_key((const unsigned char *)data.data(), data.size(), key.Km, key.Kr); }


    String encrypt(StrView data, const Key &key) {
        String encrypted;
        if (data.empty())
            return encrypted;

        auto remainder = data.size() % 8;
        std::size_t encryptedSize = (data.size() / 8 + (remainder ? 1 : 0)) * 8;
        encrypted.resize(encryptedSize);

        encryptData(data.data(), encryptedSize, key, encrypted.data());
        //if ((encrypedSize % 8) !=


        return encrypted;
    }

    String decrypt(StrView encrypted, const Key& key) {
        String decrypted;
        if (encrypted.empty())
            return decrypted;

        decrypted.resize(encrypted.size());

        decryptData(encrypted.data(), encrypted.size(), key, decrypted.data());

        return decrypted;
    }

public:
    static void encryptBlock(uint32_t &L, uint32_t &R, const uint32_t *Km, const uint32_t *Kr) noexcept {
        f1< 0>(L, R, Km, Kr);
        f2< 1>(R, L, Km, Kr);
        f3< 2>(L, R, Km, Kr);
        f1< 3>(R, L, Km, Kr);
        f2< 4>(L, R, Km, Kr);
        f3< 5>(R, L, Km, Kr);
        f1< 6>(L, R, Km, Kr);
        f2< 7>(R, L, Km, Kr);
        f3< 8>(L, R, Km, Kr);
        f1< 9>(R, L, Km, Kr);
        f2<10>(L, R, Km, Kr);
        f3<11>(R, L, Km, Kr);
        f1<12>(L, R, Km, Kr);
        f2<13>(R, L, Km, Kr);
        f3<14>(L, R, Km, Kr);
        f1<15>(R, L, Km, Kr);
    }

    static void decryptBlock(uint32_t &L, uint32_t &R, const uint32_t Km[16], const uint32_t Kr[16]) noexcept {
        f1<15>(L, R, Km, Kr);
        f3<14>(R, L, Km, Kr);
        f2<13>(L, R, Km, Kr);
        f1<12>(R, L, Km, Kr);
        f3<11>(L, R, Km, Kr);
        f2<10>(R, L, Km, Kr);
        f1< 9>(L, R, Km, Kr);
        f3< 8>(R, L, Km, Kr);
        f2< 7>(L, R, Km, Kr);
        f1< 6>(R, L, Km, Kr);
        f3< 5>(L, R, Km, Kr);
        f2< 4>(R, L, Km, Kr);
        f1< 3>(L, R, Km, Kr);
        f3< 2>(R, L, Km, Kr);
        f2< 1>(L, R, Km, Kr);
        f1< 0>(R, L, Km, Kr);
    }


private:

    // Prepare all the types, so the actual algorithm can be copied from the RFC with minimal changes
    class SwapDword {
    private:
        uint32_t data;
        uint8_t *d0, *d1, *d2, *d3;

    public:
        SwapDword(uint8_t *x)
        {
            d0 = &x[0], d1 = &x[1], d2 = &x[2], d3 = &x[3];
            data = 0xFFFFFFFFL & ((*d0<<24) | (*d1<<16) | (*d2<<8) | (*d3));
        }
        void operator = (uint32_t d)
        {
            data = d;
            *d0 = (uint8_t)(d>>24), *d1 = (uint8_t)(d>>16), *d2 = (uint8_t)(d>>8), *d3 = (uint8_t)(d);
        }

        uint32_t operator ^ (uint32_t d)
        {
            return data ^ d;
        }
    };




    template <typename T>
    static constexpr T rotl(T a, int n) noexcept {
        return (a<<n) | (a>>((sizeof(T)*8)-n));
    }

    static constexpr uint32_t n2l(const uint8_t *b) noexcept {
        //return 0xFFFFFFFFL & ((uint32_t(b[0])<<24) | (uint32_t(b[1])<<16) | (uint32_t(b[2])<<8) | uint32_t(b[3]));
        return (uint32_t(b[0])<<24) | (uint32_t(b[1])<<16) | (uint32_t(b[2])<<8) | uint32_t(b[3]);
    }

    static constexpr void l2n(uint32_t d, uint8_t *b) noexcept {
        b[0] = (uint8_t)(d>>24);
        b[1] = (uint8_t)(d>>16);
        b[2] = (uint8_t)(d>>8);
        b[3] = (uint8_t)(d);
    }

    template <int i>
    static void f1(uint32_t &L, uint32_t D, const uint32_t *Km, const uint32_t *Kr) noexcept
    {
        auto I = rotl(Km[i] + D, Kr[i]);

        int Ia = (I>>8) & 0xff;
        int Ib = I & 0xff;
        int Ic = (I>>24) & 0xff;
        int Id = (I>>16) & 0xFF;

        L ^= ((S1[Ia] ^ S2[Ib]) - S3[Ic]) + S4[Id];
    }

    template <int i>
    static void f2(uint32_t &L, uint32_t D, const uint32_t *Km, const uint32_t *Kr) noexcept
    {
        auto I = rotl(Km[i]^D, Kr[i]);

        int Ia = (I>>8)&0xff;
        int Ib = (I)&0xff;
        int Ic = (I>>24)&0xff;
        int Id = (I>>16)&0xFF;

        L ^= ((S1[Ia] - S2[Ib]) + S3[Ic]) ^ S4[Id];
    }

    template <int i>
    static void f3(uint32_t &L, uint32_t D, const uint32_t *Km, const uint32_t *Kr) noexcept
    {
        auto I = rotl(Km[i] - D, Kr[i]);

        int Ia = (I>>8) & 0xff;
        int Ib = (I) & 0xff;
        int Ic = (I>>24) & 0xff;
        int Id = (I>>16) & 0xff;

        L ^= ((S1[Ia] + S2[Ib]) ^ S3[Ic]) - S4[Id];
    }



    void gen_key(const uint8_t *key, std::size_t len, uint32_t *Km, uint32_t *Kr)
    {
        uint8_t x[16], z[16];
        uint32_t K[32];
        std::size_t i;

        if (len > 16)  // maximum key length is 16*8=128 bits
            len = 16;

        for (i = 0; i < len; i++)
            x[i] = key[i];

        for (i = len; i < 16; i++)  // pad with zeroes
            x[i] = 0;

        uint32_t &K1 = K[0], &K2 = K[1], &K3 = K[2], &K4 = K[3], &K5 = K[4], &K6 = K[5], &K7 = K[6], &K8 = K[7],
            &K9 = K[8], &K10 = K[9], &K11 = K[10], &K12 = K[11], &K13 = K[12], &K14 = K[13], &K15 = K[14], &K16 = K[15],
            &K17 = K[16], &K18 = K[17], &K19 = K[18], &K20 = K[19], &K21 = K[20], &K22 = K[21], &K23 = K[22], &K24 = K[23],
            &K25 = K[24], &K26 = K[25], &K27 = K[26], &K28 = K[27], &K29 = K[28], &K30 = K[29], &K31 = K[30], &K32 = K[31];

        uint8_t &x0 = x[0], &x1 = x[1], &x2 = x[2], &x3 = x[3], &x4 = x[4], &x5 = x[5], &x6 = x[6], &x7 = x[7],
            &x8 = x[8], &x9 = x[9], &xA = x[10], &xB = x[11], &xC = x[12], &xD = x[13], &xE = x[14], &xF = x[15];
        SwapDword x0x1x2x3(&x[0]), x4x5x6x7(&x[4]), x8x9xAxB(&x[8]), xCxDxExF(&x[12]);

        uint8_t &z0 = z[0], &z1 = z[1], &z2 = z[2], &z3 = z[3], &z4 = z[4], &z5 = z[5], &z6 = z[6], &z7 = z[7],
            &z8 = z[8], &z9 = z[9], &zA = z[10], &zB = z[11], &zC = z[12], &zD = z[13], &zE = z[14], &zF = z[15];
        SwapDword z0z1z2z3(&z[0]), z4z5z6z7(&z[4]), z8z9zAzB(&z[8]), zCzDzEzF(&z[12]);


        z0z1z2z3 = x0x1x2x3 ^ S5[xD] ^ S6[xF] ^ S7[xC] ^ S8[xE] ^ S7[x8];
        z4z5z6z7 = x8x9xAxB ^ S5[z0] ^ S6[z2] ^ S7[z1] ^ S8[z3] ^ S8[xA];
        z8z9zAzB = xCxDxExF ^ S5[z7] ^ S6[z6] ^ S7[z5] ^ S8[z4] ^ S5[x9];
        zCzDzEzF = x4x5x6x7 ^ S5[zA] ^ S6[z9] ^ S7[zB] ^ S8[z8] ^ S6[xB];
        K1 = S5[z8] ^ S6[z9] ^ S7[z7] ^ S8[z6] ^ S5[z2];
        K2 = S5[zA] ^ S6[zB] ^ S7[z5] ^ S8[z4] ^ S6[z6];
        K3 = S5[zC] ^ S6[zD] ^ S7[z3] ^ S8[z2] ^ S7[z9];
        K4 = S5[zE] ^ S6[zF] ^ S7[z1] ^ S8[z0] ^ S8[zC];
        x0x1x2x3 = z8z9zAzB ^ S5[z5] ^ S6[z7] ^ S7[z4] ^ S8[z6] ^ S7[z0];
        x4x5x6x7 = z0z1z2z3 ^ S5[x0] ^ S6[x2] ^ S7[x1] ^ S8[x3] ^ S8[z2];
        x8x9xAxB = z4z5z6z7 ^ S5[x7] ^ S6[x6] ^ S7[x5] ^ S8[x4] ^ S5[z1];
        xCxDxExF = zCzDzEzF ^ S5[xA] ^ S6[x9] ^ S7[xB] ^ S8[x8] ^ S6[z3];
        K5 = S5[x3] ^ S6[x2] ^ S7[xC] ^ S8[xD] ^ S5[x8];
        K6 = S5[x1] ^ S6[x0] ^ S7[xE] ^ S8[xF] ^ S6[xD];
        K7 = S5[x7] ^ S6[x6] ^ S7[x8] ^ S8[x9] ^ S7[x3];
        K8 = S5[x5] ^ S6[x4] ^ S7[xA] ^ S8[xB] ^ S8[x7];
        z0z1z2z3 = x0x1x2x3 ^ S5[xD] ^ S6[xF] ^ S7[xC] ^ S8[xE] ^ S7[x8];
        z4z5z6z7 = x8x9xAxB ^ S5[z0] ^ S6[z2] ^ S7[z1] ^ S8[z3] ^ S8[xA];
        z8z9zAzB = xCxDxExF ^ S5[z7] ^ S6[z6] ^ S7[z5] ^ S8[z4] ^ S5[x9];
        zCzDzEzF = x4x5x6x7 ^ S5[zA] ^ S6[z9] ^ S7[zB] ^ S8[z8] ^ S6[xB];
        K9 = S5[z3] ^ S6[z2] ^ S7[zC] ^ S8[zD] ^ S5[z9];
        K10 = S5[z1] ^ S6[z0] ^ S7[zE] ^ S8[zF] ^ S6[zC];
        K11 = S5[z7] ^ S6[z6] ^ S7[z8] ^ S8[z9] ^ S7[z2];
        K12 = S5[z5] ^ S6[z4] ^ S7[zA] ^ S8[zB] ^ S8[z6];
        x0x1x2x3 = z8z9zAzB ^ S5[z5] ^ S6[z7] ^ S7[z4] ^ S8[z6] ^ S7[z0];
        x4x5x6x7 = z0z1z2z3 ^ S5[x0] ^ S6[x2] ^ S7[x1] ^ S8[x3] ^ S8[z2];
        x8x9xAxB = z4z5z6z7 ^ S5[x7] ^ S6[x6] ^ S7[x5] ^ S8[x4] ^ S5[z1];
        xCxDxExF = zCzDzEzF ^ S5[xA] ^ S6[x9] ^ S7[xB] ^ S8[x8] ^ S6[z3];
        K13 = S5[x8] ^ S6[x9] ^ S7[x7] ^ S8[x6] ^ S5[x3];
        K14 = S5[xA] ^ S6[xB] ^ S7[x5] ^ S8[x4] ^ S6[x7];
        K15 = S5[xC] ^ S6[xD] ^ S7[x3] ^ S8[x2] ^ S7[x8];
        K16 = S5[xE] ^ S6[xF] ^ S7[x1] ^ S8[x0] ^ S8[xD];
        z0z1z2z3 = x0x1x2x3 ^ S5[xD] ^ S6[xF] ^ S7[xC] ^ S8[xE] ^ S7[x8];
        z4z5z6z7 = x8x9xAxB ^ S5[z0] ^ S6[z2] ^ S7[z1] ^ S8[z3] ^ S8[xA];
        z8z9zAzB = xCxDxExF ^ S5[z7] ^ S6[z6] ^ S7[z5] ^ S8[z4] ^ S5[x9];
        zCzDzEzF = x4x5x6x7 ^ S5[zA] ^ S6[z9] ^ S7[zB] ^ S8[z8] ^ S6[xB];
        K17 = S5[z8] ^ S6[z9] ^ S7[z7] ^ S8[z6] ^ S5[z2];
        K18 = S5[zA] ^ S6[zB] ^ S7[z5] ^ S8[z4] ^ S6[z6];
        K19 = S5[zC] ^ S6[zD] ^ S7[z3] ^ S8[z2] ^ S7[z9];
        K20 = S5[zE] ^ S6[zF] ^ S7[z1] ^ S8[z0] ^ S8[zC];
        x0x1x2x3 = z8z9zAzB ^ S5[z5] ^ S6[z7] ^ S7[z4] ^ S8[z6] ^ S7[z0];
        x4x5x6x7 = z0z1z2z3 ^ S5[x0] ^ S6[x2] ^ S7[x1] ^ S8[x3] ^ S8[z2];
        x8x9xAxB = z4z5z6z7 ^ S5[x7] ^ S6[x6] ^ S7[x5] ^ S8[x4] ^ S5[z1];
        xCxDxExF = zCzDzEzF ^ S5[xA] ^ S6[x9] ^ S7[xB] ^ S8[x8] ^ S6[z3];
        K21 = S5[x3] ^ S6[x2] ^ S7[xC] ^ S8[xD] ^ S5[x8];
        K22 = S5[x1] ^ S6[x0] ^ S7[xE] ^ S8[xF] ^ S6[xD];
        K23 = S5[x7] ^ S6[x6] ^ S7[x8] ^ S8[x9] ^ S7[x3];
        K24 = S5[x5] ^ S6[x4] ^ S7[xA] ^ S8[xB] ^ S8[x7];
        z0z1z2z3 = x0x1x2x3 ^ S5[xD] ^ S6[xF] ^ S7[xC] ^ S8[xE] ^ S7[x8];
        z4z5z6z7 = x8x9xAxB ^ S5[z0] ^ S6[z2] ^ S7[z1] ^ S8[z3] ^ S8[xA];
        z8z9zAzB = xCxDxExF ^ S5[z7] ^ S6[z6] ^ S7[z5] ^ S8[z4] ^ S5[x9];
        zCzDzEzF = x4x5x6x7 ^ S5[zA] ^ S6[z9] ^ S7[zB] ^ S8[z8] ^ S6[xB];
        K25 = S5[z3] ^ S6[z2] ^ S7[zC] ^ S8[zD] ^ S5[z9];
        K26 = S5[z1] ^ S6[z0] ^ S7[zE] ^ S8[zF] ^ S6[zC];
        K27 = S5[z7] ^ S6[z6] ^ S7[z8] ^ S8[z9] ^ S7[z2];
        K28 = S5[z5] ^ S6[z4] ^ S7[zA] ^ S8[zB] ^ S8[z6];
        x0x1x2x3 = z8z9zAzB ^ S5[z5] ^ S6[z7] ^ S7[z4] ^ S8[z6] ^ S7[z0];
        x4x5x6x7 = z0z1z2z3 ^ S5[x0] ^ S6[x2] ^ S7[x1] ^ S8[x3] ^ S8[z2];
        x8x9xAxB = z4z5z6z7 ^ S5[x7] ^ S6[x6] ^ S7[x5] ^ S8[x4] ^ S5[z1];
        xCxDxExF = zCzDzEzF ^ S5[xA] ^ S6[x9] ^ S7[xB] ^ S8[x8] ^ S6[z3];
        K29 = S5[x8] ^ S6[x9] ^ S7[x7] ^ S8[x6] ^ S5[x3];
        K30 = S5[xA] ^ S6[xB] ^ S7[x5] ^ S8[x4] ^ S6[x7];
        K31 = S5[xC] ^ S6[xD] ^ S7[x3] ^ S8[x2] ^ S7[x8];
        K32 = S5[xE] ^ S6[xF] ^ S7[x1] ^ S8[x0] ^ S8[xD];

        // Section 2.4.1. Masking Subkeys And Rotate Subkeys
        for (i = 0; i < 16; i++) {
            Km[i] = K[i];
            Kr[i] = (K[16+i]+16) & 0x1F;
        }

    }


    void encrypt_ecb(uint8_t *data, Key &key) {
        auto L = n2l(data);
        auto R = n2l(data+4);
        encryptBlock(L, R, key.Km, key.Kr);
        l2n(R, data);
        l2n(L, data+4);
    }

    void decrypt_ecb(uint8_t *data, Key &key)
    {
        auto L = n2l(data);
        auto R = n2l(data+4);
        decryptBlock(L, R, key.Km, key.Kr);
        l2n(R, data);
        l2n(L, data+4);
    }

    void encryptData(const void *inData, std::size_t len, const Key &key, void *outData)
    {
        auto in = (const uint8_t *)inData;
        auto out = (uint8_t *)outData;

        size_t i, main_len = len/8;

        auto remainingLength = len % 8;

        // do the main cycle by 64 bit blocks
        auto in0 = in, in4 = in0+4;
        auto out0 = out, out4 = out0+4;
        for (i = 0; i < main_len; i++, in0 += 8, in4 += 8, out0 += 8, out4 += 8) {
            auto L = n2l(in0), R = n2l(in4);
            encryptBlock(L, R, key.Km, key.Kr);
            l2n(R, out0), l2n(L, out4);
        }

        // Build the last 64-bit block
        if (remainingLength > 0) {
            uint8_t lastInData[8]{}, lastOutData[8]{};
            for (i = 0; i < remainingLength; i++, in0++)
                lastInData[i] = *in0;

            // Encrypt the last 64-bit block
            auto L = n2l(lastInData);
            auto R = n2l(lastInData + 4);
            encryptBlock(L, R, key.Km, key.Kr);
            l2n(R, lastOutData);
            l2n(L, lastOutData + 4);

//            l2n(R, out0);
//            l2n(L, out4);

            for (i = 0; i < remainingLength; i++, out0++)
                *out0 = lastOutData[i];
        }
    }

    void decryptData(const void *inData, size_t len, const Key &key, void *outData)
    {
        auto in = (const uint8_t *)inData;
        auto out = (uint8_t *)outData;

        size_t i, main_len = len/8;

        auto remainingLength = len % 8;

        // do the main cycle by 64 bit blocks
        auto in0 = in, in4 = in0 + 4;
        auto out0 = out, out4 = out0 + 4;
        for (i = 0; i < main_len; i++, in0 += 8, in4 += 8, out0 += 8, out4 += 8) {
            auto L = n2l(in0);
            auto R = n2l(in4);
            decryptBlock(L, R, key.Km, key.Kr);
            l2n(R, out0);
            l2n(L, out4);
        }

        // Decrypt the last block
        if (remainingLength > 0) {
            //uint8_t last_data[8]{};

//            for (i = 0; i < remainingLength; i++, in0++)
//                last_data[i] = *in0;

            uint8_t lastBlock[8]{};

            //out0 = lastBlock;

            auto L = n2l(in0);
            auto R = n2l(in4);
//            auto L = n2l(last_data);
//            auto R = n2l(last_data + 4);
            decryptBlock(L, R, key.Km, key.Kr);
            l2n(R, lastBlock);
            l2n(L, lastBlock + 4);

            for (i = 0; i < remainingLength; i++, ++out0)
                *out0 = lastBlock[i];

        }
    }


};



}  // namespace apl
