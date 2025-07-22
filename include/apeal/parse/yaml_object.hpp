/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <concepts>


namespace apl {

enum class YamlKeyType { Scalar, Complex };
enum class YamlValueType { Unknown, Scalar, Seq, Map, FlowSeq, FlowMap };


namespace impl {


enum class YamlSeqType { Collection, Sequence };
enum class YamlChildType { Regular, FlowSeq, FlowMap };
enum class YamlTagType { None, BuiltIn, Custom };

template <class S>
struct YamlTag {
    using TagType = YamlTagType;
    using Str = S;

    TagType type{TagType::None};
    Str name;

    bool isNone() const noexcept { return TagType::None == type; }
    bool isBuiltIn() const noexcept { return TagType::BuiltIn == type; }
    bool isCustom() const noexcept { return TagType::Custom == type; }
    bool isCustom(StrView n) const noexcept { return isCustom() && n == name; }
};

template <class S>
struct YamlNode {
    using Str = S;
    using Node = YamlNode;
    using NodeContainer = std::vector<Node>;
    using Iterator = typename NodeContainer::iterator;
    using KeyType = YamlKeyType;
    using ValType = YamlValueType;
    using SeqType = YamlSeqType;
    using IdxType = int;

    KeyType keyType{};
    ValType valueType{};
    SeqType seqType{};

    YamlTag<S> tag;

    Str key;
    Str value;

    // Parsing runtime
    IdxType indent{};
    IdxType index{};
    IdxType line{}, column{};

    NodeContainer nodes;


    //
    // Checking the value
    //
    auto isNull() const noexcept {
        return valueType == YamlValueType::Scalar && (value.empty() || value == "null");
    }

    auto isKeyScalar() const noexcept { return KeyType::Scalar == keyType; }
    auto isKeyComplex() const noexcept { return KeyType::Complex == keyType; }


    auto isScalar() const noexcept { return YamlValueType::Scalar == valueType; }

    auto isFlowSeq() const noexcept { return YamlValueType::FlowSeq == valueType; }
    auto isFlowMap() const noexcept { return YamlValueType::FlowMap == valueType; }
    auto isSeq() const noexcept {
        return ValType::Seq == valueType || ValType::FlowSeq == valueType;
    }
    auto isMap() const noexcept {
        return ValType::Map == valueType || ValType::FlowMap == valueType;
    }
    auto isFlow() const noexcept {
        return ValType::FlowSeq == valueType || ValType::FlowMap == valueType;
    }
    auto isCollection() const noexcept { return isMap() || isSeq(); }


    //
    // Reading the value
    //

    auto& getValue() const noexcept { return value; }

    auto& getString() const noexcept { return value; }
    auto getInt() const { return parseInt(value); }
    auto getInt(int def) const noexcept { return parseInt(value, def); }
    auto getBool() const { return parseBool(value); }
    auto getBool(bool def) const noexcept { return parseBool(value, def); }

    template <class Real>
    auto getReal() const {
        return parseReal<Real>(value);
    }

    auto get(bool* val) const {
        *val = getBool();
    }


    auto get(char *val) const {
        *val = value.empty() ? 0 : value[0];
    }

    auto get(unsigned char *val) const {
        *val = value.empty() ? 0 : value[0];
    }

    auto get(std::integral auto* val) const {
        parse_int_radix(value, *val, 10);
    }

    auto get(std::unsigned_integral auto* val) const {
        parse_uint_radix(value, *val, 10);
    }


    auto get(std::floating_point auto* val) const {
        parse(value, val);
    }

    auto get(String* val) const { *val = value; }

    auto getKey(std::integral auto* k) const {
        parse_int_radix(key, *k, 10);
    }

    auto getKey(String* k) const { *k = key; }

    auto& getScalar() const {
        verify(isScalar(), "Yaml::getScalar() called on non-scalar value");
        return value;
    }

    auto& operator()() const { return getValue(); }


    //
    // Selecting child value
    //
    template <class T>
    auto getProp(StrView propKey, T* val) const {
        for (auto& node: nodes) {
            if (node.key == propKey) {
                node.get(val);
                return true;
            }
        }

        return false;
    }

    //
    // Iterating over child nodes
    //
    auto begin() const noexcept { return nodes.begin(); }
    auto end() const noexcept { return nodes.end(); }
};

template <class S>
using YamlDocument = YamlNode<S>;


template <class S>
struct Yaml {

    using Node = YamlNode<S>;
    using Str = S; // typename Node::Str;
    using NodeContainer = typename Node::NodeContainer;
    using Document = YamlDocument<Str>;
    // using DocumentContainer = typename YamlDocument<Str>::DocumentContainer;
    using DocumentContainer = typename YamlDocument<Str>::NodeContainer;
    using Tag = YamlTag<Str>;

    int versionHi{}, versionLo{};

    // NodeCont documents;
    DocumentContainer documents;
    std::vector<String> warnings;

    auto begin() const noexcept { return documents.begin(); }
    auto end() const noexcept { return documents.end(); }
    auto size() const noexcept { return documents.size(); }
    auto ssize() const noexcept { return std::ssize(documents); }
    auto empty() const noexcept { return documents.empty(); }
    auto& front() const {
        verify(!documents.empty(), "Yaml is empty");
        return documents.front();
    }

    void clear() noexcept { *this = {}; }
};


} // namespace impl

} // namespace apl
