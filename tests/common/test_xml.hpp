/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "./test_console.hpp"

#include <apeal/parse/xml.hpp>

template <class S>
struct XmlDumper {
    using Xml = apl::Xml;

    apl::Index indent{};
    apl::Console& con_;


    XmlDumper(apl::Console& con) noexcept
        : con_{con}
    {
        con_.setIndent(4);
    }

    void dump(const Xml& xml)
    {
        con_.printiln(indent, "xml:");
        ++indent;
        dumpElement(xml.element);
        --indent;
    }

    void dumpElement(const Xml::Element& element) {
        con_.printiln(indent, "name: %s", element.name);

        dumpAttributes(element.atts);

        if (!element.content.empty())
            con_.printiln(indent, "content: %s", element.content);

        if (!element.el.empty()) {
            con_.printiln(indent, "elements:");

            for (auto&& el: element.el) {
                ++indent;
                con_.printiln(indent, "-");
                
                ++indent;
                dumpElement(el);
                --indent;

                --indent;
            }
        }
    }

    void dumpAttributes(const Xml::AttributeList& atts) {
        if (atts.empty())
            return;
        con_.printiln(indent, "attributes:");
        for (auto&& att: atts) {
            ++indent;
            con_.printiln(indent, "- name: %s", att.name);
            con_.printiln(indent, "  value: %s", att.value);
            --indent;
        }
    }

};


template <class Xml>
void dumpXml(const Xml& xml)
{
    XmlDumper<typename Xml::Str> dumper{con_};

    dumper.dump(xml);
}
