/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once


#include "../error.hpp"
#include "../string.hpp"
#include "../str/case.hpp"

#include "parse_input.hpp"
#include "float.hpp"
#include "bool.hpp"
#include "int.hpp"


#include <vector>
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>

namespace apl {


//
// Xml object
//

struct XmlAttribute {
    String name, value;
};

struct XmlAttributeList: public std::vector<XmlAttribute> {

    String getValue(StrView name) const 
    {
        for (auto& att: *this) {
            if (att.name == name)
                return att.value;
        }
        return "";
    }

    auto value(StrView name) const { return getValue(name); }

    void set_value(StrView name, StrView value) noexcept
    {
        for (auto& att: *this) {
            if (att.name == name) {
                att.value = value;
                return;
            }
        }

        auto& att = this->emplace_back();
        att.name = name;
        att.value = value;
    }

    int int_value(StrView name, int def = 0) const { 
        return intValue(name, def); 
    }

    int intValue(StrView name, int def = 0) const
    {
        int val = -1;
        auto v = getValue(name);
        if (v.empty())
            return def;
        parse_int(v, val);
        return val;
    }

    void set_int_value(StrView name, int i) noexcept {
        String s;
        str_format(s, "%d", i);
        set_value(name, s);
    }

    bool bool_value(StrView name) const { return boolValue(name); }

    bool boolValue(StrView name) const {
        bool val = false;
        parse_bool(getValue(name), &val);
        return val;
    }

    double double_value(StrView name) const {
        double v = 0;
        parseReal(getValue(name), v);
        return v;
    }

    float float_value(StrView name) const {
        float v = 0;
        parseFloat(getValue(name), v);
        return v;
    }


    bool exists(StrView name) const noexcept {
        for (auto& att: *this) {
            if (att.name == name)
                return true;
        }

        return false;
    }

//private:
    //String empty_string_;
};


struct XmlElement {
    using Str = String;

    using AttributeList = XmlAttributeList;
    using Element = XmlElement;

    String name;  // tag
    String content;

    using ElementList = std::vector<XmlElement>;


    //
    // Attributes
    //
    AttributeList atts;

    // convenient routines
    // finds the first appearance of parameter with name 'name'
    auto param_find(StrView n)
    {
        for (auto it = atts.begin(), end = atts.end(); it != end; ++it)
            if (it->name == n)
                return it;
        return atts.end();
    }

    void att_add(StrView nam, StrView val)
    {
        auto& a = atts.emplace_back();
        a.name = nam;
        a.value = val;
    }

    String att_value(StrView n) const
    {
        return get_att(n);
    }

    String get_att(StrView n) const
    {
        for (auto& att: atts)
            if (att.name == n)
                return att.value;

        // needed exception maybe
        return {};  // empty
    }

    String getAtt(StrView n) const { return get_att(n); }


    //
    // XML element list
    //
    ElementList el;

    // convenient routines
    auto element_find(StrView tag) noexcept
    {
        for (auto it = el.begin(); it != el.end(); it++)
            if (it->name == tag)
                return it;
        return el.end();
    }



    bool empty{};  // empty tag


    //Element(StrView n) noexcept: name{n}, empty{false} {}
    //Element() noexcept: empty{false} {}

    void clear()
    {
        *this = {};
        //name.clear();
        //content.clear();
        //atts.clear();
        //el.clear();
        //empty = false;
    }


    auto select(StrView node, StrView condAtt = {}, StrView condVal = {}) const
    {
        ElementList els;
        select_body(node, els, condAtt, condVal);
        return els;
    }


    void select(StrView node, ElementList& els, StrView condAtt = {}, StrView condVal = {}) const
    {
        els.clear();
        select_body(node, els, condAtt, condVal);
    }

    void select_body(StrView node, ElementList& els, StrView condAtt = {}, StrView condVal = {}) const
    {
        for (auto el_it = el.begin(), el_end = el.end(); el_it != el_end; ++el_it) {
            el_it->select_body(node, els, condAtt, condVal);
            if (el_it->name == node) {
                if (!condAtt.empty() && el_it->atts.getValue(condAtt) != condVal)
                    continue;
                els.push_back(*el_it);
            }
        }
    }
};

using XmlElementList = XmlElement::ElementList;


struct Xml {
    using Str = String;

