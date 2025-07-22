/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/signal.cxx>

#include <iostream>


struct SignalParam {
	int value ;
} ;


class Slot {
public :
	void meth(int &i, SignalParam &p)
	{
		std::cout << "Slot::meth(" << i << "," << p.value << "):signal" << std::endl ;
		i = 100, p.value = 200 ;
	}
} ;


int main()
{
	using namespace apl ;
	using namespace std ;

	ASignal<int &,SignalParam &> sig_ref ;

	SignalParam p ;
	int i = 50 ;

	p.value = 10 ;


	Slot s ;

	sig_ref.connect(&s,&Slot::meth) ;
	sig_ref(i,p) ;

	cout << "int: " << i << "  val: " << p.value << endl ;

	return 0 ;
}


