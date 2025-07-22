/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "parse_input.hpp"

#include <stack>
#include <deque>


namespace apl {

namespace impl {

template <class Input, class YamlT, class Processor>
struct YamlParserImplBody: public ParserBase {
    using Yaml = YamlT;
    using YamlP = Yaml*;
    using Str = typename Yaml::Str;
    //using Node = typename Yaml::Node;
    using Node = YamlNode<Str>;
    using NodeP = Node*;
    using NodeR = Node&;
    using NodeA = const Node&;
    using KeyType = YamlKeyType;
    using ValType = YamlValueType;


    YamlParserImplBody(Input& in, YamlP yaml, Processor& proc): input_{in}, processor_{proc} {
        processor_.yaml = yaml;
    }

    void parse() {
        input_.ignoreEol = false;

        processor_.onBegin();

        // Main parsing cycle
        while (!input_.eof()) {
            processLine();
        }

        // Unwind the nodes
        while (!nodeStack_.empty()) {
            endNode();
        }

        processor_.onEnd();
    }


private:
    Input& input_;
    Processor& processor_;

    bool hasNode_{};
    std::deque<Node> nodeStack_;

    bool documentOpened_{false};
    int flowSeqCount_{}, flowMapCount_{};

    void processLine() {
        int indent = 0, contentIndent = 0;

        while (!input_.eof() && isIndent(input_.peek())) {
            ++indent;
            input_.skip();
        }

        //input_.skipSpaces();

        if (input_.eof())
            return;

        // Comments
        if (processComment())
            return;

        // Directive
        if (isDirective() && indent == 0) {
            input_.skip();
            input_.skipSpaces();
            auto dir = readName();

            if ("YAML" == dir) {
                auto [shi, slo] = str_split(str_trim(readLine()), '.');
                auto [hi, lo] = std::pair{parseInt(shi), parseInt(slo)};
                processor_.onVersion(hi, lo);
                // Check the versions themselves
                // According to the specs the high component above supported should be rejected
                if (hi > 1)
                    handleError("Unsupported version: %d.%d", hi, lo);
                if (lo > 2)
                    handleWarning("Unsupported version: %d.%d", hi, lo);
            } else if ("TAG" == dir) {
                // auto line = readLine(input_);
                //  TODO
            } else { // add warnings
                handleWarning("Invalid directive:%s:%s", dir, readLine());
            }
            return;
        }

        if (isEndDocument() && !indent) {
            // input_.skip();
            //  End of document
            processor_.onEndDocument();
            readLine();
            return;
        }

        auto ch = input_.peek();

        contentIndent = indent;

        if ('-' == ch) {
            input_.skip();
            contentIndent++;

            ch = input_.peek();

            if (!indent) {
                if (processNewDocument())
                    return;
            } // if not indent

            if (!documentOpened_) {
                YamlTag<Str> tag{};
                processor_.onBeginDocument(tag, ValType::Seq);
                documentOpened_ = true;
            }

            verify(isSpace(ch) || isEol(ch), "Expected space");


            while(input_.isSpace()) {
                input_.skip();
                contentIndent++;
            }

            while (!nodeStack_.empty()) {
                auto& n = nodeStack_.back();

                if (n.indent < indent)
                    break;

                // Check for empty nodes (no value no subnodes)
                if (YamlValueType::Unknown == n.valueType) {
                    n.valueType = YamlValueType::Scalar;
                    beginNode(n);
                }

                endNode();
            }

            if (!nodeStack_.empty()) {
                updateNode(nodeStack_.back(), ValType::Seq);
            }

            // Check for flow begin
            if (isValFlow(checkFlow())) {
                processFlow();
                return;
            }


            beginNode(initNode(ValType::Unknown, indent));
            processCurrentNode(indent, contentIndent);
            return;
        }  // If '-' in the beginning

        // Skip empty lines
        input_.skipSpaces();
        if (input_.eol()) {
            input_.skip();
            return;
        }


        // if we are here then it's just a node or a scalar
        if (!documentOpened_) {
            YamlTag<Str> tag{};
            processor_.onBeginDocument(tag, YamlValueType::Map);
            documentOpened_ = true;
        }


        //auto mapContinue = false;

        while (!nodeStack_.empty()) {
            auto& n = nodeStack_.back(); // top node
            if (n.indent < indent)
                break;


            // Check for empty nodes (no value no subnodes)
            if (ValType::Unknown == n.valueType) {
                n.valueType = ValType::Scalar;
                beginNode(n);
            }

            endNode();
        }

        if (!nodeStack_.empty()) {
            updateNode(nodeStack_.back(), ValType::Map);
        }

        beginNode(initNode(ValType::Unknown, indent));

        processCurrentNode(indent, contentIndent);
    }