    using Attribute = XmlAttribute;
    using AttributeList = XmlAttributeList;

    using Element = XmlElement;
    using ElementList = XmlElementList;

    struct DocType {
        Str name;
        void clear() { name.clear(); }
    };

    struct Prolog {
        String version{"1.0"}, encoding, standalone;
        DocType doctype;

        void clear() noexcept {
            *this = {};
            version = "1.0";
        }
    };
    
    Prolog prolog;
    Element element;

    void clear() noexcept {
        *this = {};
        prolog.clear();
    }


    //
    // Queries
    //

    ElementList select(StrView node, StrView condAtt = {}, StrView condVal = {}) const
    {
        ElementList els;
        select_basic(element, node, els, condAtt, condVal);
        return els;
    }

    void select(StrView node, ElementList& els, StrView condAtt = {}, StrView condVal = "") const
    {
        els.clear();
        select_basic(element, node, els, condAtt, condVal);
    }

    String value(const String& node, const String& att, const String cond_att = "", const String cond_val = "") const
    {
        ElementList els;
        select_basic(element, node, els, cond_att, cond_val);
        if (els.empty())
            return "";
        return els[0].atts.getValue(att);
    }


    void update(StrView node, StrView att, StrView value, StrView cond_att = "", StrView cond_val = "") {
        update_basic(element, node, att, value, cond_att, cond_val);
    }

    void insert(StrView node, StrView name, StrView cond_att = "", StrView cond_val = "") {
        insert_basic(element, node, name, cond_att, cond_val);
    }

    void remove(StrView node, StrView cond_att = "", StrView cond_val = "") {
        remove_basic(element, node, cond_att, cond_val);
    }


private:
    void select_basic(const Element& current, StrView node, ElementList& els, StrView cond_att = "", StrView cond_val = "") const
    {
        for (auto el_it = current.el.begin(), el_end = current.el.end(); el_it != el_end; ++el_it) {
            select_basic(*el_it, node, els);
            if (el_it->name == node) {
                if (!cond_att.empty() && el_it->atts.getValue(cond_att) != cond_val)
                    continue;
                els.push_back(*el_it);
            }
        }
    }


    void update_basic(Element& current, StrView node, StrView att, StrView value, StrView cond_att = "", StrView cond_value = "")
    {
        for (auto el_it = current.el.begin(), el_end = current.el.end(); el_it != el_end; ++el_it)
            update_basic(*el_it, node, att, value, cond_att, cond_value);

        if (current.name == node && (cond_att.empty() || current.atts.getValue(cond_att) == cond_value))
            current.atts.set_value(att, value);
    }


    void remove_basic(Element& current, StrView node, StrView cond_att = "", StrView cond_val = "")
    {
        for (auto el_it = current.el.begin(), el_end = current.el.end(); el_it != el_end; ) {
            if (el_it->name == node && (cond_att.empty() || el_it->atts.getValue(cond_att) == cond_val)) {
                el_it = current.el.erase(el_it);
                continue;
            }
            remove_basic(*el_it, node, cond_att, cond_val);
            ++el_it;
        }
    }

    void insert_basic(Element& current, StrView node, StrView name, StrView condAtt = "", StrView condVal = "")
    {
        for (auto el_it = current.el.begin(), el_end = current.el.end(); el_it != el_end; ++el_it)
            insert_basic(*el_it, node, name, condAtt, condVal);

        if (current.name == node && (condAtt.empty() || current.atts.getValue(condAtt) == condVal)) {
            auto& el = current.el.emplace_back();
            el.name = name;
            current.empty = false;
        }
    }

};


struct AProcessObjectXML {

    AProcessObjectXML(Xml& obj): obj_(obj)
    {
        obj_.clear();
        el_ = obj_.element.el.end();
    }

    void version(StrView s) {
        obj_.prolog.version = s;
    }

    void standalone(const String& s)
    {
        obj_.prolog.standalone = s;
    }

    void encoding(const String& s)
    {
        obj_.prolog.encoding = s;
    }

    void doctype(const String& s)
    {
        obj_.prolog.doctype.name = s;
    }

