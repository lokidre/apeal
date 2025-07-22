/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <type_traits>
#include <concepts>

namespace apl {

namespace impl {

template <class S>
struct YamlBuilderBody {
    using Str = S;
    using Char = typename Str::value_type;

    YamlBuilderBody(Str& istr) noexcept: str_{&istr} { str_->reserve(1000); }

    YamlBuilderBody() noexcept: str_{&yamlData_} { str_->reserve(1000); }

    ~YamlBuilderBody() noexcept {
        try {
            end();
        } catch (...) {
        }
    }

    const auto& str() noexcept {
        end();
        return *str_;
    }

    void begin() {
        str_->clear();
    }

    void end() {
        if (docOpen) {
            out("...", nl_);
            docOpen = false;
        }
    }

    void nl() { out(nl_); }

    void directive(StrView name, StrView value) {
        write('%');
        write(name);
        write(' ');
        write(value);
        nl();
    }

    void obj(StrView name) { beginObject(name); }
    void obj(std::integral auto name) { beginObject(std::to_string(name)); }
    void object(StrView name) { beginObject(name); }
    void object(std::integral auto name) { beginObject(std::to_string(name)); }
    void array(StrView name) { beginObject(name); }
    void array(std::integral auto name) { beginObject(std::to_string(name)); }

    void pop() { endObject(); }
    void pop(Index count) {
        for (Index i{}; i < count; ++i)
            pop();
    }

    void map() { beginMap(); }
    void seq() { beginMap(); }

    template <class T>
    void seq(StrView name, T value) {
        writeIndent();
        write(seqPrefix_);
        outName(name);
        out(' ');
        outStringValue(value);
        nl();

        ++indentLevel;
        updateIndent();
    }


    template <class T, std::enable_if_t<std::is_same_v<T, StrView>, int> = 0>
    void item(T value) {
        writeIndent();
        write(seqPrefix_);
        outStringValue(value);
    }

    template <class T> //, std::enable_if_t<std::is_same_v<T, StrView>, int> = 0>
    void item(StrView name, T value) {
        writeIndent();
        write(seqPrefix_);
        outName(name);
        out(' ');
        outStringValue(value);
    }


    template <class Vec>
    void flowSeqVecBody(StrView name, const Vec& val) {
        outName(name);
        out(' ');

        // TODO: test and fix later
        out('[');
        auto first = true;
        for (auto ival: val) {
            if (first) {
                first = false;
            } else {
                out(',');
            }

            outValue(ival);
            // outStringValue(val);
        }
        out(']');
        nl();
    }

    template <class Vec>
    void flowSeqVec(StrView name, const Vec& val) {
        writeIndent();
        flowSeqVecBody(name, val);
    }

    template <class... T>
    [[deprecated("use seqBlock")]]
    void seqVec(StrView name, T... v1) {
        writeIndent();
        write(seqPrefix_);
        flowSeqVecBody(name, std::vector{v1...});
    }

    // Sequence
    template <class VecT>
    void seqBlockVec(StrView name, const VecT& vec) {
        writeIndent();
        write(seqPrefix_);
        flowSeqVecBody(name, vec);
    }

    template <class... T>
    void seqBlock(StrView name, T... v1) {
        seqBlockVec(name, std::vector{v1...});
    }


    template <class... T>
    void flowSeq(StrView name, T... v1) {
        flowSeqVec(name, std::vector{v1...});
    }
    template <class... T>
    void flowSeq(int name, T... v1) {
        flowSeqVec(std::to_string(name), std::vector{v1...});
    }
    template <class... T>
    void flowSeq(float name, T... v1) {
        flowSeqVec(std::to_string(name), std::vector{v1...});
    }
    template <class... T>
    void flowSeq(double name, T... v1) {
        flowSeqVec(std::to_string(name), std::vector{v1...});
    }
    template <class... T>
    void flowSeq(unsigned name, T... v1) {
        flowSeqVec(std::to_string(name), std::vector{v1...});
    }


    //template <class T, std::enable_if_t<std::is_same_v<T, char>, int> = 0>
    void field(StrView name, char val) {
        outStringField(name, fmt("%c", val));
    }

    //template <class T, std::enable_if_t<std::is_same_v<T, unsigned char>, int> = 0>
    void field(StrView name, unsigned char val) {
        outStringField(name, fmt("%c", val));
    }

    //template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    void field(StrView name, std::integral auto val) {
        //outField(name, fmt("%d", val));
        outField(name, std::to_string(val));
    }


    //template <class T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    void field(StrView name, std::floating_point auto val) {
        outField(name, fmt("%g", val));
    }


    template <class T, std::enable_if_t<std::is_same_v<T, StrView>, int> = 0>
    void field(StrView name, T val) {
        outStringField(name, val);
    }



    template <class T, std::enable_if_t<std::is_same_v<T, StrView>, int> = 0>
    void outValue(T val) {
        outStringValue(val);
    }

    template <class T, std::enable_if_t<std::is_same_v<T, String>, int> = 0>
    void outValue(T val) {
        outStringValue(val);
    }


