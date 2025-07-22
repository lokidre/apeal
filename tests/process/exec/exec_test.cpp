/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>
#include <stdexcept>
#include <string>

#include <apeal/process.hpp>


void body()
{
	apl::String command = "/usr/bin/7z";
	apl::Process proc;

	proc.run(command);

		cout << cmd << " " << arg << flush ;
		proc.create(cmd,arg) ;
		cout << endl << "  pid: " << proc.pid() << flush ;
		proc.wait() ;
		cout << endl << "  exit: " << proc.exit_code() << endl ;
}
