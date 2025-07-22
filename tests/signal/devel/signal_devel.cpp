/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../../common/main.hpp"


#include <memory>
#include <functional>

namespace intrnl {
// Have to use virtual calls
template <class... Args>
struct ASlotBase {
	virtual void invoke(Args...) = 0 ;
	virtual void *slot() const = 0 ;
	
	//virtual ~ASlotBase(){}
} ;


template <class C, class... Args>
struct ASlotItem: public ASlotBase<Args...> {

	using M = void (C::*)(Args...) ;

	M m{} ;  // slot method
	C *c{} ;  // slot class

	ASlotItem ( M _m, C *_c ): m(_m), c(_c){}

protected :
	void invoke(Args... args) override { (c->*m)(args...) ;  }  
	void *slot() const override {  return (void *)c ;  }
} ;

}


template <class ...Args>
class ASignal {
public:
	~ASignal() { disconnect() ; }

	// Connecting Class::Method
	// Follow the std::bind to avoid confusion
	template <class C>
	void connect ( typename intrnl::ASlotItem<C,Args...>::M m, C *c ) 
	{
		slot_ = std::make_unique<intrnl::ASlotItem<C,Args...>>(m,c) ;
	}

	// Connecting lambda or function
	// convenient but might be less efficient
	using F = std::function<void(Args...)> ;
	F f{} ;

	void connect ( F &&_f ) {
		f = std::move<F>(_f) ;
	}

	void operator()(Args&& ...args){  emit(std::forward<Args>(args)...) ;  }

	void emit(Args&& ...args) {
		if ( slot_ )
			slot_->invoke(std::forward<Args>(args)...) ;
		else
			if ( f )
				f(std::forward<Args>(args)...) ;
	}

	void disconnect(){  
		slot_.reset() ;  
		f = 0 ;
	}

private:
	std::unique_ptr<intrnl::ASlotBase<Args...>> slot_ ;

} ;

struct C {
	void v() {  apl::println("C::v()") ;  }
	void i(int i) {  apl::println("C::i(%d)", i) ;  }
} ;


ASignal<std::string> g ;

void scope()
{
	g.f = [](auto s){  apl::println("[](%s)", s) ;  } ;
}


void body()
{
	ASignal<int> i ;

	C c ;

	i.connect ( &C::i, &c ) ;
	i(2) ;

	i.disconnect() ;

	i.f = [](int i){  apl::println("[](%d)",i) ;  } ;

	i(3) ;


	scope() ;
	g("Sample string") ;

}



#include "../../common/main.cpp"

