/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>
#include <sstream>

#include <apeal/parse/xml.hpp>

int main()
{
	using namespace apl ;
	using namespace std ;

	int ret_code = 1 ;

	try {
		//string input_file("test.xml") ;
		string input_file("cdata.xml") ;
		string node("node") ;


		AXmlParser parser ;
		AXml xml ;
		AXml::element_list_type els ;

		parser.load(input_file,xml) ;

		//xml.select("node",els) ;
		xml.select("sql-debug",els) ;

		cout << "selected: " << node << endl ;
		cout << "total: " << els.size() << endl ;

		for ( const auto &el : els ) {
			cout << "name: " << el.name << endl ;;
			for ( const auto &att : el.atts ) {
				cout << "\t" << att.name << "=" << att.value << endl ;
			}
		}


		ret_code = 0 ;
	}
	catch(exception &e) {
		cerr << e.what() << endl ;
	}
	catch(...) {
		cerr << "exception" << endl ;
	}

	return ret_code ;
}

