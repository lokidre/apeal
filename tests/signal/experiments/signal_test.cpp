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



class Slot1 {
public :
	Slot1(apl::ASignal<> &sig) : sig_(sig) {}

	void meth()
	{
		std::cout << "Slot1::meth():signal disconnect" << std::endl ;
		sig_.disconnect(this) ;
	}

private :
	apl::ASignal<> &sig_ ;
} ;

class Slot2 {
public :
	void meth()
	{
		std::cout << "Slot2::meth()" << std::endl ;
	}
} ;

class Slot3 {
public :
	void meth_int(int a)
	{
		std::cout << "Slot3::meth_int("<< a <<")" << std::endl ;
	}
} ;

class Slot4 {
	public :
	void meth(int v, int u)
	{
		std::cout << "Slot4::meth("<<v<<','<<u<<")" << std::endl ;
	}
} ;




int main()
{
	using namespace apl ;

	ASignal<> sig ;
	ASignal<int> sig_int ;
	ASignal<int,int> sig_par ;

	Slot1 slot1(sig) ;
	Slot2 slot2 ;
	Slot3 slot3 ;
	Slot4 slot4 ;


	sig.connect(&slot1,&Slot1::meth) ;
	sig.connect(&slot2,&Slot2::meth) ;

	sig() ;
	sig() ;

	sig_int.connect(&slot3,&Slot3::meth_int) ;
	sig_int(3) ;

	sig_par.connect(&slot4,&Slot4::meth) ;
	sig_par(5,8) ;

	return 0 ;
}


