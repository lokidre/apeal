/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#if HEADER_Description

Testing for flags

    --flag
    --flag=1
    --flag=false
    --flag=YES

#endif


#include <test_main.h>

#include <test_cmdline.hpp>

#include <apeal/cmdline.hpp>


//static const char* helpHeader = R"(
constexpr static auto helpHeader = std::string_view{R"(
Sample command line processing utility.
Copyright (C) 2020 Andrei Mirzoyan.
Do whatever you want with this code.
Usage: test <options> [args...]

Options:
)"}.substr(1);


void body() {
    apl::CommandLine cmdline;

    cmdline.addFlag({
        .name = "help",
        .action = [&] { cmdline.printout(helpHeader).stop(); },
        .desc = "Print this message",
    });
    cmdline.dup("h", "help");

    double dval{};
    int ival{};
    std::string sval;
    bool bval{};
    bool fval{};

    cmdline.add({.name = "double", .dbind = &dval, .desc = "Double value", .arg = "<ii.ii>"});
    cmdline.dup("d", "double");

    cmdline.add({.name = "int", .ibind = &ival, .desc = "Int value", .arg = "i"});
    cmdline.dup("i", "int");

    cmdline.add({.name = "string", .sbind = &sval, .desc = "String value", .arg = "<string>"});
    cmdline.dup("s", "string");

    cmdline.add({.name = "bool", .bbind = &bval, .desc = "Bool value"});
    cmdline.dup("b", "bool");

    cmdline.addFlag({.name = "flag", .bind = &fval, .desc = "Flag value"});
    cmdline.dup("f", "flag");


    apl::CommandLine::StringList params;
    cmdline.bindArguments(&params);

    cmdline.process(cmdlineArgC_, cmdlineArgV_);

    if (cmdline.stopped())
        return;


    // Printout command line help
    cmdline.printout(helpHeader);


    con_.println("dval: %f", dval);
    con_.println("ival: %d", ival);
    con_.println("sval: %s", sval);
    con_.println("bval: %d", bval);
    con_.println("fval: %d", fval);

    con_.println("args: %s", params);
}
