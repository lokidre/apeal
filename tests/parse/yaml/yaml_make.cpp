/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <test_yaml.hpp>


#include <apeal/parse/yaml.hpp>
#include <apeal/parse/vector.hpp>
#include <apeal/parse/float.hpp>

#include <apeal/hash.hpp>

void body()
{
    using namespace apl;

    YamlBuilder yaml;

    verify(cmdlineArgC_ > 1, "Command line argument is missing");

    auto csvfile = cmdlineArgV_[1];

    auto csv = loadFile(csvfile);

    auto rows = parse_std_vector_view_trim(csv, '\n');
    for (auto&& row: rows) {
        auto fields = parse_std_vector_view_trim(row, '|');

        if (fields[0].empty())
            continue;

        if (fields[0].front() == '#')
            continue;

        auto field = [&](Index idx) {
            verify(idx < std::ssize(fields), "Field is missing: %s", row);
            return fields[idx];
        };

        using namespace apl::literals;

        switch (pshHash(fields[0])) {
        case "doc"_psh: yaml.doc(); break;
        case "com"_psh: yaml.comment(field(1)); break;

        case "o"_psh:
        case "obj"_psh:
            yaml.obj(field(1));
            break;

        case "m"_psh:
        case "map"_psh:
            yaml.map();
            break;

        case "seq"_psh:
            yaml.item(field(1));
            break;

        case "e"_psh:
        case "end"_psh:
            yaml.pop();
            break;

        case "s"_psh:
        case "str"_psh:
            yaml(field(1), field(2));
            break;

        case "i"_psh:
        case "int"_psh:
            yaml(field(1), parseInt(field(2)));
            break;

        case "f"_psh:
        case "float"_psh:
            yaml(field(1), parseReal<float>(field(2)));
            break;

        default:
            throwError("Unknown command: %s", fields[0]);
        }
    }

    con_(yaml.str());
}
