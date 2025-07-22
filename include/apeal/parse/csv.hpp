/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#if 0

CSV Parser
---

Due to the lack of somewhat accepted standard there are several options of
escaping special characters in CSV file. The parser is made to be as generic
as possible, but still some setup might be necessary. The builder tries to
build as smaller file as possible, but in certain cases some tweaking is
also necessary.

#endif

#include "parse_base.hpp"
#include "../str/trim.hpp"

namespace apl {

using Csv = std::vector<std::vector<std::string>>;
using CsvView = std::vector<std::vector<std::string_view>>;

struct CsvSetup {
    using Str = String;
    using Ch = typename Str::value_type;

protected:
    // Default setup according to RFC 4180
    // http://tools.ietf.org/html/rfc4180
    Ch comma_char_{','};
    Ch quote_char_{'\"'};
    Ch escape_char_{'\"'};
    Ch eol_char_{'\n'};
    bool generate_quoted_{true};
    bool autotrim_{false};

public:  // setup
    void set_field_separator(Ch c) noexcept { comma_char_ = c; }
    void set_record_separator(Ch c) noexcept { eol_char_ = c; }
    void set_escape_character(Ch c) noexcept { escape_char_ = c; }
    void set_quote_character(Ch c) noexcept { quote_char_ = c; }
    void set_autotrim(bool b) noexcept { autotrim_ = b; }
};


struct CsvParserImpl: public CsvSetup, public ParserBase {

    template <class Input, class data_T>
    bool preprocess_basic(Input& input, data_T& data)
    {
        data.clear();

        // calculate number of end-of-lines
        Index eolCount{};
        while (!input.eof()) {
            if (input.read() == eol_char_)
                ++eolCount;
        }

        if (eolCount)
            data.reserve(eolCount);

        return true;
    }

    template <class Input, class Csv>
    bool parse_main(Input& input, Csv *csv)
    {
        for (;;) {  // for all rows

            // skip empty lines
            while (!input.end() && (input.isSpace() || input.eol()))
                input.read();

            if (input.end())
                break;


            // read the row
            auto& row = csv->emplace_back();

            auto ch = eol_char_;

            for (;;) {  // for all fields

                // skip empty spaces
                while (!input.end() && input.isSpace())
                    input.read();

                if (input.end() || input.eol()) {
                    row.emplace_back();
                    break;
                }

                // two cases - quoted and unquoted
                if (quote_char_ && input.peek() == quote_char_) {
                    // search for quote
                    while (!input.end() && input.read() != quote_char_)
                        ;
                    if (input.end())
                        break;

                    auto& field = row.emplace_back();

                    // read until quote
                    while (!input.end()) {
                        ch = input.read();
                        if (ch == escape_char_) {
                            if (input.end() || input.eol())
                                break;
                            ch = input.read();
                            if (escape_char_ == quote_char_ && ch != quote_char_) {
                                //input.unread(ch) ;
                                break;
                            }
                        }
                        field.push_back(ch);
                    }

                    // Skip everything until either new record or new field
                    for (; !input.end() && ch != comma_char_ && ch != eol_char_; ch = input.read())
                        ;

                    if (autotrim_)
                        field = str_trim(field);

                } else {  // not using quotes
                    // search for non space
                    while (!input.end() && input.isSpace())
                        input.read();
                    if (input.end())
                        break;

                    auto& field = row.emplace_back();

                    while (!input.end()) {
                        ch = input.read();
                        if (ch == escape_char_) {
                            ch = input.read();
                            field.push_back(ch);
                            continue;
                        }
                        if (ch == comma_char_ || ch == eol_char_)
                            break;
                        field.push_back(ch);
                    }

                    // Here we trim the field
                    if (autotrim_)
                        field = str_trim(field);
                }

                if (ch == eol_char_ || input.end())
                    break;

            } // for all fields
        }  // for all rows

        return true;
    }

};

using CsvParser = ParserTemplate<CsvParserImpl, Csv>;

class CsvBuilder: public CsvSetup {
public:
    using string_type = String;
    using char_type = string_type::value_type;

    using Str = String;
    using View = StrView;
    using Ch = typename Str::value_type;

public:
    CsvBuilder() { ctor(); }
    CsvBuilder(Str& csv) { begin(csv); }

    ~CsvBuilder() { end(); }

public:
    void field(View s = {})
    {
        if (currentColumn_)
            *csv_ += comma_char_;

        if (generate_quoted_)
            *csv_ += quote_char_;

        for (auto c = s.begin(), c_end = s.end(); c != c_end; ++c) {
            if (*c == quote_char_)
                *csv_ += quote_char_;
            *csv_ += *c;
        }

        if (generate_quoted_)
            *csv_ += quote_char_;

        ++currentColumn_;
    }

    template <class V, std::enable_if_t<std::is_integral_v<V>, int> = 0>
    void val(V val) noexcept {
        field(std::to_string(val));
    }

    template <class V, std::enable_if_t<std::is_floating_point_v<V>, int> = 0>
    void val(V val) noexcept {
        field(std::to_string(val));
    }


    void header(View s)
    {
        field(s);
    }

    void endl()  // end record
    {
        if (currentColumn_ == 0)
            return;

        if (!columnsKnown_) {
            columnsKnown_ = true;
            totalColumns_ = currentColumn_;
        }

        for (int col = currentColumn_; col < totalColumns_; ++col) {
            field();
        }

        *csv_ += eol_char_;
        currentColumn_ = 0;
    }

    void begin(Str& csv)
    {
        currentColumn_ = 0;
        columnsKnown_ = false;
        is_header_ = true;
        csv_ = &csv;
        csv_->clear();
    }

    void end()
    {
        if (!columnsKnown_)
            return;

        if (currentColumn_) {
            for (int col = currentColumn_; col < totalColumns_; ++col)
                field();
            *csv_ += eol_char_;
        }
    }


public:
    Str* csv_{};

private:
    bool columnsKnown_{};
    int totalColumns_{}, currentColumn_{};
    bool is_header_{};


    void ctor() {
        columnsKnown_ = false;
        totalColumns_ = currentColumn_ = 0;
    }

};

inline
Csv parseCsv(StrView text) {
    CsvParser parser;
    return parser.parse(text);
}

inline
Csv loadCsv(StrView filename) {
    CsvParser parser;
    return parser.load(filename);
}


}  // namespace