    void processCurrentNode(int nodeIndent, int contentIndent) {
        // empty node
//        if (input_.eol())
//            return beginNode(nodeStack_.back());

        auto ch = input_.peek();

        if (input_.eof())
            return;


        while (isSpace(ch)) {
            ch = input_.read();
        }

        if (input_.eol())
            return;

        auto key = readKey();

        //if (input_.eof()) {
        //    int a = 0;
        //}

        verify(!input_.eof(), "Unexpected EOF");


        ch = input_.peek();

        auto backIt = nodeStack_.rbegin();
        auto backEnd = nodeStack_.rend();
        ++backIt;

        if (':' == ch) {

            if (backIt != nodeStack_.rend()) {

                // check for the parent
                auto& parent = *backIt;
                if (ValType::Seq == parent.valueType) {
                    auto& node = nodeStack_.back();
                    updateNode(node, ValType::Map);
                    //beginNode(initNode(ValType::Map, nodeIndent));
                    beginNode(initNode(ValType::Unknown, contentIndent));
                } else if (parent.valueType == YamlValueType::Unknown) {
                    backIt->valueType = YamlValueType::Map;
                    beginNode(parent);
                }
            }

            auto& node = nodeStack_.back();
            node.key = key;

            input_.skip();
            input_.skipSpaces();

            processNodeValue(node);
            return;

        }

        auto& node = nodeStack_.back();
        node.value = key;
        if (node.valueType == ValType::Unknown) {
            node.valueType = ValType::Scalar;
            beginNode(node);
        }
    }

    auto& initNode(ValType valueType, int nodeIndent) noexcept {
        Node node;
        node.keyType = KeyType::Scalar;
        node.valueType = valueType;
        node.indent = nodeIndent;

        node.line = static_cast<typename Node::IdxType>(input_.line);
        node.column = static_cast<typename Node::IdxType>(input_.column);
        node.index = static_cast<typename Node::IdxType>(input_.index);

        processTypeTag(node.tag);

        nodeStack_.emplace_back(std::move(node));
        return nodeStack_.back();
    }

    auto updateNode(NodeR node, ValType valType) {
        if (node.valueType == ValType::Unknown) {
            node.valueType = valType;
            beginNode(node);
            return true;
        }

//        if (node.valueType != valType) {
//            int a = 0;
//        }
//
//        verify(node.valueType == valType, "Node value type don't match");
        return false;
    }

    auto& beginNode(NodeR node) {
        if (ValType::Unknown != node.valueType) {
            processor_.onBeginNode(node);
        }
        //nodeStack_.push_back(node);
        hasNode_ = true;
        return node;
    }

    auto endNode() {
        if (nodeStack_.empty())
            return false;

        auto& node = nodeStack_.back();

        if (ValType::Unknown == node.valueType) {
            node.valueType = ValType::Scalar;
            processor_.onBeginNode(node);
        }

        processor_.onEndNode(node);
        nodeStack_.pop_back();
        return true;
    }

