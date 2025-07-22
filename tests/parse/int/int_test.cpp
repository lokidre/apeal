/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../../common/test_main.h"

#include <apeal/parse/int.hpp>
#include <apeal/assert.hpp>

#include <typeinfo>

template <class S, class T>
struct TestDataset {
    using String = S;
    using Value = T;

    S s;
    T n;
};


TestDataset<std::string,int> testDataString[] = {
    { "1", 1 },
    { "0001", 1 },
    { "100", 100 },
    { "0", 0 },
} ;

TestDataset<const char *,int> testDataChar[] = {
    { "1", 1 },
    { "0001", 1 },
    { "100", 100 },
    { "0", 0 },

} ;

TestDataset<const char *,int64_t> testDataLarge[] = {
//    { "1", 1 },
//    { "0001", 1 },
//    { "100", 100 },
//    { "0", 0 },
    { "9223372036854775807", 0x7FFFFFFFFFFFFFFFULL },

} ;



template <class Dataset>
void processTestData ( const Dataset &dataset )
{
    using namespace apl ;

    //using ElType = std::remove_all_extents<decltype(dataset)>::type;
    using ElType = std::remove_reference<decltype(dataset[0])>::type;

    println ( "String: '%s', Int: '%s'", typeid(dataset->s).name(), typeid(dataset->n).name() ) ;

    for ( auto &t: dataset ) {
        typename ElType::Value r;

        parseInt ( t.s, r ) ;

        verify ( r == t.n, "Parse Failed: %s != %d", t.s, r ) ;

        println ( "%d == %d", t.s, t.n ) ;
    }
    println() ;
}

void body()
{
    //processTestData ( testDataString ) ;
    //processTestData ( testDataChar ) ;
    processTestData ( testDataLarge ) ;
}

