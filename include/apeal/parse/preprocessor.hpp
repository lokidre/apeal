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

#include "../str/case.hpp"
#include "../str/trim.hpp"

#include "../file.hpp"

#include <fstream>
#include <stack>
#include <tuple>
#include <unordered_map>


namespace apl {


class Preprocessor {
public:
    using ErrorPolicy = AErrorPolicy;

    using InputIterator = typename StrView::const_iterator;
    using Input = ParseInput<InputIterator>;

public:
    Char preprocessorChar{'@'};
    bool unquotedIncludes{};
    String includeExt;
    Char commentChar = '#';
    String multilineCommentBegin = "#[[";
    String multilineCommentEnd = "]]";
    String include_prefix;  // Prefix to be added to any file in 'include' statement

    using SearchDirsList = std::vector<String>;

    SearchDirsList search_dirs;

    struct Dep {
        String filepath;
        Index line, count;
    };

    using Deps = std::vector<Dep>;
    Deps deps;


    using Vars = std::unordered_map<String, String>;
    Vars vars;

private:

    struct Macro {
        String name;

        struct Param {
            String name, def;
        };
        using ParamList = std::vector<Param>;
        ParamList params;

        String contents;
    };
    using MacroList = std::unordered_map<String, Macro>;
    MacroList macros_;

    std::string contents_;

    std::stack<bool> conditionsStack_;
    bool currentCondition_ = true;
    int currentConditionDepth_ = 0;


    template <class ...Args>
    bool error(const Input &input, StrView fmt, Args&&... args) {
        error_(sformat("%d:%s", input.line, sformat(fmt, std::forward<Args>(args)...)));
        return false;
    }



public:

    bool parse_basic(InputIterator begin, InputIterator end, String &output)
    {
        Input input{begin, end};

        String temp_line;
        String keyword;
        String serr;

        lineCount_ = 0;

        while (!input.end()) {
            // process the line
            temp_line.clear();

            for (;;) {

                // skip whitespace
                while (!input.end() && is_space(input.peek()))
                    temp_line += input.read();

                // handle end of file situation
                if (input.end()) {
                    output += temp_line;
                    lineCount_++;
                    break;
                }

                // handle end of line situation
                if (is_eol(input.peek())) {
                    output += temp_line;
                    output += input.read();
                    lineCount_++;
                    break;
                }

                // read first char
                auto c = input.read();

                if (commentChar && commentChar == c) {

                    // check for multiline comment
                    bool multiline = false;
                    if (!multilineCommentBegin.empty()) {
                        multiline = true;
                        auto it = multilineCommentBegin.begin();
                        ++it;
                        for (; it != multilineCommentBegin.end(); ++it) {
                            if (!input.end() && *it != input.peek()) {
                                multiline = false;
                                break;
                            }
                            input.read();
                        }
                    }
                    if (multiline) {
                        while (!input.end()) {
                            if (input.read() != multilineCommentEnd[0])
                                continue;
                            auto it = multilineCommentEnd.begin();
                            ++it;
                            auto finished = true;
                            for (; it != multilineCommentEnd.end(); ++it) {
                                if (!input.end() && *it != input.read()) {
                                    finished = false;
                                    break;
                                }
                            }
                            if (finished)
                                break;
                        }
                    } else {
                        skipLine(input);
                    }
                    continue;
                }

                if (preprocessorChar == c) {
                    // read the preprocessor command
                    while (!input.end() && !is_eol(input.peek()) && is_space(input.peek()))
                        input.read();
                    if (input.end() || is_eol(c))
                        break;

                    readTerm(input, keyword);

                    if (input.end())
                        break;

                    if ("include" == keyword)	process_include(input, output, false);
                    else if ("include_once" == keyword)	process_include(input, output, true);
                    else if ("macro" == keyword) process_macro(input, output);
                    else if ("expand" == keyword) process_expand(input, output);
                    else if ("set" == keyword) process_set(input, output);
                    else if ("if" == keyword) process_if(input, output);
                    else if ("elif" == keyword) process_elif(input, output);
                    else if ("else" == keyword) process_else(input, output);
                    else if ("endif" == keyword) process_endif(input, output);
                    else if (keyword.empty())	return error_("Keyword is missing");
                    else return error_(sformat("Invalid preprocessor keyword - '%s'", keyword));

                    continue;
                }

                // if we are here, then the line is not a preprocessor line
                if (currentCondition_) {
                    output += temp_line;
                    temp_line.clear();
                    output += c;
                    while (!input.end() && !is_eol(input.peek()))
                        output += input.read();
                    if (!input.end())  // add EOL
                        output += input.read();
                }
                lineCount_++;
            }
        }

        return true;
    }

