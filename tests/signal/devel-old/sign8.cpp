/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
// add thread-safety

// policy: 
//   . slot can be called without finishing the previous call, 
//     the slot has to implement synchronization itself (if necessary)
//   . changing of slot lists is protected by mutexes

#include <iostream>
#include <vector>


#include <apeal/mutex.cxx>


class AVoid{} ;
class ANull{} ;


class SignalImplBase0 {
public :
	virtual void invoke() = 0 ;
	virtual void *get_slot() const = 0 ;
} ;

template <class T1>
class SignalImplBase1 {
public :
	virtual void invoke(T1) = 0 ;
	virtual void *get_slot() const = 0 ;
} ;

template <class T1, class T2>
class SignalImplBase2 {
public :
	virtual void invoke(T1,T2) = 0 ;
	virtual void *get_slot() const = 0 ;
} ;


// generic class, must be specialized
template <class slot_T, class ret_T=AVoid, class T1=AVoid, class T2=AVoid, class dummy_T=ANull>
class SignalImpl {
private :
	SignalImpl(){}
} ;

// specializations
template <class slot_T>
class SignalImpl<slot_T,AVoid,AVoid,AVoid> : public SignalImplBase0 {
public :
	typedef slot_T slot_type ;
	typedef void (slot_T::*meth_type)() ;

public :
	slot_type *slot_ptr_ ;
	meth_type meth_ptr_ ;

public :
	SignalImpl(slot_type *s, meth_type m) : slot_ptr_(s), meth_ptr_(m) {}
	void invoke() { (slot_ptr_->*meth_ptr_)() ;  }
	void *get_slot() const {  return (void *)slot_ptr_ ;  }
} ;

template <class slot_T, class ret_T>
class SignalImpl<slot_T,ret_T,AVoid,AVoid> : public SignalImplBase0 {
public :
	typedef slot_T slot_type ;
	typedef ret_T (slot_T::*meth_type)() ;

public :
	slot_type *slot_ptr_ ;
	meth_type meth_ptr_ ;

public :
	SignalImpl(slot_T *s, meth_type m) : slot_ptr_(s), meth_ptr_(m) {}
	void invoke() { (slot_ptr_->*meth_ptr_)() ;  }
	void *get_slot() const {  return (void *)slot_ptr_ ;  }
} ;

template <class slot_T, class ret_T, class T1>
class SignalImpl<slot_T,ret_T,T1,AVoid> : public SignalImplBase1<T1> {
public :
	typedef slot_T slot_type ;
	typedef ret_T (slot_T::*meth_type)(T1) ;

public :
	slot_type *slot_ptr_ ;
	meth_type meth_ptr_ ;

public :
	SignalImpl(slot_type *s, meth_type m) : slot_ptr_(s), meth_ptr_(m) {}
	void invoke(T1 p1) { (slot_ptr_->*meth_ptr_)(p1) ;  }
	void *get_slot() const {  return (void *)slot_ptr_ ;  }
} ;



template <class signal_impl_base_T>
class SignalBase {
protected :

	SignalBase() //: mutex_valid_(false)
	{
		list_mutex_.create() ;
	}

	template <class impl_T>
	void connect_base ( typename impl_T::slot_type *s, typename impl_T::meth_type m )
	{
		apl::ALock<mutex_type> lock(list_mutex_) ;
		slot_list_.push_back(new impl_T(s,m)) ;
	}

	template <class slot_T>
	void disconnect_base ( slot_T *slot )
	{
		apl::ALock<mutex_type> lock(list_mutex_) ;
		for ( typename slot_list_type::iterator slot_it = slot_list_.begin() ; slot_it != slot_list_.end() ; ) {
			if ( (*slot_it)->get_slot() == (void *)slot )
				slot_it = slot_list_.erase(slot_it) ;
			else
				++slot_it ;
		}
	}

	template <class impl_T>
	void disconnect_base ( typename impl_T::slot_type *slot, typename impl_T::meth_type m )
	{
		apl::ALock<mutex_type> lock(list_mutex_) ;
		for ( typename slot_list_type::iterator slot_it = slot_list_.begin() ; slot_it != slot_list_.end() ; ) {
			if ( (*slot_it)->get_slot() == (void *)slot ) {
				impl_T *pimpl = (impl_T *)(*slot_it) ;

				if ( pimpl->meth_ptr_ == m )
					slot_it = slot_list_.erase(slot_it) ;
				else
					++slot_it ;
			} else
				++slot_it ;
		}
	}

	void invoke_base()
	{
		slot_list_type slot_list(slot_list_) ;

		for ( typename slot_list_type::iterator slot_it = slot_list.begin(), slot_end = slot_list.end() ; slot_it != slot_end ; ++slot_it )
			(*slot_it)->invoke() ;
	}

