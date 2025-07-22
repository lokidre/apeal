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

#include <vector>

struct Custom {
    int a = 5;
};

template<class S, class F> //, class T>
void str_format_type(S& str, F& fmt, const F& fend, const Custom& v) {
    fmt = apl::str_format_collect_spec(fmt, fend); // skip the formatting parameters and the typespec
    str = apl::sformat("Custom: {%d}", v.a);
}


template<class T>
struct CustomT {
    T a = 3;
};

template<class S, class F, class T>
void str_format_type(S& str, F& fmt, const F& fend, const CustomT<T>& v) {
    fmt = apl::str_format_collect_spec(fmt, fend); // skip the formatting parameters and the typespec
    str = apl::sformat("CustomT: {%d}", v.a);
}


struct CustomDerived: public std::vector<int> {
};


template<class S, class F>
void str_format_type(S& str, F& fmt, const F& fend, const CustomDerived& v) {
    fmt = apl::str_format_collect_spec(fmt, fend); // skip the formatting parameters and the typespec
    str = apl::sformat("CustomTypedef: ");
}


using CustomVector = std::vector<int>;

namespace std {
// Must be declared in whatever namespace the class resides

template<class S, class F>
void str_format_type(S& str, F& fmt, const F& fend, const CustomVector& v) {
    fmt = apl::str_format_collect_spec(fmt, fend); // skip the formatting parameters and the typespec
    str = apl::sformat("CustomVector: ");
}

}



template<class T>
static void test(const char* fmt, const T& arg) {
    using namespace apl ;

    auto s = sformat(fmt, arg);

    println("fmt ( %z ): '%s'", fmt, s);
}


void body() {
    using namespace apl ;

    //test ( "%z", Custom() ) ;

    //test ( "%z", CustomT<long>() ) ;

    //test ( "%z", CustomDerived() ) ;

    test("%z", CustomVector());

}
