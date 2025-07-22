/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

namespace apl {

template <class JsonT>
struct JsonProcessorBase {
    JsonT* json{};
};


namespace impl {

template <class JsonT>
struct JsonProcessor: JsonProcessorBase<JsonT> {
    using Node = JsonT;
    using NodeType = typename Node::Type;
    using ValueType = typename Node::ValueType;
    using Str = typename Node::Str;

    using NodeStack = std::stack<Node*>;

    //Json* json;

    NodeStack stack;

//    explicit JsonProcessor(Json* ijson) noexcept : json{ijson} {
//        // json->clear();
//    }

    void pushObject(StrView key, NodeType type = NodeType::Object) noexcept {
        if (stack.empty()) {
            this->json->type = type;
            stack.push(this->json);
            return;
        }

        Node node{};
        node.type = type;
        node.key = key;

        auto curr = stack.top();

        auto& it = curr->nodes.emplace_back(std::move(node));
        stack.push(std::addressof(it));
    }

    void closeObject() noexcept { stack.pop(); }

    void pushArray(StrView key) noexcept { pushObject(key, NodeType::Array); }

    void pushValue(StrView key, const Node& node) noexcept {
        auto curr = stack.top();
        curr->nodes.push_back(node);
    }

    void pushValue(StrView key, StrView value, ValueType valueType) noexcept {

        Node node{};
        node.type = NodeType::Scalar;
        node.key = key;
        node.value = value;
        node.valueType = valueType;
        pushValue(key, node);
    }
};



}  // namespace impl
}  // namespace apl