    void element_open(const String& name)
    {
        if (obj_.element.name.empty()) {
            obj_.element.name = name;
        } else {
            XmlElementList::value_type e;
            if (el_ == obj_.element.el.end())
                el_ = obj_.element.el.insert(obj_.element.el.end(), e);
            else
                el_ = el_->el.insert(el_->el.end(), e);
            el_->name = name;
            el_stack_.push(el_);
        }
    }

    void element_empty(bool e = true)
    {
        if (el_ == obj_.element.el.end())
            obj_.element.empty = e;
        else
            el_->empty = e;
    }

    auto element_name() const
    {
        return (el_ == obj_.element.el.end()) ? obj_.element.name : el_->name;
    }

    bool is_element_empty() const
    {
        return (el_ == obj_.element.el.end()) ? obj_.element.empty : el_->empty;
    }

    void element_close(StrView name)
    {
        if (el_stack_.empty()) {
            el_ = obj_.element.el.end();
        } else {
            el_stack_.pop();
            el_ = el_stack_.top();
        }
    }

    void content(const String& s)
    {
        if (el_ == obj_.element.el.end())
            obj_.element.content += s;
        else
            el_->content += s;
    }

    void attribute(const String& name, const String& value)
    {
        if (el_ == obj_.element.el.end())
            obj_.element.att_add(name, value);
        else
            el_->att_add(name, value);
    }

private:
    Xml& obj_;

    std::stack<typename XmlElementList::iterator> el_stack_;
    typename XmlElementList::iterator el_;
};


//
// ---- XML document parser ----
//


class XmlParser {

public:
    using ErrorPolicy = AErrorPolicy;


private:
    // parts of xml language definition
    String xml_pi_begin{"<?"}, xml_decl_begin{"<?xml"}, xml_doctype_begin{"<!DOCTYPE"};
    String xml_cdata_start{"<![CDATA["}, xml_cdata_end{"]]>"};

    // this is ugly and temporary hack
    bool collect_elements;
    String collect_element_name;
    XmlElementList collect_element_list;

    ErrorPolicy error_;
    int error_code_;
    String error_string_;


public:  //

    template <class iterator_type, class obj_type>
    bool parse_basic(const iterator_type& begin, const iterator_type& end, obj_type& obj)
    {
        bool res = true;

        ParseInput<iterator_type> input(begin, end);

        try {
            obj.clear();

            //// handle the encoding (UTF-8 - TODO)
            //if ( input.peek() == 0xEF )
            //	input.read(), input.read(), input.read() ;

            process_prolog(input, obj);
            process_element(input, obj.element);
            process_misc(input);
        }
        catch (const std::exception& e) {
            error_string_ = e.what(), error_code_ = 1;
            res = false;
        }
        catch (...) {
            error_code_ = 1, error_string_ = "Exception!";
            res = false;
        }

        if (!res)
            return error_(error_string_);

        return res;
    }

    template <class iterator_type, class processor_T>
    bool parse_basic_new(const iterator_type& begin, const iterator_type& end, processor_T& proc)
    {
        bool res = true;
        error_code_ = 0;

        ParseInput<iterator_type> input{begin, end};

        try {
            process_prolog_new(input, proc);
            process_element_new(input, proc); //obj.element) ;
            process_misc(input);
        }
        catch (const std::exception& e) {
            error_string_ = e.what(), error_code_ = 1;
            res = false;
        }
        catch (...) {
            error_code_ = 1, error_string_ = "Exception!";
            res = false;
        }

        if (!res)
            return error_(error_string_);

        return res;
    }

    template <class object_T>
    bool parse(const String& text, object_T& obj)
    {
        return parse_basic(text.begin(), text.end(), obj);
    }

    template <class object_T>
    bool parse(const char* text, object_T& obj)
    {
        return parse_basic(text, text + strlen(text), obj);
    }

    template <class object_T>
    bool parse(char* text, object_T& obj)
    {
        return parse_basic(text, text + strlen(text), obj);
    }



    bool parse_new(const String& text, Xml& xml)
    {
        AProcessObjectXML proc(xml);
        return parse_basic_new(text.begin(), text.end(), proc);
    }


    template <class String, class object_T>
    inline bool parse(const char* text, object_T& obj)
    {
        return parse_basic(text, text+strlen(text), obj);
    }


