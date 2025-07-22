/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/console.hpp>
#include <apeal/assert.hpp>
#include <apeal/debug.hpp>
#include <test_main.h>

template <class T>
struct FormatTestValue {
    std::string_view fmt;
    T val;
    std::string_view expected;
};

FormatTestValue<int> testDataCharInt[] = {
    { "%d", 0, "0" },

    { "%d", 5, "5" },
    { "%d", 10, "10" },
    { "%d", 12345678, "12345678" },
    { "%d", -12345678, "-12345678" },
    { "%d", -9, "-9" },


    { "%x", 0, "0" },
    { "%x", 5, "5" },
    { "%x", 10, "a" },
    { "%x", 12345678, "bc614e" },
    { "%x", -12345678, "-bc614e" },
    { "%x", -9, "-9" },

    { "%10d",      1234, "      1234" },
    { "%2d",       1234, "1234" },
    { "%10d",     -1234, "     -1234" },
    { "%010d",     1234, "0000001234" },
    { "%010d",    -1234, "-000001234" },
    { "%+10d",     1234, "     +1234" },
    { "% 010d",    1234, " 000001234" },
    { "% #010X", 0x1234, " 0X0001234" },
};


/*
using namespace apl ;
using namespace std ;

template <class T>
void { const char *fmt, const T &val, const string &expect = "" ) {
    auto s = sformat ( fmt, val },
    i{ !expect.empty() && s != expect )
        throw runtime_error ( "!Error!!! " + s + " (" + fmt + ") != " + expect },
    cout << "(" << fmt << ", " << val << ") => |" << s << "|" << endl ;
}
*/

void body()
{
    using namespace apl;


    for (auto& t: testDataCharInt) {
        auto text = sformat(t.fmt, t.val);

        if (text != t.expected) {
            throwError("Error! fmt(%s, %z): '%s' != '%s'", t.fmt, t.val, text, t.expected);
        }
        //i{ s != t.expected ) {
        //debug_trap_assert( s == t.expected },
        //}

        println(R"(fmt("%s", %z): '%s')", t.fmt, t.val, text);
    }

}






