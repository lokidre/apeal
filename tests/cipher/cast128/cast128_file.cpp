/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/file.hpp>

#include <apeal/cipher/cast128.hpp>

const static apl::StrView pswd_ = "Ab$973Etoi94";


void body() {
    using namespace apl;

    auto filename = requireCmdlineArg();

    auto data = loadFile(filename);

    CipherCast128 cipher;
    CipherCast128::Key key{};

    cipher.generateKey(pswd_, &key);

    auto encrypted = cipher.encrypt(data, key);

    auto decrypted = cipher.decrypt(encrypted, key);

    auto result = data == decrypted.substr(0, data.size());


    int a= 0;
}
