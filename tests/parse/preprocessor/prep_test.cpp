/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
//#include <apeal/parse/input.hpp>

#include <apeal/parse/preprocessor.hpp>
//#include <apeal/file.hpp>


int main(int argc, char *argv[])
{
    int retcode = 1;

    try {
        auto filename = "include.ini";

        if (argc > 1)
            filename = argv[1];


        apl::APreprocessor prep;
        prep.search_dirs.push_back(".");
        auto output = prep.load(filename);

        apl::saveFile(apl::fmt("temp/%s.prep", filename), output);

        retcode = 0;
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "exception!" << std::endl;
    }

    return retcode;

}

