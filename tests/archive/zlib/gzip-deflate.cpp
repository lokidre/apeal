/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>


#include <apeal/zlib.hpp>


using namespace std ;
using namespace apl ;

int main ( int argc, char *argv[] )
{

    int retcode = 1 ;

    try {
        AZLib zlib ;

        if ( argc < 3 )
            throw runtime_error("usage: gzip-deflate <input-file> <gzip-file>") ;

        zlib.deflateGzipFile ( argv[1], argv[2] ) ;

        retcode = 0 ;
    }
    catch(exception &e) {
        cerr << e.what() << endl ;
    }
    catch(...) {
        cerr << "exception!" << endl ;
    }

    return retcode ;

}
