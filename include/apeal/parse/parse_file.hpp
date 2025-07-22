/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../file.hpp"
#include "../string.hpp"

namespace apl {

namespace impl {

struct ParseFileInput {
    using Iterator = std::string::const_iterator;

    std::string data;

    ParseFileInput(StrView filename) {
        loadFile(filename, &data);
    }

    auto begin() const noexcept {  return data.begin();  }
    auto end() const noexcept {  return data.end();  }
};

}  // namespace impl

}  // namespace apl

