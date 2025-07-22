/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "assert.hpp"
#include "parse/split.hpp"
#include "parse/bool.hpp"
#include "parse/int.hpp"
#include "parse/float.hpp"
#include "console.hpp"

#include <unordered_map>
#include <functional>
#include <memory>

// TODO: finish switching to string_view

namespace apl {


//
// Setting up options
// 

using CmdlineOptionAction = std::function<void(StrView)>;
using CmdlineFlagOptionAction = std::function<void()>;

// Parsing of the command line
struct CmdlineArgParams {
    String orig;  // original string supplied for error reporting
    StrView name, value;
    bool op{};  // option or argument
    bool sh{};  // is short option (-o) or long (--option)
};


struct CmdlineOptionParam {
    StrView name;
    Char salt;  // Short version (one character)
    StrView dup;  // Duplicate
    CmdlineOptionAction action;
    String *sbind;
    int* ibind;
    bool* bbind;
    double* dbind;
    float* fbind;
    StrView desc;  // option description
    StrView arg;  // argument/parameter name
    StrView def;  // default value. TODO: implement
};

struct CmdlineFlagParam {
    StrView name;
    Char salt;  // Short version (one character)
    StrView dup;  // Duplicate
    CmdlineFlagOptionAction action;
    bool *bind;
    bool def;  // default value
    StrView desc;  // option description
    StrView arg;  // argument/parameter name
};



//
// Options
//

struct CmdlineOptionSetup {
    String name;  // Option name
    Char salt{};  // Short alternative
    String dup{};  // Duplicate option name
    bool sh{};  // Short or long option
    CmdlineOptionSetup *duplicate{}; // for duplicate option it's the pointer to the copy
    bool required{};
    bool isflag{};  // no parameters required

    auto& setRequired(bool f = true) {
        required = f;
        return *this;
    }

    // Descriptive information
    String desc;  // Option description
    String paramName;  // parameter name (for informational purposes only)
    String def;

    // Action
    CmdlineOptionAction action{};
    CmdlineFlagOptionAction flagAction{};

    // Bindings
    // TODO: do variant
    String *stringBinding{};  // Option value
    int *intBinding{};
    double *doubleBinding{};
    float *floatBinding{};
    bool *boolBinding{};


    // runtime parsing information
    bool supplied{};
};








class CommandLine {

public:
    using StringList = std::vector<String>;
    using Arg = CmdlineArgParams;


    //
    // Setup
    //
public:
    // Command line parsing parameters
    bool allOptionsRequired{};
    bool allowUnknownOptions{};
    std::pair<Index, Index> argsRange{};  // 0, 0 - no limit


private:
    using OptionSetupPtr = std::shared_ptr<CmdlineOptionSetup>;
    auto makeOptionSetupPtr() noexcept { return std::make_shared<CmdlineOptionSetup>(); }

    std::unordered_map<StrView, OptionSetupPtr> optionsByName_;
    std::vector<OptionSetupPtr> optionsByOrder_;  // Options in supplied order

    String *argStringBinding{};
    StringList *argVectorBinding{};


public:
    //
    // Result
    //
    std::vector<Arg> args;
    std::vector<Index> argIdx;



    //
    // Setup
    //


    //
    // Setup up arguments
    //

    // Binds one required argument
    void bindRequiredArgument(String *binding) {
        argsRange = {1, 1};
        argStringBinding = binding;
    }

    // Binds optional argument
    void bindOptionalArgument(String *binding) {
        argsRange = {0, 1};
        argStringBinding = binding;
    }


    // Binds number of arguments. 
    void bindArguments(StringList *binding, int minArgs = 0, int maxArgs = 0) {
        argsRange = {minArgs, maxArgs};
        argVectorBinding = binding;
    }




private:


    // Guess option type by its name
    OptionSetupPtr assignOptionName(StrView name, StrView desc = {}, StrView paramName = {}) {
        auto opt = makeOptionSetupPtr();

        verify(!name.empty(), "Option name cannot be empty");

        opt->desc = desc;
        opt->paramName = paramName;

        if (name.front() == '-') {
            if (name.size() == 2) {
                verify(name[1] != '-', "Invalid option name: %s", name);
                opt->name = name.substr(1);
                opt->sh = true;
            } else {
                if (name[1] == '-') {
                    opt->name = name.substr(2);
                } else {
                    opt->name = name.substr(1);
                }
                opt->sh = false;
            }
        } else {
            opt->name = name;
            opt->sh = name.size() == 1;
        }
        return opt;
    }

