/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
// add argument
// add variable template arguments


#include <string>
#include <iostream>
#include <vector>


//class AVoid{} ;
//class ANull{} ;

template <class... Args>
class SlotItemBase {
public :
	virtual void invoke(Args...) = 0 ;
	virtual void *slot() const = 0 ;
} ;


template <class S, class... Args>
class SlotItem : public SlotItemBase<Args...> {
public :
	typedef void (S::*M)(Args...) ;

	S *slotPtr_ ;
	M methPtr_ ;

	SlotItem(S *slot, M method) : slotPtr_(slot), methPtr_(method) {}

protected :
	void invoke(Args... args) override { (slotPtr_->*methPtr_)(args...) ;  }  
	void *slot() const override {  return (void *)slotPtr_ ;  }
} ;


template <class... Args>
class Signal {

public :
	template <class S>
	void connect ( S *s, typename SlotItem<S,Args...>::M m )
	{
		slots_.push_back(new SlotItem<S,Args...>(s,m)) ;
	}

	template <class S>
	void disconnect ( S *slot )
	{
		for ( auto it = slots_.begin() ; it != slots_.end() ; ) {
			if ( (*it)->slot() == (void *)slot ) {
				delete *it ;
				it = slots_.erase(it) ;
			} else
				++it ;
		}
	}

	template <class S>
	void disconnect ( S *slot, typename SlotItem<S,Args...>::M m )
	{
		for ( auto it = slots_.begin() ; it != slots_.end() ; ) {
			if ( (*it)->slot() == (void *)slot ) {
				auto pimpl = (SlotItem<S,Args...> *)*it ;

				if ( pimpl->methPtr_ == m ) {
					delete *it ;
					it = slots_.erase(it) ;
				} else
					++it ;
			} else
				++it ;
		}
	}

	void invoke(Args... args)
	{
		for ( auto &slot : slots_ )
			slot->invoke(args...) ;
	}

	void operator()(Args... args) { invoke(args...) ;  }


protected :
	typedef std::vector<SlotItemBase<Args...> *> slot_list_type ;

	slot_list_type slots_ ;
} ;



class Slot1 {
public :
	void meth()
	{
		std::cout << "Slot1::meth()" << std::endl ;
	}
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
	void meth_int(int i)
	{
		std::cout << "int Slot3::meth_int(" << i << ")" << std::endl ;
	}
} ;

class Slot4 {
public :
	void meth(std::string v)
	{
		std::cout << "Slot4::meth(" << v << ")" << std::endl ;
	}
} ;



int main()
{
	Slot1 slot1 ;
	Slot2 slot2 ;
	Slot3 slot3 ;
	Slot4 slot4 ;

	Signal<> sig ;
	Signal<int> sig_int ;
	Signal<std::string> sig_string ;

	sig.connect(&slot1,&Slot1::meth) ;
	sig.connect(&slot2,&Slot2::meth) ;

	sig() ;

	sig.disconnect(&slot1,&Slot1::meth) ;

	sig() ;

	sig_int.connect(&slot3,&Slot3::meth_int) ;
	sig_int(2) ;

	sig_string.connect(&slot4,&Slot4::meth) ;
	sig_string("test!!") ;

	return 0 ;
}


