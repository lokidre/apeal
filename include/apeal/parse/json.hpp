/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "parse_base.hpp"
#include "../parse.hpp"

#include <vector>
#include <stack>
#include <functional>
#include "json_object.hpp"
#include "json_processor.hpp"
#include "json_parser.hpp"
#include "json_builder.hpp"


namespace apl {

//
// Json object
//
using JsonNode = impl::JsonNode<String>;
using Json = impl::JsonNode<String>;
using JsonView = impl::JsonNode<StrView>;


using JsonProcessor = impl::JsonProcessor<Json>;


template <class Processor, class Obj>
using JsonParserProcessor = ParserTemplate<impl::JsonParserImpl<Obj, Processor>, Obj>;

using JsonParser = JsonParserProcessor<JsonProcessor, Json>;


//using JsonParser = ParserTemplate<impl::JsonParserImpl<Json>, Json>;



inline Json parseJson(StrView contents) {
    JsonParser parser;
    return parser.parse(contents);
}

inline Json loadJson(StrView filename) {
    JsonParser parser;
    return parser.load(filename);
}


} // namespace apl