    template <class element_list_type>
    bool parse_elements(const String& text, const String& name, element_list_type& obj)
    {
        // warning, this is ugly. need to rewrite
        collect_elements = true;
        collect_element_name = name;
        collect_element_list.clear();

        Xml tmp_obj;

        if (!parse_basic(text.begin(), text.end(), tmp_obj))
            return false;

        obj = collect_element_list;

        return true;
    }


    // loading from file
    template <class xml_object_type>
    bool load(const String& uri, xml_object_type& obj)
    {
        std::ifstream file(uri.c_str(), std::ios_base::in | std::ios_base::binary);

        collect_elements = false;

        if (!file.good())
            return error_(str_format(error_string_, "error opening file - '%s'", uri));

        std::istreambuf_iterator<Char> begin(file), end;

        return parse_basic(begin, end, obj);
    }

    bool load_elements(const String& uri, const String& name, XmlElementList& obj)
    {
        // warning, this is ugly. need to rewrite
        collect_elements = true;
        collect_element_name = name;
        collect_element_list.clear();

        std::ifstream file(uri.c_str(), std::ios_base::in|std::ios_base::binary);

        if (!file.good())
            return error_(str_format(error_string_, "error opening file - '%s'", uri));

        std::istreambuf_iterator<typename String::value_type> begin(file), end;

        Xml tmp_obj;

        if (!parse_basic(begin, end, tmp_obj))
            return false;

        obj = collect_element_list;

        return true;
    }


    bool load_elements_new(const String& uri, const String& name, XmlElementList& obj)
    {
        // warning, this is ugly. need to rewrite
        collect_elements = true;
        collect_element_name = name;
        collect_element_list.clear();

        std::ifstream file(uri.c_str(), std::ios_base::in|std::ios_base::binary);

        if (!file.good())
            return error_(str_format(error_string_, "error opening file - '%s'", uri));


        std::istreambuf_iterator<typename String::value_type> begin(file), end;

        Xml tmp_obj;
        AProcessObjectXML proc(tmp_obj);

        if (!parse_basic_new(begin, end, proc))
            return false;

        obj = collect_element_list;

        return true;
    }


    template <class object_type>
    bool save(const String& uri, object_type& obj)
    {
        std::ofstream file(uri.c_str(), std::ios_base::out|std::ios_base::binary);

        if (!file.good())
            return error_(str_format(error_string_, "error opening file - '%s'", uri));

        return generate(obj, file);
    }


    template <class object_type, class String>
    bool generate(const object_type& xml, String& text)
    {
        bool res = true;

        text.clear();

        try {
            generate_prolog(xml, text);
            int tab_level = 0;
            generate_element(xml.element, tab_level, text);
        }
        catch (const std::exception& e) {
            error_string_ = e.what(), error_code_ = 1;
            res = false;
        }
        catch (...) {
            error_code_ = 1, error_string_ = "Exception!";
            res = false;
        }

        if (!res)
            return error_(error_string_);

        return res;
    }



    //
    // Various utilities
    //
    template <class input_iterator_type>
    static String& escape_string_basic(const input_iterator_type& begin, const input_iterator_type& end, String& str)
    {
        str.clear();
        std::stringstream ss;
        for (auto c = begin; c != end; ++c) {
            generate_entity(ss, *c);
        }
        str = ss.str();

        return str;
    }

    static String& escape_string(StrView s, String& o)
    {
        return escape_string_basic(s.begin(), s.end(), o);
    }

    // note: slow version - extra copying
    static String escape_string(StrView s)
    {
        String o;
        escape_string(s, o);
        return o;
    }

private:  // XML parser routines
    template <class input_T, class obj_type>
    void process_prolog(input_T& input, obj_type& obj)
    {
        String qstr;

        typedef typename String::value_type value_type;

        obj.prolog.clear();

        // skip spaces
        skip_space(input);

        // XML declaration
        if (xml_keyword(input, xml_decl_begin)) {

            String name;

            for (;;) {
                skip_space(input);

                auto q = input.read(), b = input.read();
                if (q=='?' && b=='>')
                    break;
                else
                    input.unread(b), input.unread(q);


                expect_keyword(input, name);

                skip_space(input);

                str_lower_update(name);

                if (name == "version") {  // version info
                    expect_eq(input);
                    expect_quoted_string(input, obj.prolog.version);
                } else if (name == "standalone") {  // standalone document
                    expect_eq(input);
                    expect_quoted_string(input, obj.prolog.standalone);
                } else if (name == "encoding") {  // standalone document
                    expect_eq(input);
                    expect_quoted_string(input, obj.prolog.encoding);
                }
            }
        }

        skip_space(input);

        if (xml_keyword(input, xml_doctype_begin)) {

            expect_name(input, obj.prolog.doctype.name);
            skip_space(input);

            while (!input.end() && input.peek()!='>')
                input.read();

            input.read();  // skip the closing '>'
        }

        process_misc(input);
    }


