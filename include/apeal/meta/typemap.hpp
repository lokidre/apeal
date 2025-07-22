/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once


#include "../def.hpp"


namespace apl {


namespace typemap {

namespace typemap_private {

	struct NullType {
		typedef NullType Key ;
		typedef NullType Value ;
	} ;

	template <class K, class V, class N=NullType>
	struct item_ {
		typedef K Key ;
		typedef V Value ;
		typedef N Next ;
	} ;

	template <class K, class V>
	struct item_<K,V,NullType> {
		typedef K Key ;
		typedef V Value ;
		typedef NullType Next ;
	} ;

}  // typemap_private namespace



	template <class K, class V>
	struct typemap {
		typedef typename typemap_private::item_<K,V> X ;
	} ;


namespace typemap_private {

	template <class K, class C, class KK=typename C::Key>
	struct find_match_ {
		typedef typename find_match_<K,typename C::Next,typename C::Next::Key>::X X ;
	} ;

	template <class K, class C>
	struct find_match_<K,C,K> {
		typedef C X ;
	} ;

	template <class K, class KK>
	struct find_match_<K,NullType,KK> {
		typedef NullType X ;
	} ;
}  // private

	template <class K, class C>
	struct find {
		typedef typename typemap_private::find_match_<K,C>::X X ;
		typedef X second_type ;
	} ;

	template <class K>
	struct find<K,typemap_private::NullType> {
		typedef typemap_private::NullType X ;
	} ;


namespace typemap_private {

	template <typename T, typename I, typename P>
	struct back_match_ {
		typedef typename back_match_<typename I::Next,typename I::Next,I>::X X ;
	} ;

	// end of list
	template <typename I, typename P>
	struct back_match_<NullType,I,P> {
		typedef P X ;
	} ;

}  // private

	template <class C>
	struct back {
		typedef typename typemap_private::back_match_<typename C::Next,C,typename C::Next>::X X ;
		typedef typename X::Key Key ;
		typedef typename X::Value Value ;
	} ;

	template <typename C>
	struct front {
		typedef C X ;
		typedef typename C::Key Key ;
		typedef typename C::Value Value ;
	} ;


namespace typemap_private {

	// iterator with no parameters
	template <class C, template <class> class F>
	struct iterator : iterator<typename C::Next,F> {
		void operator()()
		{
			F<C> f ;
			f() ;

			iterator<typename C::Next,F> next ;
			next() ;
		}
	} ;

	template <template <class> class F>
	struct iterator<NullType,F> {
		void operator()(){}
	} ;


	// iterator with one parameter
	template <class C, template <class> class F, class P1>
	struct iterator_1 : iterator_1<typename C::Next,F,P1> {
		void operator()(P1 &p1)
		{
			F<C> f ;
			f(p1) ;

			iterator_1<typename C::Next,F,P1> next ;
			next(p1) ;
		}
	} ;

	template <template <class> class F, class P1>
	struct iterator_1<NullType,F,P1> {
		void operator()(P1 &){}
	} ;

}  // private

	template <class C, template <class> class F>
	struct for_each {
		for_each() // for no parameters
		{
			typename typemap_private::iterator<C,F> it ;
			it() ;
		}

		template <class P1>
		for_each(P1 &p1) // one parameter
		{
			typename typemap_private::iterator_1<C,F,P1> it ;
			it(p1) ;
		}
	} ;


	template <class K, class T, class C=typemap_private::NullType>
	struct push_front {
		typedef typemap_private::item_<K,T,C> X ;
	} ;


namespace typemap_private {
	template <class C, class I>
	struct reverse_ {
		typedef item_<typename C::Key, typename C::Value, I> Item ;
		typedef typename reverse_<typename C::Next,Item>::X X ;
	} ;

	template <class I>
	struct reverse_<NullType,I> {
		typedef I X ;
	} ;
}

	template <class C>
	struct reverse {
		typedef typename typemap_private::reverse_<C,typemap_private::NullType>::X X ;
	} ;


	template <class K, class T, class C>
	struct push_back {
	private :
		typedef typename reverse<C>::X R ;
		typedef typename push_front<K,T,R>::X P ;
	public  :
		typedef typename reverse<P>::X X ;
	} ;


namespace typemap_private {

	template <class R, class C>
	struct concat_ {
		typedef item_<typename R::Key,typename R::Value,C> I ;
		typedef typename concat_<typename R::Next,I>::X X ;
	} ;


	template <class C>
	struct concat_<NullType,C> {
		typedef C X ;
	} ;

	template <class M>
	struct erase_front_ {
		typedef typename M::Next X ;
	} ;

	template <class K, class C, class KK=typename C::Key, class I=NullType, class O=C>
	struct erase_ {
	private :
		typedef item_<typename C::Key,typename C::Value,I> Item ;
	public :
		typedef typename erase_<K,typename C::Next,typename C::Next::Key,Item,O>::X X ;
	} ;

	// item was found
	template <class K, class C, class I, class O>
	struct erase_<K,C,K,I,O> {
	private :
		typedef I R ;  // reversed
		typedef typename erase_front_<C>::X E ;  // erased
	public :
		typedef typename concat_<R,E>::X X ;
	} ;

	// item was not found
	template <class K, class CurrentKey, class I, class O>
	struct erase_<K,NullType,CurrentKey,I,O> {
		typedef O X ;
	} ;
}

	template <class K, class C>
	struct erase {
		typedef typename typemap_private::erase_<K,C>::X X ;
	} ;


namespace typemap_private {

	template <class K, class V, class C, class CurrentKey=typename C::Key, class I=NullType, class O=C>
	struct insert_ {
	private :
		typedef item_<typename C::Key,typename C::Value,I> Item ;
	public :
		typedef typename insert_<K,V,typename C::Next,typename C::Next::Key,Item,O>::X X ;
	} ;

	// case when element was found
	template <class K, class V, class C, class I, class O>
	struct insert_<K,V,C,K,I,O> {
	private :
		typedef I R ;  // reversed
		typedef typename erase_front_<C>::X E ;
		typedef typename push_front<K,V,E>::X P ;
	public :
		typedef typename concat_<R,P>::X X ;
	} ;

	// case when element was not found
	template <class K, class V, class CurrentKey, class I, class O>
	struct insert_<K,V,NullType,CurrentKey,I,O> {
		typedef typename push_front<K,V,O>::X X ;
	} ;
}

	template <typename K, typename V, class C=typemap_private::NullType>
	struct insert {
		typedef typename typemap_private::insert_<K,V,C>::X X ;
	} ;

	// specialization for empty container
	template <class K, class V>
	struct insert<K,V,typemap_private::NullType> {
		typedef typename typemap_private::item_<K,V> X ;
	} ;


}  // typemap


}


