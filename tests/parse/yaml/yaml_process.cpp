/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <test_yaml.hpp>

#include <apeal/parse/yaml.hpp>


template <class Yaml>
struct DumpProcessor: public apl::YamlProcessorBase<Yaml> {
    using Node = typename Yaml::Node;
    using NodeA = const Node&;
    using StrView = apl::StrView;

    int indent_ = 0;

    void onVersion(int hi, int lo) { con_("yaml::version: %d.%d", hi, lo); }

    void onTag() {}
    void onComment(StrView comment) { con_("comment %s", comment); }
    void onBegin() { con_("yaml::begin"); }
    void onEnd() { con_("yaml::end"); }

    void onBeginDocument(const typename Yaml::Tag& tag, apl::YamlValueType valueType) {
        apl::println("doc::begin");
    }

    void onEndDocument() { con_("doc::end"); }

    void onBeginNode(NodeA node) {
        printIndent(indent_++);
        // apl::println("node::begin");
        processNodeBegin(node);
        //++indent_;
    }

    void onEndNode(NodeA node) {
        --indent_;
        if (node.isScalar()) {
            con_("}");
        }

        if (node.isCollection()) {
            printIndent(indent_);
            con_.print("node::end");
            if (!node.key.empty()) {
                con_.print(apl::fmt(": '%s'", node.key));
            }
            con_.nl();
        }
    }

    void onWarning(StrView text) { con_("warn: %s", text); }
    void onError(StrView text) { con_("err: %s", text); }

private:
    void processNodeBegin(NodeA node) {
        // printIndent(indent_ + 1);
        if (node.isCollection()) {
            con_.print("node::begin:");
        } else {
            con_.print("node:");
        }
        con_.print(" {");

        if (node.isFlowSeq())
            con_.print("type:flow-seq");
        else if (node.isSeq())
            con_.print("type:seq");

        if (node.isFlowMap())
            con_.print("type:flow-map");
        else if (node.isMap())
            con_.print("type:map");

        if (node.isScalar())
            con_.print("type:scalar");

        if (!node.key.empty()) {
            con_.print(", key: '%s'", node.key);
        }

        if (!node.isCollection()) {
            con_.print(", value: '%s'", node.value);
        }

        con_.print(", indent: %d", node.indent);

        if (node.isCollection()) {
            con_('}');
        }
    }

    void printIndent(int indent) {
        for (auto i = 0; i < indent; ++i)
            con_.print("  ");
    }
};


void body() {
    using namespace apl;

    auto filename = requireCmdlineArg();

    YamlParserProcessor<DumpProcessor<Yaml>, Yaml> parser;
    parser.load(filename);
}