    template <class input_T, class processor_T>
    void process_prolog_new(input_T& input, processor_T& proc)
    {
        //text_type qstr ;

        typedef typename String::value_type value_type;

        String name, value, qstr;

        // obj.prolog.clear() ;

        // skip spaces
        skip_space(input);

        // XML declaration
        if (xml_keyword(input, xml_decl_begin)) {

            //text_type name ;

            for (;;) {
                skip_space(input);

                auto q = input.read(), b = input.read();
                if (q=='?' && b=='>')
                    break;

                input.unread(b), input.unread(q);


                expect_keyword(input, name);

                skip_space(input);

                str_lower_update(name);

                if (name == "version") {  // version info
                    expect_eq(input);
                    expect_quoted_string(input, value);//obj.prolog.version) ;
                    proc.version(value);
                } else if (name == "standalone") {  // standalone document
                    expect_eq(input);
                    expect_quoted_string(input, value);//obj.prolog.standalone) ;
                    proc.standalone(value);
                } else if (name == "encoding") {  // standalone document
                    expect_eq(input);
                    expect_quoted_string(input, value); //obj.prolog.encoding) ;
                    proc.encoding(value);
                }
            }
        }

        skip_space(input);

        if (xml_keyword(input, xml_doctype_begin)) {

            expect_name(input, name); //obj.prolog.doctype.name) ;
            proc.doctype(name);
            skip_space(input);

            while (!input.end() && input.peek()!='>')
                input.read();

            input.read();  // skip the closing '>'
        }

        process_misc(input);
    }


    template <class input_T, class element_T>
    void process_element(input_T& input, element_T& element)
    {
        if (input.read() != '<')
            throw std::runtime_error("Expected '<'");

        expect_name(input, element.name);
        element.empty = false;


        // read attributes (tag parameters)
        for (;;) {
            skip_space(input);

            if (input.peek() == '>') {
                input.read();
                break;
            }

            auto s = input.read(), b = input.read();
            if (s == '/' && b == '>') {
                element.empty = true;
                break;
            } else
                input.unread(b), input.unread(s);

            element.atts.resize(element.atts.size()+1);
            auto& p = element.atts.back();

            expect_name(input, p.name);
            expect_eq(input);
            expect_quoted_string(input, p.value);
        }


        process_content(input, element);
        process_misc(input);

        // ugly hack, needed temporary
        if (collect_elements && element.name == collect_element_name) {
            collect_element_list.push_back(element);
        }
    }

    template <class input_T, class processor_T>
    void process_element_new(input_T& input, processor_T& proc)
    {
        //typedef typename String::value_type value_type ;

        if (input.read() != '<')
            throw std::runtime_error("Expected '<'");

        String element_name, attribute_name, attribute_value;

        expect_name(input, element_name);

        proc.element_open(element_name);


        // read attributes (tag parameters)
        for (;;) {
            skip_space(input);

            if (input.peek() == '>') {
                input.read();
                break;
            }

            auto s = input.read(), b = input.read();
            if (s=='/' && b=='>') {
                proc.element_empty(true);
                break;
            } else
                input.unread(b), input.unread(s);


            expect_name(input, attribute_name);
            expect_eq(input);
            expect_quoted_string(input, attribute_value);
            proc.attribute(attribute_name, attribute_value);
        }


        process_content_new(input, proc);
        process_misc(input);
    }


