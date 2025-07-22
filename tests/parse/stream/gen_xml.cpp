/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
// Generate huge XML for testing the parsing capabilities

#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdio>


// parameters
int max_level = 3 ;
int max_content = 5 ;
int max_atts = 2 ;


int content_index = 0, content_sum = 0, att_index = 0, att_sum = 0 ;


template <class stream_type>
void build_indent ( stream_type &file, int level )
{
  for ( int ispace = 0 ; ispace < level + 1 ; ispace++ )
    file << '\t' ;
}



template <class stream_type>
void build_content ( stream_type &file, int level )
{
  using namespace std ;

  char tag[64] ;

  for ( int icontent = 0 ; icontent < max_content ; icontent++ ) {
    build_indent(file,level) ;

    sprintf ( tag, "Tag_%d_%d", level, icontent ) ;

    if ( max_level < 2 || level < max_level - 2 )
      cout << tag << endl ;


    file << '<' << tag ;
    
    for ( int iatt = 0 ; iatt < max_atts ; iatt++ ) {
      file << " att_" << iatt << "='" << att_index << '\'' ;
      att_sum += att_index++ ;
    }

    
    file << ">\n" ;

    build_indent(file,level+1) ;
    file << content_index << endl ;
    content_sum += content_index++ ;


    if ( level < max_level )
      build_content(file,level+1) ;


    // fill up with some content
    build_indent(file,level) ;
    file << "</" << tag << ">\n" ;
  }

}


int process_cmdln ( int argc, char *argv[] )
{
  using namespace std ;

  for ( int iarg = 1 ; iarg < argc ; iarg++ ) {
    char *arg = argv[iarg] ;

    if ( strcmp(arg,"--help") == 0 || strcmp(arg,"-h") == 0 || strcmp(arg,"-?") == 0 ) {
      cout << "gen_xml [--max_content <n>] [--max_level <n>] [--max_att <n>]" << endl ;
      return 1 ;
    }

    if ( iarg == argc ) {
      cout << "invalid command line" << endl ;
      return 1 ;
    }


    char *parm = argv[++iarg] ;

    if ( strcmp(arg,"--max_content") == 0 )
      max_content = atoi(parm) ;
  }

  return 0 ;
}



int main ( int argc, char *argv[] )
{
  using namespace std ;

  if ( process_cmdln(argc,argv) != 0 )
    return 1 ;


  ofstream file("huge.xml") ;


  if ( !file.good() ) {
    cerr << "error creating file" << endl ;
    return 1 ;
  }

  file << "<?xml version=\"1.0\"?>" << endl ;
  file << "<!DOCTYPE test>" << endl ;

  file << "<Root>" << endl ;

  build_content(file,0) ;


  file << "\t<!-- Content Sum: " << content_sum << ", Attributes Sum: " << att_sum << " -->\n" ;
  file << "\t" << content_sum << ":" << att_sum << "\n" ;
  file << "</Root>\n" ;
  file.close() ;


  return 0 ;
}


