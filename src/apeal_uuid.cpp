/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/uuid.hpp"

#include "../3rdparty/base58/base58.h"

#ifdef WIN32
#include <Rpc.h>
#pragma comment(lib, "Rpcrt4.lib")
#else
#include "../3rdparty/uuid/uuid_uuid.h"
#endif

namespace apl {

String generateUuidBase58() noexcept
{
#ifdef WIN32
    UUID uuid;
    UuidCreate(&uuid);
#else
    uuid_t uuid;
#ifdef __EMSCRIPTEN__	
    uuid_generate(uuid);
#else
    uuid_generate_random(uuid);
#endif
#endif

    auto begin = (const uint8_t *)&uuid;
    auto id = EncodeBase58(begin, begin + sizeof(uuid));

    return id;
}

}  // namespace apl

