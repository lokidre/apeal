/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../../common/test_main.h"

#include <iostream>
#include <string>

#include <apeal/file.hpp>
#include <apeal/parse.hpp>


template <class T>
void printUriItem(std::string_view name, T value)
{
    if (apl::isFalse(value))
        return;

    con_("    %8s: %s", name, value);
}

//void uri_gen_item ( std::string_view name, std::string_view value )
//{
//    if ( !value.empty() )
//        std::cout << name << ":\t" << value << std::endl ;
//}


void printUriDesc(const apl::Url& uri)
{
    printUriItem("scheme", uri.scheme);
    printUriItem("userinfo", uri.userinfo);
    printUriItem("host_name", uri.host_name);
    printUriItem("host_address", uri.host_address);
    printUriItem("port", uri.port);
    printUriItem("reg_name", uri.reg_name);
    printUriItem("net_path", uri.net_path);
    printUriItem("abs_path", uri.abs_path);
    printUriItem("rel_path", uri.rel_path);
    printUriItem("opaque_part", uri.opaque_part);
    printUriItem("query", uri.query);

    if (!uri.params.empty()) {
        apl::printlns(4, "params:");
        for (auto&& param: uri.params) {
            apl::printlns(8, "%s: %s", param.first, param.second);
        }
    }

    //for ( auto p = uri.params.begin() ; p != uri.params.end() ; ++p ) {
    //    std::cout << "\t\t" <<  p->first << " = " << p->second ;
    //}
    //if ( !uri.params.empty() )
    //    std::cout << std::endl ;

    printUriItem("fragment", uri.fragment);
}


void body()
{
    std::string filename;

    if (cmdlineArgC_ <= 1)
        filename = "tests.uri";
    else
        filename = cmdlineVector_[1];

    auto&& data = apl::loadFile(filename);
    auto&& list = apl::parse_std_vector_view_trim(data, '\n');
    for (auto&& str: list) {
        if (str.empty())
            continue;
        con_("%s", str);
        auto&& uri = apl::UrlParser::parse(str);
        printUriDesc(uri);
    }

}

