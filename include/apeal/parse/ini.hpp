/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "preprocessor.hpp"
#include "../parse.hpp"
#include "../have.hpp"
#include "../string.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <map>


namespace apl {

struct SchemaFieldList {
    using list_pair_type = std::pair<String, String>;
    using list_type = std::vector<list_pair_type>;
};


struct IniSchemaField {
    using Str = String;

    using string_type = String;

    enum type_t {
        type_endl, type_comment, type_string, type_int, type_ulong, type_bool, type_double, type_float, type_list,
        type_array_int, type_array_float
    };
    type_t type{type_string};

    string_type name, value, comment;


    bool required{}, allow_repeat{};

    string_type* string_value{};
    int* int_value{};
    unsigned long* ulong_value{};
    bool* bool_value{};
    double* double_value{};
    float* float_value{};
    SchemaFieldList* list_value{};

    int array_size{0};

    // run-time data
    bool already_processed{};


    IniSchemaField() noexcept: type{type_endl} {}

    IniSchemaField(const string_type& iname, string_type* istring_value)
    {
        name = iname, type=type_string, string_value = istring_value;
    }

    IniSchemaField(const string_type& iname, int* iint_value)
    {
        name = iname, type=type_int, int_value = iint_value;
    }

    //#if sizeof(unsigned long) != sizeof(uint64_t)
    //	ACfgSchemaField ( const string_type &iname, unsigned long *ulong_value ) {
    //		ctor(), name = iname, type=type_ulong, ulong_value = ulong_value ;
    //	}
    //#endif

    IniSchemaField(const string_type& iname, uint64_t* ulong_value) {
        name = iname, type=type_ulong, ulong_value = ulong_value;
    }


    IniSchemaField(const string_type& iname, bool* ibool_value)
    {
        name = iname, type=type_bool, bool_value = ibool_value;
    }

    IniSchemaField(const string_type& iname, double* idouble_value)
    {
        name = iname, type=type_double, double_value = idouble_value;
    }

    IniSchemaField(const string_type& iname, float* value)
    {
        name = iname, type = type_float, float_value = value;
    }

    IniSchemaField(const string_type& iname, int size, int* value)
    {
        name = iname, type = type_array_int, array_size = size, int_value = value;
    }

    IniSchemaField(const string_type& iname, int size, float* value)
    {
        name = iname, type = type_array_float, array_size = size, float_value = value;
    }

};


struct IniSchema {
    using string_type = String;
    typedef IniSchemaField field_type;

    typedef std::vector<field_type> field_list_type;

    struct Section {
        string_type name;
        field_list_type fields;
    };
    typedef std::vector<Section> section_list_type;



    // options
    bool error_unknown = false;

    field_list_type fields;
    section_list_type sections;


    template <class value_type>
    void add(const string_type& name, const value_type v)
    {
        add_field(field_type(name, v));
    }

    template <class value_type>
    void add(const string_type& section, const string_type& name, const value_type v)
    {
        add_field(section, field_type(name, v));
    }


    template <class value_type>
    void add(const string_type& name, const int size, const value_type v)
    {
        add_field(field_type(name, size, v));
    }

    void insert_field(const string_type& name, string_type* value)
    {
        add_field(field_type(name, value));
    }


    void add_field(const field_type& new_field)
    {
        fields.push_back(new_field);
    }

    void add_field(const string_type& section, const field_type& field)
    {
        for (auto& s : sections) {
            if (s.name == section)
                s.fields.push_back(field);
        }
    }


    void add_comment(const string_type& comment)
    {
        fields.resize(fields.size()+1);
        field_type& f=fields.back();

        f.type = field_type::type_comment;
        f.comment = comment;
    }

    void add_endl()
    {
        fields.resize(fields.size()+1);
        field_type& f=fields.back();

        f.type = field_type::type_endl;
    }


    void add_section(const string_type& section)
    {
        Section s;
        s.name = section;
        sections.push_back(s);
    }

};



class Ini {
public:
    using string_type = String;

public:

    struct Element {
        String name, value, comment;
        int line{};
    };

    typedef Element element_type;

    typedef std::vector<element_type> element_list_type;


    struct Section {
        typedef Element element_type;

        string_type name, comment;
        element_list_type el;

        int line{};
    };

    typedef Section section_type;

    typedef std::vector<section_type> section_list_type;

public:

    element_list_type el;
    section_list_type sec;



public:  // building configuration file