    // TODO: move to private and implement for all types
    template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    void outValue(T val) {
        //outStringValue(fmt("%d", val));
        outStringValue(std::to_string(val));
    }

    template <class T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    void outValue(T val) {
        outStringValue(fmt("%g", val));
    }


    template <class... Args>
    void field(StrView name, StrView f, Args&&... args) {
        outStringField(name, fmt(f, std::forward<Args>(args)...));
    }

    template <class... Args>
    void field(int name, StrView f, Args&&... args) {
        outStringField(std::to_string(name), fmt(f, std::forward<Args>(args)...));
    }

    template <class... Args>
    void field(float name, StrView f, Args&&... args) {
        outStringField(std::to_string(name), fmt(f, std::forward<Args>(args)...));
    }

    template <class... Args>
    void field(double name, StrView f, Args&&... args) {
        outStringField(std::to_string(name), fmt(f, std::forward<Args>(args)...));
    }

    template <class... Args>
    void field(unsigned name, StrView f, Args&&... args) {
        outStringField(std::to_string(name), fmt(f, std::forward<Args>(args)...));
    }

    template <class T>
    void fieldIntegral(std::integral auto name, T value) {
        if constexpr (std::same_as<decltype(value), std::string>) {
            outStringField(std::to_string(name), value);
        } else {
            outStringField(std::to_string(name), std::to_string(value));
        }
    }

    template <class T>
    void fieldIntegral(StrView name, T value) {
        outStringField(name, std::to_string(value));
    }


    // template <class T>
    // void operator()(StrView name, T val){  field(name, val);  }

    // template <class T, std::enable_if_t<std::is_same_v<T, StrView>, int> = 0>
    // void operator()(StrView name, T val={}) { outStringField(name, val); }

    // template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    // void operator()(StrView name, T val) { outField(name, fmt("%d", val)); }

    // template <class T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    // void operator()(StrView name, T val) { outField(name, fmt("%f", val)); }


    // template <class ...Args>
    // void operator()(StrView name, StrView f, Args&&... args) {
    //    outStringField(name, fmt(f, std::forward<Args>(args)...));
    //}

    template <class... Args>
    void operator()(StrView key, Args&&... args) {
        field(key, std::forward<Args>(args)...);
    }


    void comment(StrView text) { out('#', text, nl_); }

    void doc() {
        if (docOpen) {
            out("...", nl_, nl_);
        }

        out("---", nl_);
        docOpen = true;
    }

    int indentSize{2};

private:
    Str* str_;
    int indentLevel{};
    Str yamlData_;
    String indentData;
    bool docOpen{};
    StrView seqPrefix_{"- "};
    Char nl_{'\n'};

    void writeIndent() { write(indentData); }
    void updateIndent() noexcept { indentData.assign(indentSize * indentLevel, ' '); }

    void beginNode(StrView name, bool seq) {
        writeIndent();
        if (seq)
            write(seqPrefix_);
        str_->append(name);
    }

    void write(StrView text) { str_->append(text); }

    void write(Char ch) { str_->append(1, ch); }

    template <class... Args>
    void out(Args... args) {
        (write(args), ...);
    }

    void outName(StrView name) { out(name, ':'); }

    void outField(StrView name, StrView value) {
        writeIndent();
        outName(name);
        out(' ');
        out(value);
        nl();
    }

    void outStringField(StrView name, StrView value) {
        writeIndent();
        outName(name);
        out(' ');
        outStringValue(value);
        nl();
    }

    void beginObject(StrView name) {
        writeIndent();
        outName(name);
        nl();
        ++indentLevel;
        updateIndent();
    }

    void beginMap() {
        writeIndent();
        write('-');
        nl();
        ++indentLevel;
        updateIndent();
    }

    void endObject() {
        --indentLevel;
        updateIndent();
    }

    void outStringValue(StrView value) {
        if (value.empty())
            return;
        auto quotes = false;
        auto multiline = false;

        auto fc = value.front();
        auto bc = value.back();

        // analyze the string
        for (auto c: value) {
            if ('*' == c || '?' == c)
                quotes = true;
            if ('\n' == c)
                multiline = true;
            if (quotes && multiline) // no need if everything is set
                break;
        }


        if (!quotes && (isSpace(fc) || isSpace(bc)))
            quotes = true;

        // if (!quotes && isDigit(fc)) {
        //     bool notnumber{};

        //    // check if the string can be interpreted as a number
        //    for (auto&& c : value) {
        //        if (!isDigit(c) && '.' != c) {
        //            notnumber = true;
        //            break;
        //        }
        //    }

        //    if (notnumber)
        //        quotes = true;
        //}

        if (!quotes && ('{' == fc || '[' == fc))
            quotes = true;

        if (quotes)
            write('\"');

        write(value);

        if (quotes)
            write('\"');
    }

    bool isSpace(Char c) const noexcept { return c == ' ' || c == '\t' || '\r' == c; }
    bool isDigit(Char c) const noexcept { return '0' <= c && '9' >= c; }
};


} // namespace impl


} // namespace apl
