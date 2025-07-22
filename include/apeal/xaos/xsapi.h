/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "def.h"



#define API_CLASS_MM                    0x00000003
#define API_CLASS_CON                   0x00000004    // console
#define API_CLASS_FILE                  0x00000005
#define API_CLASS_PROCESS               0x00000006
#define API_CLASS_ENVIRONMENT           0x00000007
#define API_CLASS_OS                    0x00000008    // operating system related


// Memory Management
#define API_MM_ALLOC                    0x00000003    // allocate memory
#define API_MM_FREE                     0x00000004    // free memory
#define API_MM_GET_INFO                 0x00000005    // returns available memory


// Console
#define API_CONSOLE_OUT                 0x00000003    // Write string to console

// File
#define API_FILE_CREATE                 0x00000003    // Create File
#define API_FILE_CLOSE                  0x00000004    // Close Handle
#define API_FILE_READ                   0x00000005
#define API_FILE_WRITE                  0x00000006
#define API_FILE_FLUSH                  0x00000007
#define API_FILE_GET_POSITION           0x00000008
#define API_FILE_SET_POSITION           0x00000009
#define API_FILE_TRUNCATE               0x0000000A
#define API_FILE_GET_SIZE               0x0000000B
#define API_FILE_GET_TIME               0x0000000C
#define API_FILE_SET_TIME               0x0000000D
#define API_FILE_GET_INFO               0x0000000E
#define API_FILE_DELETE                 0x0000000F
#define API_FILE_RENAME                 0x00000010
#define API_FILE_EXIST                  0x00000011
#define API_FILE_GET_ATTRIBUTES         0x00000012 
#define API_FILE_SET_ATTRIBUTES         0x00000013

#define API_DIRECTORY_CREATE            0x00010007
#define API_DIRECTORY_REMOVE            0x00010008
#define API_DIRECTORY_SET_CURRENT       0x00010009
#define API_DIRECTORY_GET_CURRENT       0x0001000A
#define API_DIRECTORY_OPEN              0x0001000B
#define API_DIRECTORY_CLOSE             0x0001000C
#define API_DIRECTORY_GET_SIZE          0x0001000D
#define API_DIRECTORY_READ              0x0001000E
#define API_DIRECTORY_GET_POSITION      0x0001000F
#define API_DIRECTORY_SET_POSITION      0x00010010


// Process
#define API_PROCESS_CREATE              0x00000003
#define API_PROCESS_TERMINATE           0x00000004
#define API_PROCESS_EXIT                0x00000005


// Environment
#define API_ENV_GET_VAR                 0x00000003


// Operating System
#define API_OS_GET_VERSION              0x00000003


// Version 0.00.02

#define XAOS_PARAM_STRUCT_SIZE 20*4

struct XaosParam {
  dword  handle ;      // 0  - caller handle
  dword  version ;     // 4  - caller version

  dword  apiclass ;    // 8
  dword  apifunc ;     // 12

  void*  object ;      // 16
  void*  buffer ;      // 20
  dword  param ;       // 24
  dword  count ;       // 28
  qword  size ;        // 32

  dword  error ;       // 36 - last operation error code
  dword  result ;      // 40 - result

  void*  object2 ;
  void*  buffer2 ;
  dword  param2 ;
  dword  count2 ;
  qword  size2 ;

  void*  object3 ;
  void*  buffer3 ;
  dword  param3 ;

  void*  object4 ;
  void*  buffer4 ;
  dword  param4 ;
} ;


extern "C"
bool xaos_api ( tXaosParam *parm, dword apiclass, dword apifunc ) ;