    auto processNewDocument() {
        auto ch = input_.peek();  // Second dash

        // Check for the new document
        if ('-' == ch) {
            input_.skip();  // Skip the second dash
            ch = input_.read();  // Third dash
            verify('-' == ch, "Invalid sequence --");

            input_.skipSpacesEol();

            // End all the nodes
            while (!nodeStack_.empty()) {
                endNode();
            }

            if (documentOpened_) {
                processor_.onEndDocument();
                documentOpened_ = false;
            }

//            // read the type name
//            // TODO: Read document tag and use it for the first document node
//            YamlTag<Str> tag{};
//            processTypeTag(tag);
//            processor_.onBeginDocument(tag, YamlValueType::Map);
//            documentOpened_ = true;
            return true;
        }
        return false;
    }



    void processNodeValue(NodeR node) {

        auto valueType = checkFlow();

        switch (valueType) {
        using enum YamlValueType;
        case FlowSeq:
            processFlow();
            break;
        case FlowMap:
            //flowMapCount_++;
            processFlow();
            break;

        case Scalar:
            node.value = readValue(valueType);
            updateNode(node, Scalar);
            endNode();
            break;
        default: break;
        }
    }


    // Returns true if the flow ended
    auto checkFlow() {
        using enum YamlValueType;

        input_.skipSpaces();

        if (input_.eol())
            return Unknown;

        auto ch = input_.peek();

        if ('#' == ch) {
            // Process comment
            readLine();
            return Unknown;
        }


        if ('[' == ch) {
            //flowSeqCount_++;
            return FlowSeq;
        }

        if ('{' == ch) {
            //flowMapCount_++;
            return FlowMap;
        }

        return Scalar;
    }

    void processFlow() {
        using enum YamlValueType;

        StrView currentKey{};
        YamlTag<Str> tag{};

        auto ch = input_.read();
        auto valueType = Unknown;

        if ('[' == ch) {
            flowSeqCount_++;
            valueType = FlowSeq;
        }
        if ('{' == ch) {
            flowMapCount_++;
            valueType = FlowMap;
        }

        input_.skipSpacesEol();

        if (!nodeStack_.empty()) {
            if (!updateNode(nodeStack_.back(), valueType)) {
                beginNode(initNode(valueType, -1));
            }
        } else {
            beginNode(initNode(valueType, -1));
        }
        //auto addNode = node.valueType != valueType;

        //if (addNode) {
        //    node.valueType = valueType;
        //    beginNode(node);
        //}

        //flowStack_.push(valueType);

//        const auto pushValue = [&] {
//
//            // TODO: implement nested flows too
//            auto child = initNode(ValType::Scalar, -1);
//
//            if (FlowMap == valueType) {
//                child.key = readKey();
//
//                if (!input_.eof()) {
//                    if (isMappingValue()) {
//                        input_.skip();
//                        input_.skipSpacesEol();
//                    }
//                }
//            }
//
//            // child.valueType = YamlValue
//            child.value = readValue(valueType);
//            beginNode(child);
//            endNode();
//        };

        while (!input_.eof()) {
            auto vType = checkFlow();

            if (isValFlow(vType)) {
                processFlow();
                //continue;
            }


            // Expect value name for the object
            //input_.skipSpacesEol();
            ch = input_.peek();
            bool endFlowSeq{}, endFlowMap{};

            if (']' == ch && FlowSeq == valueType) {
                flowSeqCount_--;
                endFlowSeq = true;
            }

            if ('}' == ch && FlowMap == valueType) {
                flowMapCount_--;
                endFlowMap = true;
            }

            if (endFlowSeq || endFlowMap) {
                input_.skip();

                if (endFlowSeq) {
                    //flowSeqCount_--;
                    if (flowSeqCount_ > 0) {
                        input_.skipSpacesEol();
                    } else {
                        readLine();
                    }
                }

                if (endFlowMap) {
                    //flowMapCount_--;
                    if (flowMapCount_ > 0) {
                        input_.skipSpacesEol();
                    } else {
                        readLine();
                    }
                }

                //input_.skipSpaces();
                //input_.skipLine();
                //input_.skipSpacesEol();

                endNode();
                return;
            }

            if (',' == ch) {
                input_.skip();
                input_.skipSpacesEol();
                continue;
            }

            pushValue(valueType);
        }
    }  // processFlow

