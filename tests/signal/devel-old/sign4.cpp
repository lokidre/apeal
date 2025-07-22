/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
// adding container
// adding disconnect

#include <iostream>
#include <vector>


class SignalImplBase {
public :
	virtual void invoke() = 0 ;
	virtual void *get_slot() const = 0 ;
} ;



template <class slot_T, class meth_T>
class SignalImpl : public SignalImplBase {
friend class Signal ;
public :
	SignalImpl(slot_T *s, meth_T m) : slot_ptr_(s), meth_ptr_(m){}

private :
	slot_T *slot_ptr_ ;
	meth_T meth_ptr_ ;


public :
	void invoke()
	{
		(slot_ptr_->*meth_ptr_)() ;
	}

	void *get_slot() const {  return (void *)slot_ptr_ ;  }
} ;


class Signal {
public :

	template <class slot_T, class meth_T>
	void connect ( slot_T *s, meth_T m )
	{ 
		slot_list_.push_back(new SignalImpl<slot_T,meth_T>(s,m)) ;
	}

	template <class slot_T>
	void disconnect ( const slot_T *slot )
	{
		for ( auto slot_it = slot_list_.begin() ; slot_it != slot_list_.end() ; ) {
			if ( (*slot_it)->get_slot() == (void *)slot ) {
				slot_it = slot_list_.erase(slot_it) ;
			} else
				++slot_it ;
		}
	}

	template <class slot_T, class meth_T>
	void disconnect ( const slot_T *slot, const meth_T m )
	{
		for ( auto slot_it = slot_list_.begin() ; slot_it != slot_list_.end() ; ) {
			if ( (*slot_it)->get_slot() == (void *)slot ) {
				SignalImpl<slot_T,meth_T> *pimpl = (SignalImpl<slot_T,meth_T> *)(*slot_it) ;

				if ( pimpl->meth_ptr_ == m )
					slot_it = slot_list_.erase(slot_it) ;
				else
					++slot_it ;
			} else
				++slot_it ;
		}
	}

	void invoke()
	{
		for ( auto &slot : slot_list_ )
			slot->invoke() ;
	}

	void operator()() {  invoke() ;  }


private :
	typedef std::vector<SignalImplBase *> slot_list_type ;

	slot_list_type slot_list_ ;
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




int main()
{
	Slot1 slot1 ;
	Slot2 slot2 ;

	Signal sig ;

	sig.connect(&slot1,&Slot1::meth) ;
	sig.connect(&slot2,&Slot2::meth) ;

	sig() ;

	sig.disconnect(&slot1,&Slot1::meth) ;
	sig.disconnect(&slot2) ;

	sig() ;

	return 0 ;
}


