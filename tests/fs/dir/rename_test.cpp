/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../../common/test_main.h"

#include <apeal/file.hpp>

void body() {
    using namespace apl;
    using namespace std;

    using FS = apl::AFileSystem;

    FS fs;

    auto currentdir = FS::getCurrentDirectory();

    int oldpathnum = 1;
    int newpathnum = 2;


    if (cmdlineArgC_ < 1 || (cmdlineArgC_ + 1) % 2 != 0) {
        cout << "you have to write 2 names old and new " << endl;
    } else {
        for (int iarg = 1; iarg < cmdlineArgC_; iarg += 2) {
            fs.rename(currentdir + "/" + cmdlineArgV_[oldpathnum], currentdir + "/" + cmdlineArgV_[newpathnum]);
            oldpathnum = oldpathnum + 2;
            newpathnum = newpathnum + 2;
        }
    }

}