    CmdlineOptionSetup &addOption(OptionSetupPtr &opt) {
        verify(!optionsByName_.contains(opt->name), "Duplicate setup for option: %s", opt->name);
        optionsByName_[opt->name] = opt;
        optionsByOrder_.push_back(opt);

        if (opt->salt != 0) dup(opt->salt, opt->name);
        if (!opt->dup.empty()) dup(opt->dup, opt->name);

        return *opt.get();
    }


public:
    //
    // Adding options
    //
    void add(const CmdlineOptionParam& p) {
        auto opt = assignOptionName(p.name, p.desc, p.arg);
        opt->salt = p.salt;
        opt->dup = p.dup;
        opt->action = p.action;
        opt->stringBinding = p.sbind;
        opt->intBinding = p.ibind;
        opt->boolBinding = p.bbind;
        opt->doubleBinding = p.dbind;
        opt->floatBinding = p.fbind;
        opt->def = p.def;
        addOption(opt);
    }

    void addFlag(const CmdlineFlagParam& p) {
        auto opt = assignOptionName(p.name, p.desc, p.arg);
        opt->salt = p.salt;
        opt->dup = p.dup;
        opt->isflag = true;
        opt->flagAction = p.action;
        opt->boolBinding = p.bind;
        opt->def = p.def ? "1" : "0";
        addOption(opt);
    }


    //
    // Duplicate options. Usually to supply short option variation:
    // like cmdline.dup("-h","--help") or cmdline.dup("h","help") or cmdline.('h',"help") ;
    void dup(StrView name, StrView dupname) {
        auto dupIt = optionsByName_.find(dupname);
        verify(dupIt != optionsByName_.end(), "Duplicate option '%s' not found for '%s'", dupname, name);

        auto opt = assignOptionName(name);
        opt->duplicate = dupIt->second.get();
        addOption(opt);
    };

    void dup(Char dupchar, StrView dupname) {
        StrView name{&dupchar, 1};
        return dup(name, dupname);
    };



    //
    // Parsing and process
    //
private:
    bool stopped_ = false;
public:
    auto& stop() noexcept { stopped_ = true; return *this; }
    bool stopped() const { return stopped_; }

private:
    int processArgument(const Arg &a)
    {
        // Handle the arguments
        //if ( argsRange.first != 0 ) {
        //	verify ( argIdx.size() >= argsRange.first, "Argument is missing for option: %s", a.orig ) ;
        //}

        if (argsRange.second != 0) {
            verify(std::ssize(argIdx) < argsRange.second, "Extra command line arguments: %s", a.orig);
        }

        if (argStringBinding)
            *argStringBinding = a.value;

        return 0;
    }

    // Walks over the duplicates
    CmdlineOptionSetup* findDupRoot(const OptionSetupPtr &dup) const {
        auto opt = dup.get();
        while (opt->duplicate)
            opt = opt->duplicate;
        return opt;
    }

    CmdlineOptionSetup& optByName(StrView name) const {
        auto it = optionsByName_.find(name);
        verify(it != optionsByName_.end(), "Cannot find option: %s", name);
        return *findDupRoot(it->second);
    }

    int processOptionData(CmdlineOptionSetup &opt, StrView value)
    {
        if (opt.action)
            opt.action(value);

        if (opt.isflag && opt.flagAction) 
            opt.flagAction();

        if (opt.stringBinding)	
            *opt.stringBinding = value;

        if (opt.intBinding)	    
            *opt.intBinding = parseInt(value);

        if (opt.boolBinding) {
            if (opt.isflag && value.empty())
                *opt.boolBinding = true;
            else
                *opt.boolBinding = parseBool(value);
        }

        if (opt.doubleBinding)	
            *opt.doubleBinding = parseDouble(value);

        if (opt.floatBinding)	
            *opt.floatBinding = parseFloat(value);

        return 0;
    }

    int processOption(const Arg& a)
    {
        // If the user hasn't setup anything, just exit
        if (optionsByName_.empty())
            return 0;

        auto &sopt = optByName(a.name);
        sopt.supplied = true;
        processOptionData(sopt, a.value);

        return 0;
    }

