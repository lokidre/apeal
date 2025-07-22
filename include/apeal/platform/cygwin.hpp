/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once


#ifdef __CYGWIN__

#include <unistd.h>
#include <dirent.h>


//
// <unistd.h>
//

using off64_t = unsigned long ;

inline int open64 ( const char *file, int flags ) {  return ::open(file,flags) ;  }
inline int open64 ( const char *file, int flags, mode_t mode ) {  return ::open(file,flags,mode) ;  }
inline int creat64 (const char *file, mode_t mode) {  return ::creat(file,mode) ;  }

inline off64_t lseek64 (int fd, off64_t offset, int whence) {  return ::lseek(fd,offset,whence) ;  }


//
// <dirent.h>
//

using dirent64 = dirent ;

inline int readdir_r (DIR *dir, struct dirent *entry, struct dirent **result)
{
  struct dirent *p = ::readdir(dir) ;

  if ( p ) {
    *entry = *p ;
    *result = entry ;
  } else 
    *result = 0 ;

  return 0 ;
}

inline dirent64 *readdir64 (DIR *dir) {  return ::readdir(dir) ;  }
inline int readdir64_r ( DIR *dir, dirent64 *entry, dirent64 **result ) {  return ::readdir_r(dir,entry,result) ;  }


//
// <time.h>
//
inline tm *localtime_r ( const time_t *t, struct tm *r ) {  *r = *::localtime(t) ;  return r ;  }
inline tm *gmtime_r ( const time_t *t, struct tm *r) {  *r = *::gmtime(t) ;  return r ;  }


//
// <netdb.h>
//
/*
inline
const char *gai_strerror(int ecode)
{
	static char const *error_string = "" ;
	return error_string ;
}
*/

/*
struct addrinfo {  
	int ai_flags, ai_family, ai_socktype, ai_protocol ;
	size_t ai_addrlen ;  
	char *ai_canonname ;
	struct sockaddr* ai_addr ;
	struct addrinfo* ai_next ;
} ;
*/


//#define _strnicmp strnicmp


#endif