    void clear()
    {
        el.clear();
        sec.clear();
    }

    void add(const string_type& name, const string_type& value)
    {
        el.resize(el.size()+1);
        element_type& item = el.back();
        item.name = name, item.value = value;
    }

    void add_comment(const string_type& comment)
    {
        el.resize(el.size()+1);
        element_type& item = el.back();
        item.comment = comment;
    }
};


class IniParser {

    using string_type = String;
    using ErrorPolicy = AErrorPolicy;

    using S = String;
    using C = Char;

private:
    enum line_result_enum {
        res_eof,
        res_sec,
        res_el,
        res_comment,
        res_empty
    };

    ErrorPolicy error_;
    String filename_;
    String error_string_;


public:
    bool use_space = false;
    Char name_separator{'='};
    int current_line{};

    bool multilineEnabled{false};
    Char multilineBeginChar{'{'}, multilineEndChar{'}'};


    void setNameSeparator(Char c) { name_separator = c; }
    void setMultilineEnabled(bool enabled = true) { multilineEnabled = enabled; }


    //
    // Loading from the file
    //
    template <class Obj>
    auto load(StrView uri, Obj& obj)
    {
        filename_ = uri;
        obj.clear();

        auto data = loadFile(uri);

        //std::ifstream file(uri.c_str(),std::ios::in|std::ios::binary) ;

        //if ( !file.good() )
        //	return error_(str_format(error_string_,"%s : error opening file", filename_)) ;

        //std::istreambuf_iterator<typename string_type::value_type> begin(file), end ;

        return parse_basic(data.begin(), data.end(), obj);
    }


    template <class Obj>
    auto load(Preprocessor& preprocessor, StrView uri, Obj& obj)
    {
        filename_ = uri;
        String preprocessed;

        preprocessor.load(uri, preprocessed);

        return parse_basic(preprocessed.begin(), preprocessed.end(), obj);
    }


    // loading from file
    /*
    template <class schema_type>
    bool load_schema ( const char *uri, schema_type &schema )
    {
        std::ifstream file(uri,std::ios::in|std::ios::binary) ;

        if ( !file.good() )
            return error_("error opening file") ;

        std::istreambuf_iterator<char> begin(file), end ;

        bool res = parse_schema_basic(begin,end,schema) ;

        file.close() ;

        return res ;
    }
    */

    // Opens the stream using std::ifstream and returns iterators
    // TODO: needs rewrite and proper error checking
    auto openStlFileStream(StrView filename, std::ifstream& file)
    {
#if APEAL_OS_MAC || APEAL_OS_POSIX
        // Wait until iPhones are updated to min iOS 13
        file.open(ApiString{filename}.utf8(), std::ios::in | std::ios::binary);
#else
        //std::filesystem::path path{filename.begin(), filename.end()};
        // TODO: write stream class
        file.open(ApiString{filename}.win(), std::ios::in | std::ios::binary);
#endif

        verify(file.good(), "Error opening: %s", filename);

        std::istreambuf_iterator begin{file};
        decltype(begin) end;

        return std::pair{begin, end};
    }

    // Opens the stream using std::ifstream and returns iterators
    // TODO: needs rewrite and proper error checking
    auto createStlFileStream(StrView filename, std::ofstream& file)
    {
#if APEAL_OS_MAC || APEAL_OS_POSIX
        // Wait until iPhones are updated to min iOS 13
        // TODO: write stream class
        file.open(ApiString{filename}.utf8(), std::ios::out | std::ios::binary);
#else
        file.open(ApiString{filename}.win(), std::ios::out | std::ios::binary);
#endif
        //std::ofstream file{{uri.begin(), uri.end()}, std::ios::out | std::ios::binary} ;

        verify(file.good(), "Error creating: %s", filename);
    }



    //
    // Load the schema from file
    //
    bool load_schema(StrView filename, IniSchema& schema)
    {
        //std::filebuf fb;
        std::ifstream file;//(&fb);
        auto it = openStlFileStream(filename, file);

        return parse_schema_basic(it.first, it.second, schema);
    }

    template <class I>
    bool parse_schema(I begin, I end, IniSchema& schema)
    {
        return parse_schema_basic(begin, end, schema);
    }

    template <class schema_type>
    bool save_schema(StrView filename, const schema_type& schema)
    {
        return save_schema_basic(filename, schema);
    }


public:  // parsing