	template <class T1>
	void invoke_base(T1 p1)
	{
		slot_list_type slot_list(slot_list_) ;

		for ( typename slot_list_type::iterator slot_it = slot_list_.begin(), slot_end = slot_list_.end() ; slot_it != slot_end ; ++slot_it )
			(*slot_it)->invoke(p1) ;
	}

	template <class T1, class T2>
	void invoke_base(T1 p1, T2 p2)
	{
		slot_list_type slot_list(slot_list_) ;

		for ( typename slot_list_type::iterator slot_it = slot_list_.begin(), slot_end = slot_list_.end() ; slot_it != slot_end ; ++slot_it )
			(*slot_it)->invoke(p1,p2) ;
	}


protected :
	typedef std::vector<signal_impl_base_T *> slot_list_type ;

	slot_list_type slot_list_ ;

private :
	bool mutex_valid_ ;
	typedef apl::AMutex<> mutex_type ;
	mutex_type list_mutex_ ;
} ;


template <class ret_T>
class SignalR0 : public SignalBase<SignalImplBase0> {
public :
	template <class slot_T>
	void connect ( slot_T *s, ret_T (slot_T::*m)() ) {  this->template connect_base<SignalImpl<slot_T,ret_T> >(s,m) ;  }

	template <class slot_T>
	void disconnect ( slot_T *s ) {  disconnect_base(s) ;  }

	template <class slot_T>
	void disconnect ( slot_T *s, ret_T (slot_T::*m)() ) {  this->template disconnect_base<SignalImpl<slot_T,ret_T> >(s,m) ; }

	void operator()() {  invoke_base() ;  }
} ;

template <class ret_T, class T1>
class SignalR1 : public SignalBase<SignalImplBase1<T1> > {
public :
	template <class slot_T>
	void connect ( slot_T *s, ret_T (slot_T::*m)(T1) ) {  this->template connect_base<SignalImpl<slot_T,ret_T,T1> >(s,m) ;  }

	template <class slot_T>
	void disconnect ( slot_T *s ) {  disconnect_base(s) ;  }

	template <class slot_T>
	void disconnect ( slot_T *s, ret_T (slot_T::*m)(T1) ) {  this->template disconnect_base<SignalImpl<slot_T,ret_T,T1> >(s,m) ; }

	void operator()(T1 p1) {  invoke_base(p1) ;  }
} ;

template <class ret_T, class T1, class T2>
class SignalR2 : public SignalBase<SignalImplBase2<T1,T2> > {
public :
	template <class slot_T>
	void connect ( slot_T *s, ret_T (slot_T::*m)(T1,T1) ) {  this->template connect_base<SignalImpl<slot_T,ret_T,T1,T2> >(s,m) ;  }

	template <class slot_T>
	void disconnect ( slot_T *s ) {  disconnect_base(s) ;  }

	template <class slot_T>
	void disconnect ( slot_T *s, ret_T (slot_T::*m)(T1,T2) ) {  this->template disconnect_base<SignalImpl<slot_T,ret_T,T1,T2> >(s,m) ; }

	void operator()(T1 p1, T1 p2) {  invoke_base(p1,p2) ;  }
} ;



// generic class, must be specialized
template <class ret_T=void, class T1=AVoid, class T2=AVoid, class dummy_T=AVoid>
class Signal {
private :
	Signal(){}  // needed, saying that maximum number of arguments have reached
} ;

// specializations
template <class ret_T>
class Signal<ret_T,AVoid,AVoid> : public SignalR0<ret_T> {} ;

template <class ret_T, class T1>
class Signal<ret_T,T1,AVoid> : public SignalR1<ret_T,T1> {} ;

template <class ret_T, class T1, class T2>
class Signal<ret_T,T1,T2> : public SignalR2<ret_T,T1,T2> {} ;






class Slot1 {
public :
	Slot1(Signal<> &sig) : sig_(sig) {}

	void meth()
	{
		std::cout << "Slot1::meth():signal disconnect" << std::endl ;
		sig_.disconnect(this) ;
	}

private :
	Signal<> &sig_ ;
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

class Slot4 {
public :
	void meth(int v)
	{
		std::cout << "Slot4::meth(" << v << ")" << std::endl ;
	}
} ;



Signal<> sig ;
Signal<int> sig_int ;
Signal<void,int> sig_par ;

Slot1 slot1(sig) ;
Slot2 slot2 ;
Slot3 slot3 ;
Slot4 slot4 ;


void *thread_1 ( void *)
{
	return 0 ;
}

void *thread_2 ( void * )
{
	return 0 ;
}

void *thread_3 ( void * )
{
	return 0 ;
}



int main()
{
	sig.connect(&slot1,&Slot1::meth) ;
	sig.connect(&slot2,&Slot2::meth) ;

	sig() ;

	sig() ;

	sig_int.connect(&slot3,&Slot3::meth_int) ;
	sig_int() ;

	sig_par.connect(&slot4,&Slot4::meth) ;
	sig_par(5) ;

	return 0 ;
}
