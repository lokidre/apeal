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
    
//
// Json Builder - helps building json documents
//
struct JsonBuilderProcessor: NonCopyable {
    using Str = String;
    using Ch = Str::value_type;

    explicit JsonBuilderProcessor(Str& s) noexcept : str_{s} {}

    void write(StrView s) noexcept { str_ += s; }

    void write(Ch s) noexcept { str_ += s; }

    Str& str_;

    const auto& str() const { return str_; }
};


class JsonBuilder: NonCopyable {
public:
    using Str = String;

    // types
    // using string_type = String;
    using ErrorPolicy = AErrorPolicy;

    using Processor = JsonBuilderProcessor;
    using StringProcessor = JsonBuilderProcessor;

    template <class Container>
    using ValueFunctor =
        typename std::function<void(typename Container::value_type, String&, bool&)>;


public: // options
    bool generateNewlines{true};
    bool generateIndents{true};
    bool generateIndentSpaces{false};
    Index generateIndentSpaceCount{4};
    bool generateValueSpace{true};
    bool escapeSlash{false}; // Escape forward slashes '/'.

private:
    Str outputStr_;
    JsonBuilderProcessor defaultProcessor_{outputStr_};
    Processor& json_;
    ErrorPolicy error_;

    Str currentIndentString_;
    Index currentIndentLength_{};

    struct Token {
        bool object;
        bool open;
    };

    using tag_stack_type = std::stack<Token>;
    tag_stack_type tokens_;


    void ctor() {
        currentIndentString_.clear();
        currentIndentLength_ = 0;

        while (!tokens_.empty())
            tokens_.pop();

        json_.str_.clear();
    }

public:
    explicit JsonBuilder() : json_{defaultProcessor_} { ctor(); }

    ~JsonBuilder() noexcept { end(); }


    void begin() {
        ctor();

        // json file is always an object
        tokens_.push({true, false});

        json_.write('{');
    }

    void end() {
        // close the JSON
        while (!tokens_.empty())
            close();
        nl();
    }

    const auto& str() const {
        //end();
        return json_.str();
    }


    void map(StrView key = {}) { object(key);    }
    void obj(StrView key = {}) { object(key);  }
    void obj(std::integral auto key) {   object(key);   }


    void object(StrView key = {}) {
        prepval();

        tokens_.push({true, false});

        writeKey(key);
        optSpace(key);
        json_.write('{');
    }

    void object(std::integral auto key) {
        prepval();

        tokens_.push({true, false});

        writeKey(std::to_string(key));
        optSpace(std::to_string(key));
        json_.write('{');
    }


    void array(StrView key = {}) {
        prepval();

        tokens_.push({false, false});

        writeKey(key);
        optSpace(key);
        json_.write('[');
    }

    template <class Cont>
    void array(StrView key, const Cont& v, ValueFunctor<Cont> func) {
        array(key);
        arrayValues(v, func);
        close(false);
    }

    template <class Container>
    void arrayInt(StrView key, const Container& v) {
        array(key);
        intValues(v);
        close(false);
    }

    template <class ContainerT>
    void flowSeq(StrView key, const ContainerT& data) {
        array(key);

        for (auto& val: data) {
            checklist();
            writeValue(val);
        }

        //intValues(v);
        close(false);
    }


    template <class ContainerT>
    void flowSeq(std::integral auto key, const ContainerT& v) {
        array(std::to_string(key));
        intValues(v);
        close(false);
    }

    template <class Container>
    void flowSeq(std::floating_point auto key, const Container& v) {
        array(fmt("%g", key));
        intValues(v);
        close(false);
    }


    template <class KeyT, class ContainerT>
    void flowSeqVec(KeyT key, const ContainerT& data) {
        flowSeq(key, data);
    }




    // Just writes the csv array without processing it
    void arrayRaw(StrView key, StrView csv) {
        array(key);
        json_.write(csv);
        close(false);
    }


    void close(bool useNL = true) {
        if (tokens_.empty())
            return;

        auto& top = tokens_.top();

        if (useNL) {
            nl();
            indent(-1);
        }

        json_.write(top.object ? '}' : ']');

        tokens_.pop();
    }