    template <class I, class O>
    auto parse(I begin, I end, O& obj)
    {
        return parse_basic(begin, end, obj);
    }

    template <class O>
    auto parse(StrView str, O& obj)
    {
        return parse_basic(str.begin(), str.end(), obj);
    }


public:
    template <class object_type>
    bool save_basic(const object_type& obj, StrView uri)
    {
        std::ofstream file;
        createStlFileStream(uri, file);

        return generate(obj, file);
    }



    template <class schema_type>
    bool save_schema_basic(StrView uri, const schema_type& schema)
    {
        std::ofstream file;
        createStlFileStream(uri, file);

        using FieldType = IniSchemaField::type_t;

        for (auto f = schema.fields.begin(), f_end = schema.fields.end(); f != f_end; ++f) {
            if (f->type == FieldType::type_endl) {
            } else if (f->type == FieldType::type_comment) {
                file.put('#'), file.put(' '), file << f->comment;
            } else {
                file << f->name << name_separator; // '=' ;

                if (f->type == FieldType::type_string && f->string_value)
                    file << *f->string_value;
                if (f->type == FieldType::type_int && f->int_value)
                    file << int(*f->int_value);
                if (f->type == FieldType::type_ulong && f->ulong_value)
                    file << (unsigned long)(*f->ulong_value);
                if (f->type == FieldType::type_bool && f->bool_value)
                    file.put(*f->bool_value ? '1' : '0');
                if (f->type == FieldType::type_double && f->double_value)
                    file << *f->double_value;

                if (!f->comment.empty())
                    file.put(' '), file.put('#'), file.put(' '), file << f->comment;
            }


            file.put('\n');
        }

        file.close();
        return true;
    }

    template <class object_type>
    bool save(const object_type& obj, StrView uri)
    {
        return save_basic(obj, uri);
    }

    template <class element_type, class output_stream_type>
    void generate_element(element_type& el, output_stream_type& strm)
    {
        if (!el.name.empty()) {
            if (use_space) {
                strm << el.name << ' ' << name_separator << ' ' << el.value;
            } else {
                strm << el.name << name_separator << el.value;
            }
            if (!el.comment.empty())
                strm.write("   ", 3);
        }

        if (!el.comment.empty())
            strm.write("# ", 2), strm << el.comment;

        if (el.name.empty() && el.value.empty() && el.comment.empty())
            strm << "#";

        strm << std::endl;
    }

    template <class object_type, class output_stream_type>
    bool generate(object_type& obj, output_stream_type& strm)
    {
        // generate all elements
        for (auto el_it = obj.el.begin(), el_end = obj.el.end(); el_it != el_end; ++el_it) {
            generate_element(*el_it, strm);
        }

        // generate all sections
        for (const auto& sec : obj.sec) {
            strm << "[" << sec.name << "]";
            if (!sec.comment.empty())
                strm << "     # " << sec.comment;
            strm << std::endl;
            for (const auto& el : sec.el) {
                generate_element(el, strm);
            }
            strm << std::endl;
        }

        return true;
    }


private:
    template <class el_T, class field_list_T>
    bool parse_schema_fields(const el_T& el, field_list_T& fields)
    {
        typedef typename field_list_T::value_type schema_field_type;

        // search for the field name
        bool field_found = false;
        for (auto& field : fields) {
            if (field.type == schema_field_type::type_comment || field.type == schema_field_type::type_endl)
                continue;

            if (field.name == el.name) {
                if (field.already_processed && !field.allow_repeat)
                    throwError("Duplicate field: %s", el.name);

                field.value = el.value;
                field.comment = el.comment;

                switch (field.type) {
                case schema_field_type::type_string:
                    if (field.string_value)
                        str_trim(field.value, *field.string_value);
                    break;
                case schema_field_type::type_bool:
                    if (field.bool_value)
                        parse_bool(field.value, field.bool_value);
                    break;

                case schema_field_type::type_int:
                    if (field.int_value)
                        parse_int(field.value, *field.int_value);
                    break;

                case schema_field_type::type_ulong:
                    if (field.ulong_value)
                        parse_uint_radix(field.value, *field.ulong_value, 10);
                    break;

                case schema_field_type::type_double:
                    if (field.double_value) {
                        *field.double_value = parseDouble(field.value);
                    }
                    break;

                case schema_field_type::type_float:
                    if (field.float_value) {
                        *field.float_value = parseFloat(field.value);
                    }
                    break;

                case schema_field_type::type_array_int:
                    if (field.int_value)
                        parse_array(field);
                    break;

                case schema_field_type::type_array_float:
                    if (field.float_value)
                        parse_array(field);
                    break;

                default:
                    return error_("error:cfg_parser:unknown type");
                }

                field_found = true;
                field.already_processed = true;
                break;
            }
        }

        return field_found;
    }