    bool parse(StrView text, String &output)
    {
        return parse_basic(text.begin(), text.end(), output);
    }

    // loading from file
    bool load(StrView uri, String &output)
    {
        auto data = loadFile(uri);

        StrView dataView{data};
        return parse_basic(dataView.begin(), dataView.end(), output);
    }

    String load(StrView uri) 
    {
        String s;
        load(uri, s);
        return s;
    }


private:
    ErrorPolicy error_;
    String error_string_;

    Char endIncludeChar_;
    int lineCount_;


private:
    bool process_include(Input &input, String &output, bool once)
    {
        if (!currentCondition_) {
            skipLine(input);
            return true;
        }

        skipWhitespace(input);
        if (input.end())
            return true;
        auto c = input.peek();
        if (c == '\"')
            endIncludeChar_ = '\"';
        else if (c == '<')
            endIncludeChar_ = '>';
        else
            if (unquotedIncludes)
                endIncludeChar_ = 0;
            else
                return error(input, "Invalid preprocessor character '%c'", c);

        if (!unquotedIncludes)
            input.read();

        String filename;
        while (!input.end()) {
            c = input.read();
            if (is_eol(c)) {
                if (unquotedIncludes)
                    break;
                return error(input, "Unterminated @include '%c'", c);
            }
            if (!unquotedIncludes && c == endIncludeChar_) {
                input.read();
                break;
            }

            filename += c;
        }

        filename = str_trim(filename);

        AFileSystem fs;

        if (!includeExt.empty() && extractFileExt(filename).empty()) {
            filename += "." + includeExt;
        }

        auto dirs = search_dirs;
        dirs.push_back(".");
        String filepath;
        bool fileFound = false;
        for (auto &dir: dirs) {
            filepath = makeFilePath(dir, filename);
            if (fs.exists(filepath)) {
                fileFound = true;
                break;
            }
        }
        if (!fileFound)
            return error_(str_format(error_string_, "%d : include file not found: '%s'", input.line, filename));

        if (once) {
            auto foundIt = std::find_if(deps.begin(), deps.end(), [&filepath](auto &dep) {
                return dep.filepath == filepath;
            });

            if (foundIt != deps.end())
                return true;
        }

        auto data = loadFile(filepath);
        data.append(input.sourceIterator(), input.sourceEnd());
        contents_ = data;
        StrView contentsView{contents_};
        input.emplace(contentsView.begin(), contentsView.end());
        //output += contents ;
        output += '\n';

        while (!input.end() && is_eol(!input.peek()))
            input.read();
        if (!input.end())
            input.read();

        deps.push_back({filepath, lineCount_, (int)std::count(contents_.begin(),contents_.end(),'\n')});

        return true;
    }

    bool process_set(Input &input, String &output)
    {
        if (!currentCondition_) {
            skipLine(input);
            return true;
        }

        // Read variable name
        auto varname = readTerm(input);

        if (varname.empty())
            return error(input, "Variable name is missing");

        // equal sign
        if ('=' != input.read())
            return error(input, "Expected '='");

        // Read the value
        auto value = readTerm(input);

        if (value.empty())
            return error(input, "Value is missing");

        skipLine(input);

        vars[varname] = value;

        return true;
    }

    bool process_if(Input &input, String &output)
    {
        auto var = readTerm(input);
        auto it = vars.find(var);
        if (it == vars.end())
            return error(input, "Unknown variable: %s", var);

        bool condition = true;

        if (it->second == "0" || str_lower(it->second) == "false")
            condition = false;

        conditionsStack_.push(condition);

        skipLine(input);

        if (!condition) {
            if (currentCondition_)
                currentCondition_ = false;

            currentConditionDepth_++;
        }

        return true;
    }

    bool process_elif(Input &input, String &output)
    {
        ADebugTrap(); // TODO: needs to be implemented
        return true;
    }

    bool process_else(Input &input, String &output)
    {
        if (conditionsStack_.empty())
            return error(input, "Unmatched 'else'");

        if (currentCondition_) {
            currentCondition_ = false;
            currentConditionDepth_++;
        } else {
            if (currentConditionDepth_ == 1) {
                currentCondition_ = true;
                currentConditionDepth_--;
            }
        }

        return true;
    }

    bool process_endif(Input &input, String &output)
    {
        if (conditionsStack_.empty())
            return error(input, "Unmatched 'endif'");

        auto prevCond = conditionsStack_.top();
        conditionsStack_.pop();

        if (currentConditionDepth_ > 0) {
            currentConditionDepth_--;
            if (currentConditionDepth_ == 0) {
                currentCondition_ = prevCond;
            } else {
            }
        }

        return true;
    }



