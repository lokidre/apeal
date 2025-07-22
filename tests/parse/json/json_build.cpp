/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/parse/json.hpp>
#include <apeal/parse/csv.hpp>
#include <apeal/file.hpp>
#include <apeal/parse/bool.hpp>

#include <string>

void body()
{
    using namespace apl;

    verify(cmdlineArgC_ > 1, "Argument missing");

    File file;
    String json;
    JsonBuilder::StringProcessor processor(json);
    JsonBuilder builder{processor};


    //std::string data;
    //ACsvParser csvParser;
    //std::vector<std::vector<std::string>> csv;
    //csvParser.load(cmdlineArgV_[1], csv);

    auto csv = loadCsv(cmdlineArgV_[1]);


    //builder.generateNewlines = false ;
    builder.generateIndents = true;
    builder.begin();

    for (auto&& row: csv) {
        switch (row[0][0]) {
        case 'o':  // object open
            builder.object(row[1]);
            break;
        case 'a': // array open
            builder.array(row.size() >= 2 ? row[1] : "");
            break;
        case 'c': // object close
            builder.close();
            break;
        case 's': // string value
            builder.value(row[1], row[2]);
            break;
        case 'i':  // integer value
            builder.value(row[1], stoi(row[2]));
            break;
        case 'I': {
            std::vector<int> values;
            for (auto rowIt = row.begin() + 1; rowIt != row.end(); ++rowIt) {
                values.push_back(stoi(*rowIt));
            }
            builder.intValues(values);
            break;
        }
        case 'n': // null value
            builder.null(row[1]);
            break;

        case 'b': {
            auto bval = parseBool(row[2], false);
            builder.boolean(row[1], bval);
            break;
        }
        default:
            break;
        }
    }

    builder.end();

    //builder.build(argv[1],json) ;

    if (cmdlineArgC_ >= 2)
        file.save(cmdlineArgV_[2], json);
    else
        std::cout << json;
}



