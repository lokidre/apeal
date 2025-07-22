/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "split.hpp"
#include "parse_base.hpp"

#include "int.hpp"
#include "bool.hpp"

#include "yaml_object.hpp"
#include "yaml_generator.hpp"
#include "yaml_processor.hpp"
#include "yaml_parser.hpp"
#include "yaml_builder.hpp"

namespace apl {

template <class S>
using YamlNodeTemplate = impl::YamlNode<S>;

template <class S>
using YamlTemplate = impl::Yaml<S>;


// String based parser and object
using YamlNode = YamlNodeTemplate<String>;
using Yaml = YamlTemplate<String>;



//using YamlNodeView = YamlNodeTemplate<StrView>;
//using YamlView = impl::Yaml<YamlNodeView>;

using YamlProcessor = impl::YamlProcessor<Yaml>;
//using YamlProcessorView = impl::YamlProcessor<YamlView>;

template <class Processor, class Obj>
using YamlParserProcessor = ParserTemplate<impl::YamlParserImpl<Obj, Processor>, Obj>;


using YamlParser = YamlParserProcessor<YamlProcessor, Yaml>;
//using YamlParserView = YamlParserProcessor<YamlProcessor, YamlView>;


using YamlBuilderString = impl::YamlBuilderBody<String>;
using YamlBuilder = YamlBuilderString;


inline
Yaml parseYaml(StrView contents) {
    YamlParser parser;
    return parser.parse(contents);
}

/* TODO: need to implement
inline
YamlView parseYamlView(StrView contents) {
    YamlParserView parser;
    return parser.parse(contents);
}
*/


inline
Yaml loadYaml(StrView filename) {
    YamlParser parser;
    return parser.load(filename);
}

inline
String generateYaml(const Yaml& yaml) {
    YamlGenerator<String> gen;
    return gen.generate(yaml);
}

//inline
//YamlView loadYamlView(StrView filename) {
//    YamlParserView parser;
//    return parser.load(filename);
//}


}  // namespace apl

