/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <apeal/def.hpp>

#if __OS == __OS_UNIX
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include <iostream>
#include <stdexcept>

std::string watch_dir = "test" ;

void dir_handler()
{
  std::cout << "Directory changed!" << std::endl ;
}


#if __OS == __OS_UNIX
void dir_handler_unix ( int signum ) 
{ 
  dir_handler() ;
  signal(signum,dir_handler_unix) ;
}
#endif



int main()
{
  using namespace std ;
  int ret_code = 1 ;

  try {
    #if __OS == __OS_UNIX
    DIR *dir = opendir("test") ;

    if ( dir == 0 )
      throw runtime_error("Error in opendir()") ;

    int fd = dirfd ( dir ) ;

    if ( fd == -1 )
      throw runtime_error("Error in dirfd()") ;

    int pid = getpid() ;

    cout << "getpid(): " << pid << endl ;




    // std::cout << "signal :" << signal ( SIGIO, dir_handler ) << std::endl ;




    // setting notification

    int old_own = fcntl ( fd, F_GETOWN ) ;

    std::cout << "OWN: " << old_own << std::endl ;

    fcntl ( fd, F_SETOWN, pid ) ;

    fcntl ( fd, F_SETFL, FASYNC ) ;



    // setting signal handler
    cout << "signal :" << signal ( SIGIO, dir_handler_unix ) << endl ;


    // listening
    fd_set fds ;
    FD_ZERO(&fds) ;
    FD_SET(fd,&fds) ;

    cout << "waiting for changes..." << endl ;
    if ( select ( 1, &fds, &fds, &fds, NULL ) < 0 )
      std::cout << "select error" << std::endl ;

    std::cout << "k" << std::endl ;


    closedir(dir) ;
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