    template <class input_T, class element_T>
    void process_content(input_T& input, element_T& element)
    {
        if (element.empty)
            return;

        skip_space(input);

        for (;;) {
            // check for cdata
            if (xml_keyword(input, xml_cdata_start)) {
                do {
                    element.content.append(1, input.read());
                } while (!input.end() && !xml_keyword(input, xml_cdata_end));
                input.advance(xml_cdata_end.length());
                continue;
            }

            // check for the end tag
            auto b = input.read(), s = input.read();

            if (b=='<' && s== '/') {
                // check the tag name
                String name;
                expect_name(input, name);
                verify(name == element.name, "STag %s != ETag %s", element.name, name);
                skip_space(input);
                verify(input.read() == '>', "Expected '>'");
                break;
            } else
                input.unread(s), input.unread(b);

            // check if this is a tag
            if (input.peek() == '<') {  // element
                // add tag to the object
                auto& e = element.el.emplace_back();

                process_element(input, e);
            } else {
                // copy the data
                while (!input.end() && input.peek()!='<') {

                    // check the escape sequences
                    if (input.peek() == '&') {
                        process_entity(input, element.content);
                    } else {
                        element.content.append(1, input.read());
                    }
                }

                // strip whitespace
                // needed better, do the algorithm, that remembers the last non space character
                auto pos = element.content.size();
                for (auto it = element.content.rbegin(); it != element.content.rend(); ++it) {
                    if (!xml_s(*it))
                        break;
                    --pos;
                }
                element.content.erase(pos);
            }
        }
    }


    template <class input_T, class processor_T>
    void process_content_new(input_T& input, processor_T& proc)
    {
        if (proc.is_element_empty())
            return;

        skip_space(input);


        for (;;) {


            // check for the end tag
            auto b = input.read(), s = input.read();
            if (b == '<' && s == '/') {
                // check the tag name
                String name;
                expect_name(input, name);
                verify(name == proc.element_name(), "STag %s != ETag %s", proc.element_name(), name);
                skip_space(input);
                if (input.read() != '>')
                    throw std::runtime_error("Expected '>'");
                proc.element_close(name);
                break;
            } else
                input.unread(s), input.unread(b);

            // check if this is a tag
            if (input.peek() == '<') {  // element
                process_element_new(input, proc);
            } else {
                // copy the data
                String temp_content;  // temporary storage for content data

                while (!input.end() && input.peek()!='<') {

                    // check the escape sequences
                    if (input.peek() == '&') {
                        process_entity(input, temp_content);
                    } else {
                        temp_content.append(1, input.read());
                    }
                }

                proc.content(temp_content);
            }
        }

    }


    template <class input_T>
    void process_misc(input_T& input)
    {
        skip_space(input);
    }

    template <class input_T>
    void skip_space(input_T& input)
    {
        // skip whitespace
        while (!input.end() && xml_s(input.peek()))
            input.read();

        // skip comments
        if (xml_keyword(input, "<!--")) {

            for (; !input.end(); input.read())
                if (xml_keyword(input, "-->"))
                    break;

            skip_space(input);
        }
    }


private:
    template <class input_T>
    void expect_eq(input_T& input)
    {
        skip_space(input);

        if (input.read() != '=')
            throw std::runtime_error("Expected '='");
    }


    template <class input_T>
    String& expect_quoted_string(input_T& input, String& id)
    {
        skip_space(input);

        verify (!input.end(), "Unexpected end of data");

        auto ch = input.read();
        verify(ch == '\'' || ch == '\"', "Unexpected character: '%c'", ch);

        id.clear();

        for (;;) {
            verify (!input.end(), "Unexpected end of data");

            // Check for the end of string condition
            ch = input.peek();
            if (ch == '\'' || ch == '\"') {
                input.read();
                break;
            }

            if (ch == '&') {
                process_entity(input, id);
            } else {
                id.append(1, ch);
                input.read();
            }

        }

        //while (input.peek() != '\'' && input.peek() != '\"') {
        //    verify(!input.end(), "Quoted string unexpectedly terminated");
        //    if (input.peek() == '&')
        //        process_entity(input, id);
        //    else
        //        id.append(1, input.read());
        //}
        //input.read();  // skip the closing quotes
        return id;
    }

    template <class input_T>
    String& expect_name(input_T& input, String& name)
    {
        skip_space(input);

        // first has to be <letter>
        verify(xml_letter(input.peek()) || input.peek() == ':' || input.peek() == '_', "Unexpected character: '%c'", input.peek());

        name.clear();
        name.append(1, input.read());

        while (!input.end() && xml_name_char(input.peek()))
            name.append(1, input.read());

        return name;
    }

