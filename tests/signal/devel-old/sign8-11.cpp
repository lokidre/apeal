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

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>


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
private :
	std::mutex mutex_ ;
	typedef std::lock_guard<std::mutex> Lock ;

	std::atomic<bool> invoked_ = false ;

public :
	~Signal() {
		Lock lock(mutex_) ;

		for ( auto &slot : slots_ )
			delete slot ;
	}


	template <class S>
	void connect ( S *s, typename SlotItem<S,Args...>::M m )
	{
		if ( invoked_ )
			throw std::runtime_error("Recursive signal invoke detected") ;

		Lock lock(mutex_) ;
		slots_.push_back(new SlotItem<S,Args...>(s,m)) ;
	}

	template <class S>
	void disconnect ( S *slot )
	{
		if ( invoked_ )
			throw std::runtime_error("Recursive signal invoke detected") ;

		Lock lock(mutex_) ;
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
		if ( invoked_ )
			throw std::runtime_error("Recursive signal invoke detected") ;

		Lock lock(mutex_) ;

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
		invoked_ = true ;
		Lock lock(mutex_) ;

		for ( auto &slot : slots_ )
			slot->invoke(args...) ;

		invoked_ = false ;
	}

	void operator()(Args... args) { invoke(args...) ;  }



protected :
	typedef std::vector<SlotItemBase<Args...> *> slot_list_type ;

	slot_list_type slots_ ;
} ;



class Slot1 {
public :
	void meth(int th)
	{
		std::cout << "Thread " << th << " Slot1::meth()" << std::endl ;
	}
} ;

class Slot2 {
public :
	void meth(int th)
	{
		std::cout << "Thread " << th << " Slot2::meth()" << std::endl ;
	}
} ;

class Slot3 {
public :
	void meth_int(int th, int i)
	{
		std::cout << "Thread " << th << " Slot3::meth_int(" << i << ")" << std::endl ;
	}
} ;

class Slot4 {
public :
	void meth(int th, std::string v)
	{
		std::cout << "Thread " << th << " Slot4::meth(" << v << ")" << std::endl ;
	}
} ;






using namespace std ;

int main()
{
	try {

		Signal<int> sig ;
		Signal<int,int> sig_int ;
		Signal<int,std::string> sig_string ;

		Slot1 slot1 ;
		Slot2 slot2 ;
		Slot3 slot3 ;
		Slot4 slot4 ;

		sig.connect(&slot1,&Slot1::meth) ;
		sig.connect(&slot2,&Slot2::meth) ;

		sig_int.connect(&slot3,&Slot3::meth_int) ;
		sig_int(0,2) ;

		sig_string.connect(&slot4,&Slot4::meth) ;

		thread th1 ( [&]() {
			for( int i = 0 ; i < 1000 ; i++ ) {
				sig(1) ;
				sig_int(1,i) ;
				sig_string(1,"thread 1!!!") ;
				//this_thread::sleep_for(chrono::milliseconds(1)) ;
			}
		}) ;

		thread th2 ( [&]() {
			for( int i = 0 ; i < 1000 ; i++ ) {
				sig(2) ;
				sig_int(2,i) ;
				sig_string(2,"thread 2!!!") ;
				//this_thread::sleep_for(chrono::milliseconds(2)) ;
			}
		}) ;

		thread th3 ( [&]() {
			for( int i = 0 ; i < 1000 ; i++ ) {
				sig(3) ;
				sig_int(3,i) ;
				sig_string(3,"thread 3!!!") ;
				//this_thread::sleep_for(chrono::milliseconds(2)) ;
			}
		}) ;


		th1.join() ;
		th2.join() ;
		th3.join() ;
	}
	catch(exception &e) {
		cout << e.what() << endl ;
	}
	catch(...) {
		cout << "Unhandled Exception!" << endl ;
	}

	return 0 ;
}
