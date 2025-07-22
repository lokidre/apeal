/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>
#include <string>
#include <vector>

#include <apl/file.hpp>
#include <apl/parse/xmk.hpp>



const std::string test_dir = "test" ;
std::string ex_string ;

std::string desc_file_name, desc_file_path ;
tParseXML xml_parse ;
std::string txt ;
tObjectXML<std::string> xml_obj ;
AFile<> file ;




// generate xml description

int gen_level  ;

void xml_desc_gen_ident ( int lvl , std::string &desc )
{
  for ( int i = 0 ; i < lvl ; i++ ) {
    desc += ' ' ;
  }
}

void xml_desc_gen_tag ( t_xml_element &element, std::string &desc )
{
  std::string s ;

  xml_desc_gen_ident(gen_level,desc) ;
  desc += strfmt ( s, "tag: '%s'\n", element.name.c_str() ) ;

  if ( element.content.length() ) {
    xml_desc_gen_ident(gen_level,desc) ;
    desc += strfmt ( s, "content: '%s'\n", element.content.c_str() ) ;
  } 
  
  gen_level++ ;
  for ( t_xml_element_list::iterator el = element.el.begin() ; el != element.el.end() ; el++ ) {
    xml_desc_gen_tag(*el,desc) ;
  }
  gen_level-- ;
}

void xml_desc_gen ( tObjectXML<std::string> &obj, std::string &desc )
{
  desc.clear() ;
  gen_level = 0 ;

  std::string s ;

  if ( obj.prolog.version.length() )
    desc += str_format ( s, "version '%s'\n", obj.prolog.version ) ;

  if ( obj.prolog.encoding.length() )
    desc += str_format ( s, "encoding '%s'\n", obj.prolog.encoding ) ;

  if ( obj.prolog.standalone.length() )
    desc += str_format ( s, "standalone '%s'\n", obj.prolog.standalone ) ;

  if ( obj.prolog.doctype.name.length() )
    desc += str_format ( s, "doctype '%s'\n", obj.prolog.doctype.name ) ;

  xml_desc_gen_tag(obj.element,desc) ;
}



void xml_test ( const std::string &file_name )
{
  file.LoadText(file_name,txt) ;

  xml_parse.parse ( txt, xml_obj ) ;

  // generate description
  std::string desc ;
  {
    desc_file_name = file_name ;
    desc_file_name = std::string("desc") + desc_file_name.substr(desc_file_name.find_first_of('_')) ;
    MakeFilePath(test_dir,desc_file_name,desc_file_path) ;


    xml_desc_gen(xml_obj,desc) ;

    if ( !FileExists(desc_file_path) )
      file.SaveText ( desc_file_path, desc ) ;
  }

  // load stored destription
  {
    //std::string template_file_name = file_name ;
    //template_file_name = std::string("template") + template_file_name.substr(template_file_name.find_first_of('_')) ;
    std::string templ ;

    //if ( FileExists(template_file_name) ) {
      file.LoadText(desc_file_path,templ) ;

      if ( stricmp(templ,desc) != 0 )
        std::cout << "Test Failed on '" << file_name << "'" << std::endl ;
    //}
  }


  // generate output xml
  {
    std::string output_file_name = file_name ;
    output_file_name = test_dir + "/" + std::string("gen") + output_file_name.substr(output_file_name.find_first_of('_')) ;

    std::string gen_txt ;

    xml_parse.generate ( xml_obj, gen_txt ) ;

    file.SaveText ( output_file_name, gen_txt ) ;
  }
}


int main ( void )
{
  tDir xml_dir ;
  tDirEntryList files ;

  xml_dir.except() ;
  file.except() ;

  try {
    xml_dir.Open(test_dir) ;
    xml_dir.Read("test_*.xml",files) ;


    for ( tDirEntryList::iterator file = files.begin() ; file != files.end() ; file++ )
      xml_test(test_dir+"/"+file->name) ;
  }
  catch(t_err_except *err) {
    std::cerr << "Error:" << err->error_code << ":" << err->error_string << std::endl ;
  }
  catch(...) {
    std::cerr << "Exception!" << std::endl ;
  }
  
  std::cout << "XML test done" << std::endl ;

  return 0 ;
}

