/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <test_json.hpp>

//#include <apeal/parse/json.hpp>

//#include <apeal/file.hpp>
//
//#include <string>
//#include <iostream>

/*
template <class Node>
void dumpJsonNode(const Node& node)
{
    using namespace apl;

    using NodeType = typename Node::Type;
    using ValueType = typename Node::ValueType;

    auto outputName = [](StrView name) {
        if (name.empty())
            return;
        print("\"%s\":", name);
    };

    auto fmt = ValueType::String == node.valueType ? "\"%s\"" : "%z";


    switch (node.type) {
    case NodeType::Object:
        println('{');

        for (bool was{}; auto& v: node.nodes) {
            if (was)
                println(',');
            was = true;
            outputName(v.key);
            dumpJsonNode(v);
        }

        println('}');
        break;

    case NodeType::Array:
        print('[');
        //if (NodeType::Object == node.type) {
        for (bool was{}; auto& v: node.nodes) {
            if (was)
                print(',');
            was = true;
            dumpJsonNode(v);
        }

        //} else {
        //    for (bool was{}; auto& v: node.array) {
        //        if (was)
        //            print(", ");
        //        was = true;
        //        print(fmt, v);
        //    }
        //}
        println(']');
        break;

    case NodeType::Scalar:
        print(fmt, node.value);
        break;

    }
}

template <class Json>
void dumpJson(const Json& json)
{
    dumpJsonNode(json);
}
*/

void body()
{
    apl::verify(cmdlineArgC_ > 1, "Argument missing");

    //apl::AJsonParser parser ;
    //apl::AJson json ;
    //parser.load(cmdlineArgV_[1],json) ;

    auto json = apl::loadJson(cmdlineArgV_[1]);
    dumpJson(json);
}

