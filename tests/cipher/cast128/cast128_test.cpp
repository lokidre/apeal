/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>


#include <apeal/cipher/cast128.hpp>


void body() {
    using namespace apl;

    // 128-bit key
    static size_t key_len = 16;
    static uint8_t key[16] = {0x01, 0x23, 0x45, 0x67, 0x12, 0x34, 0x56, 0x78,
                              0x23, 0x45, 0x67, 0x89, 0x34, 0x56, 0x78, 0x9A};

    static size_t data_len = 11;
    static uint8_t in_data[11] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x13, 0x24, 0x57};

    static size_t encr_len = 16;
    //static uint8_t out_data[16], dec_data[16];


    CipherCast128 cast128;
    CipherCast128::Key cast_key{};

    std::cout << "generating CAST-128 key ..." << std::endl;
    cast128.gen_key(key, key_len, cast_key);

    std::cout << "testing (void *, void *) ... " << std::flush;
//    cast128.encrypt(in_data, data_len, cast_key, out_data);
//    cast128.decrypt(out_data, encr_len, cast_key, dec_data);
//    std::cout << (memcmp(in_data, dec_data, data_len) == 0 ? "ok" : "FAILED") << std::endl;
}
