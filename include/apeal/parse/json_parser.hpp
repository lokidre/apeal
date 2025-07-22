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

namespace apl {

namespace impl {

template <class InputIt, class Json, class Processor>
struct JsonParserImplBody: public ParserBase {
    using JsonP = Json*;
    using Str = typename Json::Str;
    using View = StrView;
    using ValueType = JsonValueType;


    ParseInput<InputIt>& input_;
    Processor& processor_;


public:
    //explicit JsonParserImplBody(Input& in): input_{in}{}
    explicit JsonParserImplBody(ParseInput<InputIt>& in, JsonP json, Processor& proc): input_{in}, processor_{proc} {
        processor_.json = json;
    }

    bool parse() {
        input_.ignoreEol = true;

//        input_.enableLineComments = true;
//        input_.lineCommentChar = '/';
//        input_.lineCommentChar2 = '/';

        return processElement();
    }

private:
    bool processElement() {
        View key{}, value{};

        auto ch = input_.peekAfterSpaces();

        //  // Check for the line comments
        if ('/' == ch) {
            input_.read();
            for(;;) {
                if ('/' == input_.peek()) {
                    readLine();
                    ch = input_.peekAfterSpaces();
                    if ('/' != ch)
                        break;
                    input_.read();
                }
            }
        }


        if ('\"' == ch) {
            key = readQuotedString();
            ch = input_.peekAfterSpaces();
            if (':' == ch) {
                input_.skip();
            } else {
                processor_.pushValue({}, key, ValueType::String);
                return true;
            }
        }

        // next should be either array or object
        ch = input_.peekAfterSpaces();

        switch (ch) {
        case '[':
            input_.skip();
            processor_.pushArray(key);
            for (;;) {
                ch = input_.peekAfterSpaces();

                if (']' == ch) {
                    input_.skip();
                    processor_.closeObject();
                    break;
                }

                processElement();

                ch = input_.readAfterSpaces();

                if (']' == ch) {
                    processor_.closeObject();
                    break;
                }
                if (ch == ',')
                    continue;

                throwError("Missing Separator");
            }
            break;

        case '{':
            input_.read();
            processor_.pushObject(key);
            for (;;) {
                ch = input_.peekAfterSpaces();

                if ('}' == ch) {
                    input_.skip();
                    processor_.closeObject();
                    break;
                }

                processElement();

                ch = input_.readAfterSpaces();

                if ('}' == ch) {
                    processor_.closeObject();
                    break;
                }

                if (',' == ch)
                    continue;

                throwError("Separator missing");
            }
            break;

        case '\"':
            value = readQuotedString();
            processor_.pushValue(key, value, ValueType::String);
            break;

        case 't':
            value = expect("true");
            processor_.pushValue(key, value, ValueType::Boolean);
            break;

        case 'f':
            value = expect("false");
            processor_.pushValue(key, value, ValueType::Boolean);
            break;

        case 'n':
            value = expect("null");
            processor_.pushValue(key, value, ValueType::Null);
            break;

        default:
            if (isDigit(ch) || ch == '-') {
                ValueType valueType{};
                value = readNumber(&valueType);
                processor_.pushValue(key, value, valueType);
                break;
            }

            throwError("Invalid value: %s", ch);
        }

        return true;
    }


public: // various utilities
    auto readQuotedString() {
        verify(input_.read() == '\"', "Quoted string must start with double quotes");

        auto ptr = input_.getPtr();
        std::size_t count{};

        for (;;) {
            verify(!input_.end(), "Unterminated quoted string");
            auto ch = input_.read();
            if (ch == '\"')
                break;
            ++count;
        }

        return StrView{ptr, count};
    }

    auto expect(Char ch) {
        input_.skipSpaces();
        auto begin = input_.getPtr();
        if (input_.end() || input_.read() != ch)
            throwError("Expected: '%s'", ch);
        return StrView{begin, 1};
    }


    auto expect(StrView test) {
        input_.skipSpaces();
        auto begin = input_.getPtr();
        std::size_t count{};
        for (auto ch: test) {
            verify(input_.read() == ch, "Invalid char: %c", ch);
            ++count;
        }
        return StrView{begin, count};
    }


    auto readNumber(ValueType* valueType) {
        auto begin = input_.getPtr();
        std::size_t count{};

        auto isInt = true;
        for (;;) {
            verify(!input_.end(), "Unexpected EOF");
            auto ch = input_.peek();
            if (!count && ch == '-') {
                input_.read();
                ++count;
            } else if (isDigit(ch)) {
                input_.read();
                ++count;
            } else if (ch == '.' && isInt) {
                isInt = false;
                input_.read();
                ++count;
            } else {
                verify(count, "Number is missing");
                break;
            }
        }

        *valueType = isInt ? ValueType::Integer : ValueType::Real;
        return StrView{begin, count};
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

};

template <class Json, class Processor>
struct JsonParserImpl: public ParserBase {
    using JsonP = Json*;

    template <class InputIt>
    void parse_main(ParseInput<InputIt>& input, JsonP json) {
        JsonParserImplBody body(input, json, processor_);
        body.parse();
    }

private:
    Processor processor_;
};


}  // namespace impl
}  // namespace apl
