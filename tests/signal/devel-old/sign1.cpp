/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
// basic idea

#include <iostream>


template <class Slot>
class Signal {
public :
	Signal ( Slot *s, void (Slot::*m)()) : slotPtr_(s), methPtr_(m) {}

private :
	Slot *slotPtr_ ;
	void (Slot::*methPtr_)() ;

public :
	void operator()()
	{
		(slotPtr_->*methPtr_)() ;
	}
} ;


class Slot {
public :
	void meth()
	{
		std::cout << "slot_class::slot_meth()" << std::endl ;
	}
} ;




int main()
{
	Slot slot ;

	Signal<Slot> sig(&slot,&Slot::meth) ;

	sig() ;

	return 0 ;
}