    void pushValue(YamlValueType valueType) {
        // TODO: implement nested flows too
        auto child = initNode(ValType::Scalar, -1);

        if (YamlValueType::FlowMap == valueType) {
            child.key = readKey();

            if (!input_.eof()) {
                if (isMappingValue()) {
                    input_.skip();
                    input_.skipSpacesEol();
                }
            }
        }

        // child.valueType = YamlValue
        child.value = readValue(valueType);
        beginNode(child);
        endNode();
    }

    void processTypeTag(YamlTag<Str>& tag) {
        input_.skipSpaces();
        if (!input_.eol()) {
            auto c = input_.peek();
            if (isComment(c)) {
                readLine();
            } else if ('!' == c) {
                input_.read();

                verify(!input_.eol(), "Unexpected EOL");

                c = input_.peek();

                if ('!' == c) {
                    input_.read();
                    tag.type = YamlTagType::BuiltIn;
                    // int, float, null, str, binary, set, map, omap
                } else {
                    tag.type = YamlTagType::Custom;
                }

                verify(!input_.eol(), "Missing tag name");

                tag.name = readName();
            }
        }

        input_.skipSpaces();
        if (!input_.eol() && isComment(input_.peek()))
            readLine();
    }

    StrView readName() {
        auto ptr = input_.getPtr();
        StrView::size_type count{};

        while (!input_.end()) {
            auto ch = input_.peek();
            if (!isNsChar(ch))
                break;
            ++count;
            input_.skip();
        }

        return StrView{ptr, count};
    }

    StrView readKey() {
        auto ptr = input_.getPtr();
        StrView::size_type count{};

        auto comment = false;

        while (!input_.eol()) {
            auto ch = input_.peek();
            if (':' == ch)
                break;
            if (isComment(ch)) { // '#'
                comment = true;
                break;
            }
            input_.read();
            ++count;
        }

        if (comment)
            readLine();

        return str_trim(StrView{ptr, count});
    }


    StrView readValue(ValType valueType) {
        auto isFlowSeq = ValType::FlowSeq == valueType;
        auto isFlowMap = ValType::FlowMap == valueType;
        auto isFlow = ValType::FlowSeq == valueType || ValType::FlowMap == valueType;

        bool comment{}, doubleQuoted{}, quoted{};
        StrView::size_type count{};

        if (isFlow) {
            input_.skipSpacesEol();
        } else {
            input_.skipSpaces();
        }

        if (!isFlow && input_.eol())
            return {};

        auto ch = input_.peek();

        if (isFlowSeq && ']' == ch)
            return {};
        if (isFlowMap && '}' == ch)
            return {};

        auto ptr = input_.getPtr();

        ch = input_.peek();

        if (isQuote(ch)) {
            quoted = true;
            doubleQuoted = isDoubleQuote(ch);
            input_.skip();
            ptr = input_.getPtr();
        }


        // TODO: implement better and test
        auto isFlowChar = ch == '[' || ch == '{';

        for (;;) {
            if (!isFlow && input_.eol())
                break;
            if (isFlowChar)
                break;

            ch = input_.peek();

            if (isFlow && ',' == ch)
                break;
            if (isFlowMap && '}' == ch)
                break;
            if (isFlowSeq && ']' == ch)
                break;

            if (!quoted) {
                if (isComment(ch)) { // '#'
                    comment = true;
                    break;
                }
            } else { // quoted string
                if (('\"' == ch && doubleQuoted) || ('\'' == ch && !doubleQuoted)) {
                    input_.skip();
                    input_.skipSpaces();
                    if (!input_.eol() && isComment(input_.peek()))
                        comment = true;
                    break;
                }

                // TODO: escapes for double quoted strings
            }

            input_.skip();
            ++count;
        }

        if (comment)
            readLine();

        // Strip trailing spaces
        while (count > 0) {
            ch = ptr[count - 1];
            if (!isSpace(ch) && !isEol(ch))
                break;

//            if (isFlow && isEol(ch))
//                break;
            --count;
        }

        return StrView{ptr, count};
    }


