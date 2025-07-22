/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
// lambda functions
#include <iostream>

#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <vector>


template <class... Args>
class SlotItemBase {
public :
	virtual void invoke(Args...) = 0 ;
	virtual void *slot() const = 0 ;
	virtual ~SlotItemBase(){} 
} ;


class SlotNull{} ;

template <class S, class... Args>
class SlotItem : public SlotItemBase<Args...> {
public :
	typedef void (S::*M)(Args...) ;

	S *slotPtr_{} ;
	M methPtr_{} ;

	//bool isFunc{false} ;
	//typedef std::function<void(Args...)> F ;
	//F func_ ;

	SlotItem(S *slot, M method) : slotPtr_(slot), methPtr_(method) {}
	//SlotItem(const F &_func) ; //: isFunc{true} { /*func_ = _func ;*/ }
	//SlotItem &operator()(const F &_func) { isFunc = true, func_ = _func ; }

protected :
	void invoke(Args... args) override { 
		//if ( isFunc )
		//	func_(args...) ;
		//else
			(slotPtr_->*methPtr_)(args...) ;  
	}  
	void *slot() const override {  return (void *)slotPtr_ ;  }
} ;



template <class... Args>
class Signal {
private :
	std::mutex mutex_ ;
	typedef std::lock_guard<std::mutex> Lock ;

	std::atomic<bool> invoked_ = false ;

public :
	typedef std::function<void(Args...)> F ;

protected :

	//typedef std::vector<SlotItemBase<Args...> *> slot_list_type ;
	typedef std::tuple<F,SlotItemBase<Args...> *> list_item_type ;
	typedef std::vector<list_item_type> slot_list_type ;

	slot_list_type slots_ ;


public :
	~Signal() {
		disconnect() ;
	}

	void disconnect() {
		Lock lock(mutex_) ;

		for ( auto &slot : slots_ )
			if ( auto ptr = std::get<1>(slot) )
				delete ptr ;
		slots_.clear() ;
	}


	void connect_function ( const F &func ) {
		if ( invoked_ )
			throw std::runtime_error("Recursive signal invoke detected") ;

		Lock lock(mutex_) ;
		slots_.push_back(list_item_type(func,nullptr)) ;
		
		//slots_.push_back(new SlotItem<SlotNull,Args...>(func)) ;
	}

	void connect ( const F &func ) {
		if ( invoked_ )
			throw std::runtime_error("Recursive signal invoke detected") ;

		Lock lock(mutex_) ;
		
		slots_.push_back(list_item_type(func,nullptr)) ;
	}

	template <class S>
	void connect ( S *s, typename SlotItem<S,Args...>::M m )
	{
		if ( invoked_ )
			throw std::runtime_error("Recursive signal invoke detected") ;

		Lock lock(mutex_) ;
		
		slots_.push_back(list_item_type(nullptr,new SlotItem<S,Args...>(s,m))) ;
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
		using namespace std ;

		invoked_ = true ;
		Lock lock(mutex_) ;

		for ( auto &slot : slots_ ) {
			if ( get<0>(slot) )
				get<0>(slot)(args...) ;
			else
				get<1>(slot)->invoke(args...) ;
		}

		invoked_ = false ;
	}

	void operator()(Args... args) { invoke(args...) ;  }




} ;


using namespace std ;

Signal<int> sig ;
Signal<int,int> sig_int ;
Signal<int,std::string> sig_string ;


class Slot1 {
public :
	void meth(int th)
	{
		cout << "Thread " << th << " Slot1::meth()\n" ;
	}
} ;

class Slot2 {
public :
	void meth(int th)
	{
		cout << "Thread " << th << " Slot2::meth()\n" ;
	}
} ;

class Slot3 {
public :
	void meth_int(int th, int i)
	{
		cout << "Thread " << th << " Slot3::meth_int(" << i << ")\n" ;
	}
} ;

class Slot4 {
public :
	void meth(int th, std::string v)
	{
		cout << "Thread " << th << " Slot4::meth(" << v << ")\n" ;
	}

	string m = "Slot 4 Member" ;

	Slot4() {

		function<void(int,string)> ff = [this](int th, string s) {
			cout << "Thread " << th << " Function ( " << s << " : " << m << " )\n" ;
		} ;
		sig_string.connect(ff) ;

		sig_string.connect( [this](int th, string s) {
			cout << "Thread " << th << " Function::gg ( " << s << " : " << m << " )\n" ;
		} ) ;

	}
} ;





void connect_function()
{
		function<void(int)> ff = [](int th){
			cout << "Thread " << th << " std::function<void(int)>()\n" ;
		} ;

		sig.connect_function(ff) ;


		sig_int.connect_function ( [](int th,int v) {
			cout << "Thread " << th << " std::function<void(int)>()\n" ;
		}) ;
}




int main()
{
	connect_function() ;

	try {

		Slot1 slot1 ;
		Slot2 slot2 ;
		Slot3 slot3 ;
		Slot4 slot4 ;

		sig.connect(&slot1,&Slot1::meth) ;
		sig.connect(&slot2,&Slot2::meth) ;


		/*
		sig.connect_function ( [](int th){
			cout << "Thread " << th << " Slot1::meth()" << endl ;
		}) ;
		*/

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