    void process_macro(Input &input, String &output)
    {
        String term, spaceline;

        // Read macro name
        Macro macro;
        readTerm(input, macro.name);

        if (macro.name.empty())
            return error_(str_format(error_string_, "%d : macro name is missing", input.line)), void(0);

        // check if macro is already defines
        if (auto itt = macros_.find(macro.name); itt != macros_.end())
            return error_(str_format(error_string_, "%d : macro is already defined: '%s'", input.line, macro.name)), void(0);


        // Process the parameters
        skipWhitespace(input);
        if (!input.eof() && !input.eol()) {
            // expect parameters in parenthesis
            if (input.peek() != '(')
                return error_(str_format(error_string_, "%d : expecting '(' in macro: '%s'", input.line, macro.name)), void(0);
            input.read();

            for (;;) {
                skipWhitespace(input);
                if (input.end() || input.eol())
                    return error_(str_format(error_string_, "%d : missing ')' in macro: '%s'", input.line, macro.name)), void(0);

                typename Macro::Param param;

                readTerm(input, param.name);
                if (param.name.empty())
                    return error_(str_format(error_string_, "%d : missing parameter name in macro: '%s'", input.line, macro.name)), void(0);

                skipWhitespace(input);
                if (input.end())
                    return error_(str_format(error_string_, "%d : missing ')' in macro: '%s'", input.line, macro.name)), void(0);

                // Check for default value
                if (input.peek() == '=') {
                    input.read();
                    readString(input, param.def);
                    skipWhitespace(input);

                    if (input.eof())
                        return error_(str_format(error_string_, "%d : missing parameter name in macro: '%s'", input.line, macro.name)), void(0);
                }

                macro.params.push_back(param);

                if (input.peek() == ',') {  // Next parameter
                    input.read();
                    continue;
                }

                if (input.peek() == ')')  // end of parameters
                    break;
            }
        }
        skipLine(input);


        // Now expect the macro body
        for (;;) {
            spaceline.clear();
            appendWhitespace(input, spaceline);
            if (input.eof())
                return error_(str_format(error_string_, "%d : unterminated macro: '%s'", input.line, macro.name)), void(0);
            if (input.eol()) {
                macro.contents += spaceline;
                appendLine(input, macro.contents);
                continue;
            }

            if (input.peek() == preprocessorChar) {
                spaceline += input.read();
                appendWhitespace(input, spaceline);
                readTerm(input, term);
                if (term == "endmacro" || term == "endm") {
                    skipLine(input);
                    macros_[macro.name] = macro;
                    return;
                }

                macro.contents += spaceline;
                macro.contents += term;
            }

            appendLine(input, macro.contents);
        }
    }

