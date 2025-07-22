/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>
#include <vector>
#include <string>
#include <map>


#include <apeal/signal.hpp>


struct ASampleArg {
	ASampleArg(const int &v):value(v){}
	ASampleArg():value(0){}
	int value ;
} ;


class ASampleSlot {
public :
	void ASlot ( const ASampleArg &sign )
	{
		std::cout << "<ASampleArg>(" << sign.value << ")" << std::endl ;
	}

	void ASlot ( const int &s )
	{
		std::cout << "<int>(" << s << ")" << std::endl ;
	}

	void ASlot()
	{
		std::cout << "<void>()" << std::endl ;
	}
} ;




int main()
{
	ASampleSlot slot ;


	// invoking integer signal
	{
		ASignal<int> sig ;
		sig.connect(slot) ;
		sig.connect(slot) ;
		sig(4) ;
	}


	// invoking sample argument type
	{
		ASignal<ASampleArg> sig ;

		sig.connect(slot) ;
		sig(5) ;
	}

	// invoking void version
	{
		ASignal<> sig ;
		sig.connect(slot) ;
		sig() ;
	}

	return 0 ;
}