    int checkShortOption(const Arg& a)
    {
        // if the user hasn't setup anything then treat '-o a' as solo option and argument 'a' 
        // and '-oa' as option 'o' with parameter 'a'
        if (optionsByName_.empty())
            return 0;

        auto &sopt = optByName(a.name);
        sopt.supplied = true;
        if (sopt.isflag) {
            processOptionData(sopt, "1");
            return 0;
        }

        return 1;  // expecting argument
    }


public:
    //
    // Parsing and processing the command line
    //
    int process(int argc, char *argv[])
    {
        args.clear();
        argIdx.clear();

        bool optionsSection = false;
        bool expectingOptValue = false;
        Arg expectingOpt;

        for (int iarg = 1; iarg < argc; ++iarg) {
            auto arg = StrView(argv[iarg]);  // for convenience

            if (expectingOptValue) {
                expectingOptValue = false;

                //if ( arg.front() != '-' ) {
                //	expectingOpt.value = arg ;
                //	expectingOpt.orig += ' ' + arg ;
                //}
                expectingOpt.value = arg;
                expectingOpt.orig += ' ';
                expectingOpt.orig += arg;

                processOption(expectingOpt);
                args.emplace_back(expectingOpt);

                //if ( arg.front() != '-' )
                //	continue ;
                continue;
            }

            if (arg.front() == '-') {  // Option
                if (arg.size() == 1) {
                    optionsSection = !optionsSection;
                    continue;
                }

                Arg o{.orig = String{arg}};
                o.op = true;

                if (arg[1] == '-') {  // Long option
                    auto [name, value] = str_split_trim(arg.substr(2), '=');
                    o.name = name;
                    o.value = value;
                    processOption(o);
                    args.emplace_back(o);
                } else {  // Short option
                    o.sh = true;
                    o.name = arg.substr(1, 1);
                    o.value = arg.substr(2);
                    if (!o.value.empty()) {   // if value is provided, treat it as long option
                        processOption(o);
                        args.emplace_back(o);
                    } else {
                        // Check if the value is required
                        if (checkShortOption(o) == 0) {
                            args.emplace_back(o);
                        } else {  // expecting argument
                            expectingOptValue = true;
                            expectingOpt = std::move(o);
                        }
                    }
                }

            } else {  // Argument
                Arg a{.orig = String{arg}};
                a.orig = a.value = arg;
                processArgument(a);
                argIdx.push_back(args.size());
                args.emplace_back(a);
            }

            if (stopped_)
                return 1;
        }

        verify(!expectingOptValue, "Expecting command line option value!");

        // Check number of arguments, maximum number of arguments were checked
        // check for the minimum
        verify(!argsRange.first || std::ssize(argIdx) >= argsRange.first, "Missing required command line arguments: %d", argsRange.first);

        if (argVectorBinding) {
            StringList list;
            for (auto i: argIdx)
                list.push_back(String{args[i].value});
            *argVectorBinding = list;
        }

        return 0;
    }


    //
    // Information
    //
    auto& print(StrView text) {
        Console::println(text);
        return *this;
    }

    auto& printout(StrView header, char indentChar=' ', int optIndent=2, int descIndent=18) {
        Console::println(header);

        auto makeOptionName = [](const OptionSetupPtr &opt) {
            auto name = fmt(opt->sh ? "-%s" : "--%s", opt->name);
            auto isflag = opt->duplicate ? opt->duplicate->isflag : opt->isflag;

            if (!isflag) {
                // See if the user submitted the parameter name
                auto sep = opt->sh ? ' ' : '=';
                const auto& paramName = opt->duplicate ? opt->duplicate->paramName : opt->paramName;

                name += fmt("%c<%s>", sep, paramName.empty() ? "arg" : paramName);
            }

            return name;
        };

        // Not efficient, but who cares
        for (auto& opt: optionsByOrder_) {

            // skip duplicates
            if (opt->duplicate)
                continue;

            // Build option name
            auto name = makeOptionName(opt);

            // Print option name
            Console::ch(indentChar, optIndent);
            Console::print("%s", name);

            // Build parameters information
            // TODO: add description or guess from the type

            // Search for all duplicates referring to this option
            for (auto& dup: optionsByOrder_) {
                if (dup->duplicate && opt.get() == findDupRoot(dup)) {
                   Console::print(", %s", makeOptionName(dup));
                }
            }

            Console::nl();

            // Print option description
            if (!opt->desc.empty()) {
                Console::ch(indentChar, descIndent);
                Console::println(opt->desc);
            }
        }
        return *this;
    }



};

}  // namespace apl
