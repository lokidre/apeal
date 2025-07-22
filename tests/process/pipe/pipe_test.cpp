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


#include <apeal/process.cxx>
#include <apeal/pipe.cxx>
#include <apeal/thread.cxx>
#include <apeal/time.cxx>


using namespace apl ;
using namespace std ;


class OutputHandler {
  public :
    bool finish ;

    OutputHandler() : finish(false) {}

  public :
    void stdout_thread(APipeEnd<> &pipe)
    {
      char buffer[1024] ;
      while(!finish) {
        int count = pipe.read(buffer,1024) ;

        if ( count <= 0 ) {
          Sleep(1000) ;
          continue ;
        }

        buffer[count] = 0 ;

        cout << "cout: " << buffer << endl ;
      }
    }

    void stderr_thread(APipeEnd<> &pipe)
    {
      char buffer[1024] ;
      while(!finish) {
        int count = pipe.read(buffer,1024) ;

        if ( count <= 0 ) {
          Sleep(1000) ;
          continue ;
        }

        buffer[count] = 0 ;

        cout << "cerr: " << buffer << endl ;
      }
    }
} ;






int main ( int argc, char *argv[] )
{

  int ret_code = 1 ;

  try {
    string command = "ls" ;

    if ( argc > 1 )
      command = argv[1] ;

    AProcess<> proc ;


    APipeEnd<> stdout_rd, stdout_wr, stderr_rd, stderr_wr, stdin_rd, stdin_wr;
    APipe<> stdout_pipe(stdout_rd,stdout_wr), stderr_pipe(stderr_rd,stderr_wr), stdin_pipe(stdin_rd,stdin_wr) ;
    OutputHandler output_handler ;

    stdout_pipe.create() ;
    stderr_pipe.create() ;
    stdin_pipe.create() ;

    proc.stdout_handle = stdout_wr.handle() ;
    proc.stderr_handle = stderr_wr.handle() ;
    proc.stdin_handle = stdin_rd.handle() ;


    AThreadMeth<OutputHandler,APipeEnd<> &> stdout_thread, stderr_thread ;

    stdout_thread(&output_handler,&OutputHandler::stdout_thread,stdout_rd) ;
    stderr_thread(&output_handler,&OutputHandler::stderr_thread,stderr_rd) ;


    proc.run(command) ;


    proc.wait() ;
    output_handler.finish = true ;

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
