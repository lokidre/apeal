/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <iostream>
#include <ctime>

#include <apeal/socket.cxx>
#include <apeal/except.cxx>
#include <apeal/http.cxx>
#include <apeal/file.cxx>
#include <apeal/str.cxx>
#include <apeal/policy.cxx>


template <class policy_T=apl::ADefaultPolicy>
class FileDownload {
  private :
    apl::AFile<> file_ ;
    bool valid_ ;
    unsigned long processed_count_, total_count_ ;
    time_t start_time_ ;

  public :
    std::string file_name_ ;

    FileDownload() : valid_(false) {}

    ~FileDownload()
    {
      std::cout << std::endl ;
    }

    int connect(){return 0 ;}
    void close(){}



    int send ( apl::AHttpResponse<> &response, const char *buffer, const int size )
    {
      if ( !valid_ ) {
        file_.create(file_name_,file_.create_always,file_.access_write) ;
        valid_ = true ;
        processed_count_ = total_count_ = 0 ;
        if ( !response.content_length.empty() )
          apl::parse_int(response.content_length,total_count_) ;
      }
      file_.write(buffer,size) ;
      processed_count_ += size ;

      std::cout << "\rdownloading " << file_name_ << " : " << processed_count_*100/total_count_ << "% " << "\t\t" << std::flush ;

      if ( processed_count_ >= total_count_ && total_count_ != 0 ) {
        time_t current_time ;
        time(&current_time) ;
        std::cout << std::endl << "time " << int(difftime(current_time,start_time_)) << " sec" << std::endl ;
      }

      return size ;
    }

    template <class response_type>
    int recv ( response_type &request, char *buffer, const int size )
    {
      return 0 ;
    }

} ;



int main ( int argc, char *argv[] )
{
  using namespace apl ;
  using namespace std ;


  int ret_code = 1 ;
  string uri, local_file ;

  try {
    if ( argc <= 1 )
      throw runtime_error("required parameter missing") ;


    uri = argv[1] ;

    if ( argc >= 3 )
      local_file = argv[2] ;
    else
      local_file = uri.substr(uri.find_last_of('/')+1) ;

    if ( local_file.empty() )
      throw runtime_error("local file name required") ;

    ASocket<> sock ;
    FileDownload<> body_file ;
    body_file.file_name_ = local_file ;
    AHttp<ASocket<>, ADefaultPolicy, FileDownload<> > http(sock,body_file) ;

    http.request.method = "GET" ;
    http.request.uri = uri ;

    if ( http.request.uri.substr(0,7) != "http://" )
      http.request.uri.insert(0,"http://") ;

    http.query() ;

    ret_code = 0 ;
  }
  catch(exception &e) {
    cerr << e.what() << endl ;
  }
  catch(...) {
    cerr << "exception!" << endl ;
  }

  if ( ret_code ) {
    AFileSystem<> fs ;
    fs.rm(local_file) ;
  }

  return ret_code ;
}
