/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../../common/main.hpp"

#include <apeal/file.hpp>

using namespace apl ;

inline void dumpFilenameInfo(const AString &in)
{
	println("in: '%s'", in) ;

	AFileSystem fs ;

	auto [name,ext] = fs.splitExt ( in ) ;

	printtab ( ' ', 4 ) ;
	println ( "name.ext: '%s' . '%s'", name, ext ) ;

	auto [dir,base] = fs.splitPath ( in ) ;
	printtab ( ' ', 4 ) ;
	println ( "dir/base: '%s' / '%s'", dir, base ) ;

	println() ;
}

const char *testData[] = {
	"",
	"filename",
	".hidden_file",
	"..hidden_ext",
	"../up",
	"filename.ext",
	"/.root_hidden_file",
	"/path/to/.hidden_file",
	"/path/to/.hidden_file.with.ext",
	"/path/to/dir.ext/",
} ;


void body ( int argc, char *argv[] )
{
	for ( auto &t: testData ) {
		dumpFilenameInfo(t) ;
	}
}



#include "../../common/main.cpp"



