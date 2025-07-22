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

int main ( int argc, char *argv[] )
{
    using namespace std ;
    using namespace apl ;

    int ret = 1 ;

    try {

        if ( argc < 3 )
            throw runtime_error("usage: gzip-inflate-feed <gzip-file> <output-file>") ;


        AZLib zlib ;
        AFile file ;
        AByteBuffer buffer ;
        buffer.resize(1000 * 1000) ;

        file.open ( argv[1], file.access_read ) ;

        zlib.inflateFileBegin ( argv[2], true ) ;


        for(;;) {

            int count = file.read_partial ( buffer.get(), buffer.size() ) ;
            if ( count == 0 )
                break ;
            zlib.inflateFileFeed ( buffer.get(), count ) ;
        }

        zlib.inflateFileEnd() ;

        ret = 0 ;
    }
    catch(exception &e) {
        cerr << e.what() << endl ;
    }
    catch(...) {
        cerr << "exception!" << endl ;
    }

    return ret ;

}