    template <class I, class schema_type>
    bool parse_schema_basic(I begin, I end, schema_type& schema)
    {
        typedef typename schema_type::string_type string_type;
        typedef typename schema_type::field_type schema_field_type;
        typedef Ini obj_type;

        obj_type obj;
        parse_basic(begin, end, obj);

        // now walk through the object and parse all the fields
        for (auto& el: obj.el) {
            bool field_found = parse_schema_fields(el, schema.fields);

            if (!field_found && schema.error_unknown)
                throwError("Unknown field: %s", el.name);
        }

        for (auto& sec: schema.sections) {
            for (const auto& ssec : obj.sec) {
                if (ssec.name != sec.name)
                    continue;

                for (const auto& el: ssec.el) {

                    bool field_found = parse_schema_fields(el, sec.fields);

                    if (!field_found && schema.error_unknown)
                        throwError("Unknown field: %s in section: %s", el.name, ssec.name);
                }
            }
        }

        return true;
    }


    template <class schema_field_type>
    bool parse_array(schema_field_type& field)
    {
        auto c = field.value.begin();
        auto c_end = field.value.end();

        string_type current_value;
        typename string_type::value_type separator = ',';
        int array_current = 0;

        for (;;) {
            if (array_current >= field.array_size)
                break;
            current_value.clear();
            while (c != c_end && (*c == ' ' || *c == '\t'))
                ++c;
            while (c != c_end && *c != separator)
                current_value += *c++;
            if (c != c_end)  // skip separator
                ++c;
            if (c == c_end && current_value.empty())
                break;

            if (current_value.empty()) {
                switch (field.type) {
                case schema_field_type::type_array_int:  field.int_value[array_current] = 0;  break;
                case schema_field_type::type_array_float:  field.float_value[array_current] = 0;  break;
                default:
                    break;
                }
            } else {
                switch (field.type) {
                case schema_field_type::type_array_int:
                    parse_int(current_value, field.int_value[array_current]);
                    break;
                case schema_field_type::type_array_float:
                    field.float_value[array_current] = parseFloat(current_value);
                    break;
                default:
                    break;
                }
            }

            array_current++;

        }

        return true;
    }



private:
    template <class I, class Obj>
    auto parse_basic(I begin, I end, Obj& cfg)
    {
        auto curr = begin;
        current_line = 0;

        return load_section(curr, end, cfg, cfg);
    }


    template <class I, class section_type, class root_section_type>
    void load_section(I& curr, I end, section_type& section, root_section_type& root_section)
    {
        typedef typename section_type::element_type element_type;

        line_result_enum res;
        element_type element;

        for (bool eos = false; !eos; ) {
            res = read_line(curr, end, element.name, element.value, element.comment);

            switch (res) {
            case res_comment:
            case res_el:
                element.line = current_line;
                section.el.push_back(element);
                break;
            case res_empty:  break;
            case res_eof:
                eos = true;
                break;
            case res_sec:
                root_section.sec.resize(root_section.sec.size()+1);
                root_section.sec.back().name = element.name;
                root_section.sec.back().comment = element.comment;
                root_section.sec.back().line = current_line;
                load_section(curr, end, root_section.sec.back(), root_section);
                eos = true;
                break;
            default:
                error_("unknown type");
                break;
            }
        }
    }


