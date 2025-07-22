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


int main ( int argc, char *argv[] )
{
  using namespace apl ;
  using namespace std ;
  
 
  try {
  
    string input_file("test.xml"), output_file("output.xml") ;
    //string att_name("att"), att_value("value") ;
    //string node_name("node"), node_content("content") ;
    //string cond_att_name("id"), cond_att_value("1") ;
    
    AXmlParser parser ;
    AXml xml ;
    
    parser.load(input_file,xml) ;
    
    
    xml.update("node","att2","value2") ;
    xml.update("node","att","new value") ;
    xml.update("node","att3","value 3","id","first") ;
    
    xml.remove("node2") ;
    
    xml.insert("node","node4") ;
    xml.insert("node","node5","id","first") ;
    //xml.insert("node","node5","att2","value2") ;
    
    parser.save(output_file,xml) ;
  
  }
  catch(exception &e) {
    cerr << e.what() << endl ;
    return 1 ;
  }
  catch(...) {
    cerr << "exception!" << endl ;
    return 1 ;
  }
  
  return 0 ;
}


