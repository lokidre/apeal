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

void body()
{
	using namespace apl ;

	using FS = apl::AFileSystem ;

	FS fs ;

//	auto saved = FS::pwd() ;
//	println ( "Dir: %s", saved ) ;

    //String val = "..";
	//fs.tmpdir(&val) ;

    auto tmpdir = FS::tmpdir();
    println("Tmp Dir: %s", tmpdir);


//	println ( "Dir: %s", FS::pwd() ) ;

//	fs.cd(saved) ;

//	println ( "Dir: %s", FS::pwd() ) ;
}


