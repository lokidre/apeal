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

template <class YamlT>
struct YamlProcessorBase {
    YamlT* yaml{};
};


namespace impl {

template <class YamlT>
struct YamlProcessor: YamlProcessorBase<YamlT> {

    using Node = typename YamlT::Node;
    using NodeA = const Node&;
    using Str = typename Node::Str;

    using YamlProcessorBase<YamlT>::yaml;

    Index currentDocIndex_{0}, currentNodeDepth_{-1};
    bool documentOpened_{false};



    Node& currentNode() noexcept {
        auto& doc = yaml->documents[currentDocIndex_];
        if (currentNodeDepth_ < 0)
            return doc;

        auto it = doc.nodes.rbegin();
        for (Index i = 0; i < currentNodeDepth_; ++i)
            it = it->nodes.rbegin();
        return *it;
    }

    void onBegin() noexcept {}
    void onVersion(int high, int lo) noexcept {}
    void onTag() noexcept {}
    void onComment(StrView comment) noexcept {}

    void onBeginDocument(YamlTag<Str> tag, YamlValueType valueType) {
        auto& doc = yaml->documents.emplace_back();
        doc.tag = tag;
        doc.valueType = valueType;
        //yaml->documents.push_back(node);
        currentDocIndex_ = std::ssize(yaml->documents) - 1;
        currentNodeDepth_ = -1;
        documentOpened_ = true;
    }
    void onEndDocument() {
        verify(currentDocIndex_ >= 0, "Unopened document closed");
        documentOpened_ = false;
        currentNodeDepth_ = -1;
    }

    void onBeginNode(NodeA node) {
        //if (!documentOpened_) {
        //    onBeginDocument(node);
        //} 
        verify(documentOpened_, "Yaml document not opened");

        auto& parent = currentNode();
        parent.nodes.push_back(node);
        ++currentNodeDepth_;
    }

    void onEndNode(NodeA node) {
        //if (currentNodeDepth_ < 0 && documentOpened_) {
            //documentOpened_ = false;
        //    return;
        //}

        verify(currentNodeDepth_ >= 0, fmt("Unended node: %s:%s", node.key, node.value));
        --currentNodeDepth_;
    }

    void onEnd() noexcept {}

    void onWarning(StrView s) noexcept {
        yaml->warnings.emplace_back(Str{s});
    }

    void onError(StrView s) {
        throwError(s);
    }
};

} // namespace impl

} // namespace apl

