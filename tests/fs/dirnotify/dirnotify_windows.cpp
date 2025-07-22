/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/def.hpp>

#include <iostream>

void dir_handler()
{
  std::cout << "Directory changed!" << std::endl ;
}


std::string watch_dir = "test" ;


int main()
{
  using namespace std ;
  int ret_code = 1 ;

  try {

    #if __OS == __OS_WINDOWS

    HANDLE notification_handle_ = 
        FindFirstChangeNotification (
            watch_dir.c_str(),
            TRUE,  // watch subtree
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY
        ) ;


    if ( notification_handle_ == INVALID_HANDLE_VALUE )
      throw runtime_error("Error in FindFirstChangeNotification") ;


    for(;;) {  // main watching thread

      cout << "waiting for changes..." << endl ;
      DWORD wait_status = WaitForSingleObject(notification_handle_,INFINITE) ;

      if ( wait_status == WAIT_OBJECT_0 ) {
        dir_handler() ;
        if ( FindNextChangeNotification(notification_handle_) == FALSE )
          throw runtime_error("Error in FindNextChangeNotification") ;
      } else 
        throw runtime_error("Error from WaitForSingleObject") ;
    }


    #endif

    ret_code = 0 ;
  }
  catch(exception &e) {
    cerr << e.what() << endl ;
  }
  catch(...) {
    cerr << "exception!" << endl ;
  }



  return ret_code ;
}