    template <class I>
    line_result_enum read_line(I& curr, I end, String& name, String& value, String& comment)
    {
        name.clear(), value.clear(), comment.clear();

        // get the first non-space character
        for (; curr != end && is_space(*curr); ++curr)
            ;

        if (curr == end)
            return res_eof;

        // the very first character may be ';', then this is also comment
        if (*curr == ';') {
            for (++curr; curr != end && !is_eol(*curr); ++curr)
                if (is_char(*curr))
                    comment.append(1, *curr);

            if (curr != end && is_eol(*curr))
                ++curr, ++current_line;
            return res_comment;
        }

        // check if this is a section
        if (*curr == '[') {

            // skip spaces
            for (curr++; curr != end && is_space(*curr); curr++)
                ;
            if (curr == end)
                throw std::runtime_error("Section name missing");

            // collect the section name until space
            for (; curr != end && !is_space(*curr) && !is_eol(*curr) && *curr != ']'; curr++)
                if (is_char(*curr))
                    name.append(1, *curr);

            if (curr == end || is_eol(*curr))
                throwError("Section name missing");

            // go until end of section
            for (; curr != end && !is_eol(*curr) && *curr != ']'; curr++)
                ;

            if (curr == end || *curr != ']')
                throwError("End of section is missing");

            // skip all the spaces
            for (++curr; curr != end && !is_eol(*curr) && is_space(*curr); ++curr)
                ;

            if (curr != end && is_eol(*curr))
                ++curr, ++current_line;
            else if (*curr != '#')
                throwError("Unexpected character");

            else {  // process comment
                for (++curr; curr != end && !is_eol(*curr); ++curr)
                    if (is_char(*curr))
                        comment.append(1, *curr);
                ++current_line;
            }

            return res_sec;
        }

        // check if this is a comment
        if (*curr == '#') {
            for (++curr; curr != end && !is_eol(*curr); ++curr)
                if (is_char(*curr))
                    comment.append(1, *curr);

            if (curr != end && is_eol(*curr))
                ++curr, ++current_line;
            return res_comment;
        }


        // its a regular variable
        // skip spaces
        for (; curr != end && (is_space(*curr)||!is_char(*curr)) && !is_eol(*curr); ++curr)
            ;

        // check if this is an empty string
        if (curr == end)
            return res_empty;

        if (is_eol(*curr)) {
            ++curr, ++current_line;
            return res_empty;
        }


        //
        // collect the variable name
        //
        //
        // Allow constructions like this: "this is  name = value" the parse will produce "this is name" and "value"

        for (;;) {
            for (; curr != end && !is_space(*curr) && !is_eol(*curr) && *curr!=name_separator; ++curr)
                if (is_char(*curr))
                    name.append(1, *curr);

            // skip spaces
            for (; curr != end && is_space(*curr); ++curr)
                ;

            // expect '='
            if (curr == end || is_eol(*curr)) {
                error_(str_format(error_string_, "%s : %d : Expected '='", filename_, current_line+1));
                return res_eof;
            }

            if (*curr == name_separator)
                break;

            name += ' ';
        }

        ++curr;  // skip the '=' sign

        // The value is coming
        for (; curr != end && is_space(*curr); ++curr)
            ;

        bool multiline = multilineEnabled;
        if (multiline && *curr != multilineBeginChar)
            multiline = false;

        if (multiline) {
            ++curr;
            for (; curr != end && (is_space(*curr) || is_eol(*curr)); ++curr)
                if (is_eol(*curr))
                    ++current_line;
            // collect all the lines
            bool wasEol = false;
            for (; curr != end; ++curr) {
                if (is_eol(*curr))
                    ++current_line, wasEol = true;
                if (*curr == multilineEndChar /*&& wasEol*/) {
                    ++curr;
                    break;
                }
                //if ( !is_space(*curr) && !is_eol(*curr) )
                //	wasEol = false ;
                value.append(1, *curr);
            }

            value = str_trim(value);

            for (; curr != end && !is_eol(*curr); ++curr)
                ;

        } else {
            for (; curr != end && !is_eol(*curr); ++curr)
                if (is_char(*curr))
                    value.append(1, *curr);

            value = str_trim(value);

            if (curr!=end && is_eol(*curr))
                ++curr, ++current_line;
        }

        return res_el;
    }


    bool is_space(Char c) const noexcept { return c == ' ' || c == '\t' || c == '\r'; }
    bool is_eol(Char c) const noexcept { return c == '\n'; }
    bool is_char(Char c) const noexcept { return c != '\r'; }

};


using AIni = Ini;
using AIniParser = IniParser;



// shortcut
inline 
auto loadIni(Preprocessor& preprocessor, StrView filename)
{
    Ini ini;
    IniParser parser;

    parser.load(preprocessor, filename, ini);
    return ini;
}

inline 
auto loadIni(StrView filename)
{
    Ini ini;
    IniParser parser;

    parser.load(filename, ini);
    return ini;
}

inline 
auto parseIni(StrView data)
{
    Ini ini;
    IniParser parser;
    parser.parse(data, ini);
    return ini;
}



} // apl namespace


