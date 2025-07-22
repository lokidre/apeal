/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "./test_console.hpp"

#include <apeal/parse/json.hpp>
#include <apeal/parse/bool.hpp>


struct JsonDumper {
    using S = apl::String;
    using Json = apl::Json;
    using Node = apl::Json; // typename Yaml::Node;
    using NodeA = const Node&;
    using Index = apl::Index;
    using NodeType = typename Node::Type;
    using ValueType = typename Node::ValueType;


    apl::Index indent_{};
    apl::Console& con_;

    explicit JsonDumper(apl::Console& con) noexcept: con_{con} {}

    void dump(const Json& json) {
        dumpNode(json);
        con_.nl();

        /*
        for (auto& doc: yaml.documents) {
            auto hasDoc = numDocs > 1 || !doc.tag.isNone();


            if (hasDoc) {
                if (doc.tag.isNone())
                    con_("---");
                else
                    con_("--- %s%s", doc.tag.isBuiltIn() ? "!!" : "!", doc.tag.name);
            }

            indent_ = 0;

            for (auto& node: doc) {
                dumpNode(doc, node);
            }

            if (hasDoc)
                con_("...");
        }
         */
    }


    void dumpNode(NodeA node) {
        dumpIndent(indent_);

        if (!node.key.empty()) {
            con_.print("\"");
            con_.print(node.key);
            con_.print("\": ");
        }


        switch (node.type) {
        case NodeType::Object: {
            con_("{");
            ++indent_;
            //dumpIndent(indent_);

            bool was = false;
            for (auto& n: node) {
                if (was) {
                    con_(",");
                } else {
                    was = true;
                }

                ++indent_;
                dumpNode(n);
                --indent_;
            }
            if (was)
                con_.nl();

            --indent_;
            dumpIndent(indent_);
            con_.print("}");
            break;
        }

        case NodeType::Array: {
            bool oneLiner = true;

            for (auto& n: node) {
                if (!n.isScalar()) {
                    oneLiner = false;
                    break;
                }
            }

            if (oneLiner) {
                con_.print('[');
                bool was = false;
                for (auto& n: node) {
                    if (was) {
                        con_.print(',');
                    } else {
                        was = true;
                    }
                    dumpValue(n);
                }

                con_.print(']');
            } else {
                con_('[');
                ++indent_;

                bool was = false;
                for (auto& n: node) {
                    if (was) {
                        con_(',');
                    } else {
                        was = true;
                    }
                    ++indent_;
                    dumpNode(n);
                    --indent_;
                }
                if (was)
                    con_.nl();

                --indent_;
                dumpIndent(indent_);
                con_.print(']');
            }

            break;
        }

        case NodeType::Scalar:
            dumpValue(node);
            break;
        }

    }

    void dumpValue(NodeA node) {
        switch (node.valueType) {
        case ValueType::String:
            con_.print("\"%s\"", apl::JsonBuilder::escape(node.value));
            break;
        case ValueType::Null:
            con_.print("null");
            break;
        case ValueType::Integer:
        case ValueType::Real:
            con_.print("%s", node.value);
            break;
        case ValueType::Boolean:
            con_.print(apl::parseBool(node.value) ? "true" : "false");
            break;
            //ValueType::String == node.valueType) {
        }
    }



    void dumpIndent(Index level) {
        auto spaces = level * 2;

        for (Index i = 0; i < spaces; ++i)
            con_.print(' ');
    }

};


inline
void dumpJson(const apl::Json& json) {
    JsonDumper dumper{con_};

    dumper.dump(json);
}