    void process_expand(Input &input, String &output)
    {
        // Read macro name
        auto macroName = readTerm(input);

        if (macroName.empty())
            return error_(str_format(error_string_, "%d : macro name is missing", input.line)), void(0);

        // find the macro
        auto it = macros_.find(macroName);
        if (it == macros_.end())
            return error_(str_format(error_string_, "%d : macro not found: '%s'", input.line, macroName)), void(0);

        auto &macro = it->second;

        // Prepared contents
        auto macroBody = macro.contents;

        // Process the parameters
        std::vector<std::tuple<bool, String>> parms;
        parms.resize(macro.params.size());

        skipWhitespace(input);

        // Read the parameters
        if (!input.eof() && !input.eol()) {
            // expect parameters in parenthesis
            if (input.peek() == '(') {
                input.read();
                size_t paramIndex = 0;
                for (;;) {

                    skipWhitespace(input);
                    if (input.end() || input.eol())
                        return error_(str_format(error_string_, "%d : expected ')' in : '%s'", input.line, macroName)), void(0);

                    if (input.peek() == ')') {
                        skipLine(input);
                        break;
                    }

                    skipWhitespace(input);

                    String name, value;
                    readParamValue(input, value);
                    // check if this is a name or value
                    skipWhitespace(input);
                    if (!input.eof() && input.peek() == '=') {
                        name = value;
                        input.read();
                        readParamValue(input, value);

                        // search for the parameter
                        bool found = false;
                        size_t i = 0;
                        for (auto &p : macro.params) {
                            if (p.name == name) {
                                found = true;
                                paramIndex = i;
                                break;
                            }
                            ++i;
                        }
                        if (!found)
                            return error_(str_format(error_string_, "%d : parameter not found: '%s'", input.line, name)), void(0);
                    }

                    skipWhitespace(input);

                    if (input.peek() == ',') {
                        input.read();
                    } else {
                        if (input.peek() != ')')
                            return error_(str_format(error_string_, "%d : expected ',' or ')': '%s'", input.line, macro.name)), void(0);
                    }


                    if (paramIndex >= parms.size())
                        return error_(str_format(error_string_, "%d : extra parameters in : '%s'", input.line, macro.name)), void(0);

                    std::get<0>(parms[paramIndex]) = true;
                    std::get<1>(parms[paramIndex]) = value;

                    ++paramIndex;
                }
            } else {  // No parameters
                skipLine(input);
            }
        }

        // check the parameters
        size_t index = 0;
        for (auto &pi : parms) {
            if (!std::get<0>(pi)) {
                if (macro.params[index].def.empty())
                    return error_(str_format(error_string_, "%d : parameter is missing: '%s'", input.line, macro.name)), void(0);
                std::get<1>(pi) = macro.params[index].def;
            }
            ++index;
        }

        // Expand the parameters
        String bodyOutput;
        String term;
        size_t paramIndex = 0;
        for (const auto &p : parms) {
            const auto &pi = macro.params[paramIndex];

            StrView macroBodyView{macroBody};
            Input body{macroBodyView.begin(), macroBodyView.end()};
            //ParseInput body{macroBody.begin(), macroBody.end()};

            bodyOutput.clear();

            for (;;) {
                appendWhitespace(body, bodyOutput);
                if (body.end())
                    break;
                if (body.peek() == '@') {
                    body.read();
                    readTerm(body, term);
                    if (pi.name == term) {
                        bodyOutput += get<1>(p);
                    } else {
                        bodyOutput += '@';
                        bodyOutput += term;
                    }
                } else {
                    bodyOutput += body.read();
                }
            }

            macroBody = bodyOutput;
            ++paramIndex;
        }

        // expand the macro
        output += macroBody;
    }


    String readTerm(Input &input)
    {
        String term;
        readTerm(input, term);
        return term;
    }

    void readTerm(Input &input, String &term)
    {
        term.clear();

        skipWhitespace(input);

        if (input.end())
            return;

        auto peek = input.peek();

        if (is_eol(peek) || !is_varalpha(peek))
            return;

        term += input.read();

        while (!input.end() && is_varchar(input.peek()))
            term += input.read();
    }

    void skipWhitespace(Input &input) {
        while (!input.end() && !input.eol() && is_space(input.peek()))
            input.read();
    }

    void appendWhitespace(Input &input, String &line) {
        while (!input.end() && !input.eol() && input.isSpace())
            line += input.read();
    }

    void skipLine(Input &input) 
    {
        while (!input.end() && !input.eol())
            input.read();
        if (!input.eof())
            input.read();
    }

    void appendLine(Input &input, String &line) 
    {
        while (!input.end() && !input.eol())
            line += input.read();
        if (!input.eof())
            line += input.read();
    }

    void readString(Input &input, String &term) 
    {
        term.clear();
        bool quoted_ = false;
        char quoteChar = '\0';
        if (input.peek() == '\'' || input.peek() == '\"')
            quoted_ = true, quoteChar = input.read();

        for (;;) {
            if (input.eof() || input.eol())
                return;
            if (quoted_) {
                if (input.peek() == quoteChar) {
                    input.read();
                    return;
                }
            } else {
                if (!is_varchar(input.peek()))
                    return;
            }
            term += input.read();
        }
    }

    void readParamValue(Input &input, String &value) 
    {
        value.clear();

        auto c = input.peek();
        if (c == '\'' || c == '\"' /*|| is_varchar(c)*/) {
            readString(input, value);
            return;
        }

        // read arbitrary data up until comma or closing parenthesis
        while (!input.end()) {
            if (input.eol())
                break;

            c = input.peek();
            if (c == ',' || c == ')')
                break;
            value += input.read();
        }
        value = str_trim(value);
    }

private:
    bool is_eol(Char c) const noexcept { return c == '\n'; }
    bool is_space(Char c) const noexcept { return c=='\r' || c==' ' || c=='\t'; }
    bool is_alpha(Char c) const noexcept { return (c>='A'&&c<='Z') || (c>='a'&&c<='z'); }
    bool is_num(Char c) const noexcept { return c>='0' && c<='9'; }
    bool is_varalpha(Char c) const noexcept { return is_alpha(c) || c == '_'; }
    bool is_varchar(Char c) const noexcept { return is_alpha(c) || is_num(c) || c == '_'; }

};

}  // namespace apl

