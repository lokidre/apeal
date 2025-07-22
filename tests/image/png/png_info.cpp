/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../../common/main.hpp"

#include <apeal/image/libpng.hpp>

void body()
{
	AString imageFile ;

	ACmdLine cmdline(argc,argv) ;

	cmdline.requireArgument ( &imageFile ) ;

	cmdline.process() ;

	auto image = ALibPng::loadImage ( imageFile ) ;

	println ( "size: %d", image.size ) ;
}

#include "../../common/main.cpp"