    bool processComment() {
        if (!isComment())
            return false;
        readLine();
        return true;
    }

    StrView readLine() {
        auto ptr = input_.getPtr();
        StrView::size_type count{};

        while (!input_.end() && !input_.eol()) {
            input_.get();
            ++count;
        }

        if (!input_.end())
            input_.skip(); // skip EOL character

        return StrView{ptr, count};
    }


    void skipIndent() {
        while (!input_.end() && isIndent(input_.peek()))
            input_.read();
    }

private:
    bool isSequenceEntry() { return isSequenceEntry(input_.peek()); }
    bool isMappingEntry() { return isMappingEntry(input_.peek()); }
    bool isMappingValue() { return isMappingValue(input_.peek()); }
    bool isComment() { return isComment(input_.peek()); }
    bool isDirective() { return isDirective(input_.peek()); }

    bool isEndDocument() {
        // TODO: implement properly checking for ...
        if (input_.peek() != '.')
            return false;
        return true;
    }

    static bool isValFlow(ValType valType) noexcept {
        return ValType::FlowSeq == valType || ValType::FlowMap == valType;
    }


    // Error handling
    void handleError(StrView message) {
        if (this->filename_.empty())
            this->filename_ = "::yaml";
        const auto&& s = fmt("%s:%d:%s", this->filename_, input_.line + 1, message);
        processor_.onError(s);
    }

    template <class... Args>
    void handleError(StrView text, Args&&... args) {
        handleError(fmt(text, std::forward<Args>(args)...));
    }

    void handleWarning(StrView message) noexcept {
        if (this->filename_.empty())
            this->filename_ = "::yaml";
        auto s = fmt("%s:%d:%s", this->filename_, input_.line + 1, message);
        processor_.onWarning(s);
    }

    template <class... Args>
    void handleWarning(StrView fmt, Args&&... args) noexcept {
        handleWarning(sformat(fmt, std::forward<Args>(args)...));
    }

    template <class Condition, class... Args>
    void verify(Condition cond, Args&&... args) {
        if (!cond)
            handleError(std::forward<Args>(args)...);
    }


public:
    //
    // Utility functions
    //
    constexpr static bool isSequenceEntry(Char c) noexcept { return '-' == c; }
    constexpr static bool isMappingEntry(Char c) noexcept { return '?' == c; }
    constexpr static bool isMappingValue(Char c) noexcept { return ':' == c; }

    constexpr static bool isNsChar(Char c) noexcept {
        return ('a' <= c && 'z' >= c) || ('A' <= c && 'Z' >= c);
    }
    constexpr static bool isComment(Char c) noexcept { return '#' == c; }
    constexpr static bool isDirective(Char c) noexcept { return '%' == c; }
    constexpr static bool isIndent(Char c) noexcept { return ' ' == c; }
    constexpr static bool isIgnored(Char c) noexcept { return '\r' == c; }
    constexpr static bool isQuote(Char c) noexcept { return '\"' == c || '\'' == c; }
    constexpr static bool isDoubleQuote(Char c) noexcept { return '\"' == c; }
    constexpr static bool isSingleQuote(Char c) noexcept { return '\'' == c; }
};


template <class Yaml, class Processor>
struct YamlParserImpl: public ParserBase {
    using YamlP = Yaml*;

    template <class I>
    void parse_main(ParseInput<I>& input_, YamlP yaml) {
        YamlParserImplBody body(input_, yaml, processor_);
        body.parse();
    }

private:
    Processor processor_;
};

} // namespace impl

} // namespace apl
