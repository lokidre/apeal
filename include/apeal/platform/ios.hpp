/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#if TARGET_OS_IPHONE

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>



typedef unsigned long off64_t ;

//
// <unistd.h>
//

inline int open64 ( const char *file, int flags ) {  return ::open(file,flags) ;  }
inline int open64 ( const char *file, int flags, mode_t mode ) {  return ::open(file,flags,mode) ;  }
inline int creat64 (const char *file, mode_t mode) {  return ::creat(file,mode) ;  }

inline off64_t lseek64 (int fd, off64_t offset, int whence) {  return (off64_t)::lseek(fd,offset,whence) ;  }


#endif