    void pop(bool useNL = true) {
        if (tokens_.empty())
            return;

        auto& top = tokens_.top();

        if (useNL) {
            nl();
            indent(-1);
        }

        json_.write(top.object ? '}' : ']');

        tokens_.pop();
    }


public: // writing values
    //template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    void value(StrView key, std::integral auto value) {
        prepval();
        writeKey(key);
        optSpace(key);
        json_.write(std::to_string(value));
    }

    //template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    void field(StrView key, char value) {
        prepval();
        writeKey(key);
        optSpace(key);
        writeStringValue(fmt("%c", value));
    }

    void field(StrView key, unsigned char value) {
        prepval();
        writeKey(key);
        optSpace(key);
        writeStringValue(fmt("%c", value));
    }


    void field(StrView key, std::integral auto value) {
        prepval();
        writeKey(key);
        optSpace(key);
        json_.write(std::to_string(value));
    }

    //template <class T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    void value(StrView key, std::floating_point auto val) {
        prepval();
        writeKey(key);
        optSpace(key);
        json_.write(fmt("%g", val));
    }
    //template <class T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    void field(StrView key, std::floating_point auto val) {
        prepval();
        writeKey(key);
        optSpace(key);
        //json_.write(std::to_string(val));
        json_.write(fmt("%g", val));
    }

    void field(StrView key, StrView value) {
        prepval();
        writeKey(key);
        optSpace(key);
        writeStringValue(value);
    }


    // Just writes the string without processing it
    void rawValue(StrView key, StrView value) {
        prepval();
        writeKey(key);
        optSpace(key);
        json_.write(value);
    }


    void charValue(StrView key, Char value) {
        prepval();
        writeKey(key);
        optSpace(key);
        writeStringValue(StrView{&value, 1});
    }


    // void value(StrView name, uint64_t value)
    //{
    //    prepval();
    //    writeName(name);
    //    json_.write(std::to_string(value));
    //}


    template <class Container>
    void intValues(const Container& data) {
        //for (auto& val : values) {
        //    checklist();
        //    json_.write(std::to_string(val));
        //}


        bool quoted{};
        //String s;
        for (auto& val: data) {
            checklist();
            writeValue(val);
            //if (quoted)
            //    writeStringValue(s);
            //else
            //    json_.write(s);
        }
    }

    template <class Container>
    void arrayValues(const Container& data, const ValueFunctor<Container>& func) {
        bool quoted{};
        String s;
        for (auto& val : data) {
            prepval();
            func(val, s, quoted);
            if (quoted)
                writeStringValue(s);
            else
                json_.write(s);
        }
    }

    void arrayValue(int value) {
        prepval();
        json_.write(std::to_string(value));
    }

    void arrayValue(StrView value) {
        prepval();
        writeStringValue(value);
    }

    void arrayRawValue(StrView value) {
        prepval();
        json_.write(value);
    }

    void value(StrView name, StrView value) {
        prepval();
        writeKey(name);
        optSpace(name);
        writeStringValue(value);
    }

    void value(int name, StrView value) {
        prepval();
        writeKey(std::to_string(name));
        optSpace(std::to_string(name));
        writeStringValue(value);
    }
    void value(float name, StrView value) {
        prepval();
        writeKey(std::to_string(name));
        optSpace(std::to_string(name));
        writeStringValue(value);
    }
    void value(double name, StrView value) {
        prepval();
        writeKey(std::to_string(name));
        optSpace(std::to_string(name));
        writeStringValue(value);
    }
    void value(unsigned name, StrView value) {
        prepval();
        writeKey(std::to_string(name));
        optSpace(std::to_string(name));
        writeStringValue(value);
    }

    void null(StrView name) {
        prepval();
        writeKey(name);
        optSpace(name);
        json_.write("null");
    }

    void boolean(StrView name, bool value) {
        prepval();
        writeKey(name);
        optSpace(name);
        json_.write(value ? "true" : "false");
    }


    void writeValue(int val) {
        json_.write(std::to_string(val));
    }

    void writeValue(StrView val) {
        writeStringValue(val);
    }

public:
    static auto escape(StrView stringValue) noexcept {
        String s;
        s.reserve(stringValue.size());

        for (auto c: stringValue) {
            switch (c) {
            case '\"': s += "\\\""; break;
            case '\\': s += "\\\\"; break;
            // case '/':
            //     if (escapeSlash)
            //         s += "\\/";
            //     else
            //         s += c;
            //     break;
            case '\b': s += "\\b"; break;
            case '\f': s += "\\f"; break;
            case '\n': s += "\\n"; break;
            case '\r': s += "\\r"; break;
            case '\t': s += "\\t"; break;
            default: s += c; break;
            }
        }

        return s;
    }


private:
    void prepval() {
        checklist();
        nl();
        indent();
    }

    void nl() {
        if (generateNewlines)
            json_.write('\n');
    }

    void optSpace(StrView name) {
        if (!name.empty() && generateValueSpace)
            json_.write(' ');
    }

    void indent(Index offset = 0) {
        if (!generateIndents)
            return;

        auto tokensCount = std::ssize(tokens_) + offset;
        if (currentIndentLength_ != tokensCount) {
            currentIndentString_.clear();
            currentIndentLength_ = tokensCount;
            for (Index i = 0; i < currentIndentLength_; ++i) {
                if (generateIndentSpaces) {
                    for (Index space = 0; space < generateIndentSpaceCount; ++space)
                        currentIndentString_ += ' ';
                } else {
                    currentIndentString_ += '\t';
                }
            }
        }

        json_.write(currentIndentString_);
    }


    void checklist() {
        if (tokens_.empty())
            return;

        if (tokens_.top().open) {
            json_.write(',');
        } else {
            tokens_.top().open = true;
        }
    }


    void writeKey(StrView name) {
        if (!name.empty()) {
            json_.write('\"');
            writeEscaped(name);
            json_.write('\"');
            json_.write(':');
        }
    }

    void writeStringValue(StrView stringValue) {
        json_.write('\"');
        writeEscaped(stringValue);
        json_.write('\"');
    }

    void writeEscaped(StrView stringValue) {
        json_.write(escape(stringValue));
    }
};

}  // namespace apl
