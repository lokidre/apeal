/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
// return type

#include <iostream>
#include <vector>


class SignalImplBase {
public :
	virtual void invoke() = 0 ;
	virtual void *get_slot() const = 0 ;
} ;



template <class slot_T, class meth_T>
class SignalImpl : public SignalImplBase {
public :
	SignalImpl(slot_T *s, meth_T m) : slot_ptr_(s), meth_ptr_(m) {}

public :
	slot_T *slot_ptr_ ;
	meth_T meth_ptr_ ;

public :
	void invoke()
	{
		(slot_ptr_->*meth_ptr_)() ;
	}

	void *get_slot() const {  return (void *)slot_ptr_ ;  }
} ;



class SignalBase {
protected :
	template <class slot_T, class meth_T>
	void connect_base ( slot_T *s, meth_T m )
	{
		slot_list_.push_back(new SignalImpl<slot_T,meth_T>(s,m)) ;
	}

	template <class slot_T>
	void disconnect_base ( slot_T *slot )
	{
		for ( slot_list_type::iterator slot_it = slot_list_.begin() ; slot_it != slot_list_.end() ; ) {
			if ( (*slot_it)->get_slot() == (void *)slot )
				slot_it = slot_list_.erase(slot_it) ;
			else
				++slot_it ;
		}
	}

	template <class slot_T, class meth_T>
	void disconnect_base ( slot_T *slot, meth_T m )
	{
		for ( slot_list_type::iterator slot_it = slot_list_.begin() ; slot_it != slot_list_.end() ; ) {
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

public :
	void invoke()
	{
		for ( slot_list_type::iterator slot_it = slot_list_.begin(), slot_end = slot_list_.end() ; slot_it != slot_end ; ++slot_it )
			(*slot_it)->invoke() ;
	}

	void operator()() {  invoke() ;  }


protected :
	typedef std::vector<SignalImplBase *> slot_list_type ;

	slot_list_type slot_list_ ;
} ;


class SignalVoid : public SignalBase {
public :  
	template <class slot_T>
	void connect ( slot_T *s, void (slot_T::*m)() )
	{
		connect_base(s,m) ;
	}

	template <class slot_T>
	void disconnect ( slot_T *s )
	{
		disconnect_base(s) ;
	}

	template <class slot_T>
	void disconnect ( slot_T *s, void (slot_T::*m)() )
	{
		disconnect_base(s,m) ;
	}

} ;


template <class ret_T>
class SignalR : public SignalBase {
public :
	template <class slot_T>
	void connect ( slot_T *s, ret_T (slot_T::*m)() )
	{
		connect_base(s,m) ;
	}

	template <class slot_T>
	void disconnect ( slot_T *s )
	{
		disconnect_base(s) ;
	}

	template <class slot_T>
	void disconnect ( slot_T *slot, ret_T (slot_T::*m)() )
	{
		disconnect_base(m) ;
	}
} ;


template <class ret_T>
class Signal : public SignalR<ret_T> {
} ;

template < >
class Signal<void> : public SignalVoid {
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
	int meth_int()
	{
		std::cout << "int Slot3::meth_int()" << std::endl ;
		return 1 ;
	}
} ;




int main()
{
	Slot1 slot1 ;
	Slot2 slot2 ;
	Slot3 slot3 ;

	Signal<void> sig ;
	Signal<int> sig_int ;

	sig.connect(&slot1,&Slot1::meth) ;
	sig.connect(&slot2,&Slot2::meth) ;

	sig() ;

	sig.disconnect(&slot1,&Slot1::meth) ;
	sig.disconnect(&slot2) ;

	sig() ;

	sig_int.connect(&slot3,&Slot3::meth_int) ;
	sig_int() ;

	return 0 ;
}


