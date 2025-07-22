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

#include <apeal/parse/yaml.hpp>


template <class S>
struct YamlDumper {
    using Yaml = apl::YamlTemplate<S>;
    using Node = apl::YamlNodeTemplate<S>; // typename Yaml::Node;
    using NodeA = const Node&;
    using Index = apl::Index;

    bool showLineNumbers{true};
    apl::Index indent_{};
    apl::Console& con_;
    S out_;

    YamlDumper(apl::Console& con) noexcept: con_{con} {}


    void dump(const Yaml& yaml) {
        if (yaml.versionHi || yaml.versionLo)
            con_("Version: %d.%d", yaml.versionHi, yaml.versionLo);

        auto numDocs = yaml.documents.size();

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
    }

    void dumpNode(NodeA parent, NodeA node) {
        dumpLine(node);
        dumpIndent(indent_);

        if (parent.isSeq()) {
            con_.print("- ");
        }

        dumpNodeKey(node);
        generateNodeValue(node);

        // if (parent.isSeq()) {
        //     --indent_;
        // }

        // con_.nl();

        /*
        if (node.isSeq()) {
            con_.print("- ");
            dumpNodeValue(node);
            con_.nl();
            return;
        }

        if (node.isScalar()) {
            dumpNodeValue(node);
            con_.nl();
            return;
        }

        // Check the nature of sub nodes
        if (node.isFlow()) {

            auto openChar = node.isSeq() ? '[' : '{';
            auto closeChar = node.isSeq() ? ']' : '}';

            if (!node.key.empty())
                con_.print(": %c", openChar);
            else
                con_.print(openChar);


            bool first = true;
            for (auto& n: node.nodes) {
                if (!first)
                    con_.print(", ");
                else
                    first = false;
                dumpNodeValue(n);
                //print("%s", n.value);
            }

            con_(closeChar);
            return;
        }

        // Values
        con_.nl();
        for (auto& n: node) {
            ++indent;
            dumpNode(n);
            --indent;
        }
        */
    }


    void dumpLine(NodeA node) {
        if (!showLineNumbers)
            return;
        // con_.print("%3d| ", node.line + 1);
        // con_.println("# %d|", node.line + 1);
    }

    void dumpIndent(Index level) {
        auto spaces = level * 2;

        for (Index i = 0; i < spaces; ++i)
            con_.print(' ');
    }

    void dumpNodeKey(NodeA node) {
        auto& tag = node.tag;
        auto tagSymbol = tag.isBuiltIn() ? "!!" : "!";

        if (node.key.empty())
            return;

        // Dump key name
        if (tag.isNone())
            con_.print("%s", node.key);
        else
            con_.print("%s%s %s", tagSymbol, tag.name, node.key);

        //        auto haveValue = !node.value.empty() || !node.nodes.empty();

        //        if (!haveValue)
        //            return;

        con_.print(": ");
    }

    void generateNodeValue(NodeA node) {
        if (node.isScalar()) {
            con_.println("%s", node.value);
            return;
        }

        if (node.isFlowSeq()) {
            auto allScalar = true;
            for (auto& child: node) {
                if (!child.isScalar()) {
                    allScalar = false;
                    break;
                }
            }

            if (allScalar) {
                con_.print('[');
                auto first = true;
                for (auto& child: node) {
                    if (first)
                        first = false;
                    else
                        con_.print(", ");
                    con_.print(child.value);
                }
                con_.println(']');
            } else {
                con_.nl();

                ++indent_;

                for (const auto& child: node) {
                    dumpIndent(indent_);
                    dumpLine(node);
                    con_.print("- ");
                    generateNodeValue(child);
                    con_.nl();
                }

                --indent_;
                // con_.nl();
            }

            return;
        }

        // If map
        con_.nl();

        for (auto& child: node) {
            ++indent_;

            if (child.isScalar()) {
                dumpIndent(indent_);
                if (node.isSeq()) {
                    con_.print("- ");
                    ++indent_;
                }
                dumpNodeKey(child);

                generateNodeValue(child);

                if (node.isSeq()) {
                    --indent_;
                }
            } else {
                dumpNode(node, child);
            }

            // con_.nl();
            // dumpNode(node, child);

            --indent_;


            // dumpLine(child);
        }
    }
};


template <class Yaml>
void dumpYaml(const Yaml& yaml) {
    YamlDumper<typename Yaml::Str> dumper{con_};

    dumper.dump(yaml);
}