    template <class input_T>
    bool process_entity(input_T& input, String& text)
    {
        // read until symbol ';'
        String esc;

        input.read();  // skip &

        while (!input.end() && input.peek() != ';') {
            // needed to check valid characters in entity
            verify(input.peek() != '&' && input.peek() != '<' && input.peek() != ' ', "Invalid character in entity: '%s'", input.peek());
            esc.append(1, input.read());
        }

        verify(!input.end(), "Invalid entity - %s", esc);

        input.read();  // skip ;


        // check for the entities (defined and predefined) - TODO
        if (esc == "lt")
            text.append(1, '<');
        else if (esc == "gt")
            text.append(1, '>');
        else if (esc == "amp")
            text.append(1, '&');
        else if (esc == "apos")
            text.append(1, '\'');
        else if (esc == "quot")
            text.append(1, '\"');
        else if (esc[0] == '#') {  // numeric value
            typename String::iterator e = esc.begin();
            ++e;  // skip #
            bool hex = bool(*e=='x'||*e=='X');
            if (hex)
                ++e;

            char c;
            verify(parse_num_radix(e, esc.end(), c, hex ? 16 : 10), "Bad number: %d", esc);
            text.append(1, c);
        }

        return true;
    }


private:
    template <class input_T>
    String& expect_keyword(input_T& input, String& keyword)
    {
        skip_space(input);

        verify(isalpha(input.peek()), "Character expected, but found - %c", input.peek());

        keyword.clear();

        while (!input.end() && isalpha(input.peek()))
            keyword.append(1, input.read());

        return keyword;
    }

private:
    bool xml_s(Char ch) const noexcept
    {
        auto c = (int)ch;
        return c==0x09 || c==0x20 || c==0x0A || c==0x0D;
    }

    bool xml_letter(Char c2) const noexcept
    {
        auto c = (int)c2;
        //return ( (*c>=0x61&&*c<=0x7A) || (*c>=0xC0&&*c<=0xD6) || (*c>=0xD8&&*c<=0xF6) || (*c>=0xF8) )? true : false ;
        return (c>=0x41&&c<=0x5A)||(c>=0x61&&c<=0x7A)||(c>=0xC0&&c<=0xD6)||(c>=0xD8&&c<=0xF6)||(c>=0xF8&&c<=0xFF);
    }


    bool xml_digit(Char ch) const noexcept
    {
        auto c = (int)ch;
        return c >= 0x30 && c <= 0x39;
    }

    bool xml_extender(Char ch) const noexcept
    {
        auto c = (int)ch;
        return c == 0xB7;
    }

    bool xml_combining_char(Char) const noexcept
    {
        return false;
    }

    bool xml_name_char(Char c) const noexcept
    {
        return xml_letter(c) || xml_digit(c) || c=='_' || c==':' || c=='.' || c=='-' || xml_combining_char(c) || xml_extender(c);
    }



    template <class input_T>
    bool xml_keyword(input_T& input, const String& sec_name)
    {
        Char buffer[20];
        size_t idx = 0;

        for (const auto& c: sec_name) {
            if (input.end() || tolower(buffer[idx++] = input.read()) != tolower(c)) {
                input.unread(buffer, idx);
                return false;
            }
        }
        return true;
    }




private:  // XML generation routines
    void generate_space(size_t level, String& text)
    {
        for (size_t i = 0; i < level; i++)
            text += '\t';
    }

    template <class xml_type>
    void generate_prolog(const xml_type& obj, String& ttext)
    {
        std::stringstream text;

        if (!obj.prolog.version.empty()) {
            text << "<?xml ";
            text << "version=\"" << obj.prolog.version << '\"';

            if (!obj.prolog.encoding.empty())
                text << " encoding=\"" << obj.prolog.encoding << '\"';

            if (!obj.prolog.standalone.empty())
                text << " standalone=\"" << obj.prolog.standalone << '\"';

            text << "?>\n";
        }

        if (!obj.prolog.doctype.name.empty()) {
            text << xml_doctype_begin << ' ' << obj.prolog.doctype.name << ">\n";
        }

        ttext = text.str();
    }


