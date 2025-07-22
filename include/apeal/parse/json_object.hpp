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

namespace impl {

enum class JsonNodeType {
    Object,
    Array,
    Scalar,
};

enum class JsonValueType {
    Null,
    String,
    Integer,
    Real,
    Boolean,
};


template <class S>
struct JsonNode {
    using Str = S;
    using Node = JsonNode;
    using Type = JsonNodeType;
    using ValueType = JsonValueType;

    using NodeCont = std::vector<Node>;
    using Iterator = typename NodeCont::iterator;

    Str key{}, value{};
    NodeCont nodes;
    Type type{};
    ValueType valueType{};

    void clear() noexcept {
        JsonNode temp;
        std::swap(*this, temp);
    }

    [[nodiscard]] auto& front() const noexcept {  return nodes;  }

    [[nodiscard]] auto begin() const noexcept { return nodes.begin(); }
    [[nodiscard]] auto end() const noexcept { return nodes.end(); }

    [[nodiscard]] bool isScalar() const noexcept { return Type::Scalar == type; }

    [[nodiscard]] String decode() const {
        String unescaped;
        unescaped.reserve(value);

        bool esc{};
        for (auto ch : value) {
            if (esc) {
                switch (ch) {
                case 'n': unescaped += '\n'; break;
                default: unescaped += ch; break;
                }
                esc = false;
                continue;
            }

            if ('\\' == ch) {
                esc = true;
                continue;
            }
            unescaped += ch;
        }

        return unescaped;
    }

    auto get(bool* val) const {
        parse(value, *val);
    }

    auto get(char* val) const {
        *val = value.empty() ? 0 : value[0];
    }

    auto get(unsigned char* val) const {
        *val = value.empty() ? 0 : value[0];
    }


    auto get(std::integral auto* val) const {
        parse(value, val);
        // parse_int_radix(value, *val, 10);
    }

    // auto get(std::unsigned_integral auto* val) const {
    //     parse(value, val);
    //     // parse_uint_radix(value, *val, 10);
    // }

    auto get(std::floating_point auto* val) const {
        parse(value, val);
    }

    auto get(String* val) const { *val = value; }


    int getInt() const {
        int val{};
        get(&val);
        return val;
    }

    auto getKey(std::integral auto* k) const {
        parse_int_radix(key, *k, 10);
    }

    auto getKey(String* k) const { *k = key; }


};

}  // namespace impl

}  // namespace apl
