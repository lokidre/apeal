/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once


namespace apl {
namespace exception_policy {

class AExceptionPolicy {
public :
	bool operator ( const char *error_string )
	{
		throw std::runtime_error(error_string) ;
	}
} ;

}
}