    static void generate_entity(std::basic_stringstream<Char>& text, Char c)
    {
        if (c == '<')
            text << "&lt;";
        else if (c == '>')
            text << "&gt;";
        else if (c == '&')
            text << "&amp;";
        else if (c == '\'')
            text << "&apos;";
        else if (c == '\"')
            text << "&quot;";
        else if (c > 0x7E || c < 0x20) {
            text << "&#x";
            text << format_hex_digit(c / 16, 'A');
            text << format_hex_digit(c, 'A');
            text << ';';
        } else
            text << c;
    }

    void generate_entity_string(std::basic_stringstream<Char>& text, const String& str)
    {
        for (auto& s: str)
            generate_entity(text, s);
    }

    template <class element_T, class output_T>
    void generate_element(const element_T& e, int& tab_level, output_T& text)
    {
        generate_space(tab_level, text);

        text << '<' << e.name;


        // generate attributes
        for (auto att = e.atts.begin(), att_end = e.atts.end(); att != att_end; ++att) {
            text << ' ' << att->name << "=\"";
            generate_entity_string(text, att->value);
            text << '\"';
        }

        if (e.empty || (e.el.empty() && e.content.empty()))
            text << "/>";
        else {
            text << '>';

            tab_level++;

            if (!e.content.empty()) {
                generate_entity_string(text, e.content);
            }

            if (!e.el.empty()) {
                text << '\n';
                for (auto it = e.el.begin(), it_end = e.el.end(); it != it_end; ++it) {
                    generate_element(*it, tab_level, text);
                }

                generate_space(tab_level-1, text);
            }

            tab_level--;
            text << "</" << e.name << '>';
        }

        text << '\n';
    }

};



//
// Xml Builder - helps building xml documents
//

struct XmlBuilder {
public:
    // types
    using ErrorPolicy = AErrorPolicy;

    typedef std::stack<String> tag_stack_type;

    XmlBuilder() noexcept {}
    XmlBuilder(String& xml) { begin(xml); }

    ~XmlBuilder() { end(); }

    void reset() noexcept { 
        *this = {};
    }


    void begin(String& xml)
    {
        reset();
        xml_ = &xml;
        *xml_ = "<?xml version=\"1.0\"?>\n";
    }

    void end()
    {
        // close the XML
        while (!tag_stack_.empty())
            close();
    }

    // adds attribute the the current tag
    void att(const String& name, const String& value)
    {
        if (!tag_open_)
            error_("tag is not open");

        *xml_ += ' ';
        *xml_ += name;
        *xml_ += '=';
        *xml_ += '\"';
        *xml_ += XmlParser::escape_string(value);
        *xml_ += '\"';

    }

    void att(const String& name, const int value)
    {
        String stemp;
        att(name, str_format(stemp, "%d", value));
    }

    void att(const String& name, double value)
    {
        String stemp;
        att(name, str_format(stemp, "%g", value));
    }

    // opens new tag
    void tag(const String& name, bool empty = false)
    {
        close_tag();

        tag_empty_ = empty;
        tag_open_ = true;

        xml_->append(tag_stack_.size(), '\t');
        *xml_ += '<';
        *xml_ += name;


        if (!empty)
            tag_stack_.push(name);
    }

    // closes current tag (goes level up)
    void close()
    {
        close_tag();
        xml_->append(tag_stack_.size()-1, '\t');
        str_format_append(*xml_, "</%s>\n", tag_stack_.top());
        tag_stack_.pop();
    }

    void close_tag()
    {
        if (tag_open_) {
            if (tag_empty_)
                *xml_ += "/>\n";
            else
                *xml_ += ">\n";
            tag_open_ = false;
        }
    }

    void body(const String& data)
    {
        close_tag();
        *xml_ += data;
    }

private:
    String* xml_{};
    bool tag_open_{}, tag_empty_{};
    tag_stack_type tag_stack_;
    ErrorPolicy error_;
};

inline
Xml parseXml(StrView s) {
    XmlParser parser;
    Xml xml;
    parser.parse(String{s}, xml);  // TODO: remove String{} conversion
    return xml;
}

inline 
Xml loadXml(StrView filename) {
    XmlParser parser;
    Xml xml;
    parser.load(String{filename}, xml);
    return xml;
}

}  // namespace apl

