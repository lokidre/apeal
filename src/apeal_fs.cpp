/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/fs.hpp"

namespace apl {

void AFileSystem::tmpdir(String *dir) {

#if APEAL_OS_WINDOWS // def WIN32
    wchar_t winPath[MAX_PATH + 1];
    DWORD result = ::GetTempPathW(MAX_PATH, winPath);
    if (result > 0 && result <= MAX_PATH) {
        winPath[result] = 0;
        *dir = apiFromUtf16(winPath);
        *dir = cvtFileToPosix(*dir);
    }
#else
    char const *var[] = {"TMPDIR", "TMP", "TEMP", "TEMPDIR"};
    dir->clear();
    for (auto v: var) {
        char const *dirname = getenv(v);
        if (dirname) {
            *dir = dirname;
            break;
        }
    }

    if (dir->empty()) {

#ifdef __ANDROID__
        *dir = "/data/local/tmp";
#else
        *dir = "/tmp";
#endif

    }

#endif
}




bool AFileSystem::cd(StrView dirname) {

    bool result = true;

    ApiString nameApi{dirname};

#if APEAL_OS_WINDOWS

    result = ::SetCurrentDirectoryW(nameApi.win()) ? true : false;

#elif APEAL_OS_POSIX

    result = !::chdir(nameApi.utf8()) ? true : false;

#elif APEAL_OS_XAOS

    tXaosParam parm;
    parm.object = this;
    parm.buffer = dirname.c_str();
    result = ::XaosApi(&parm, API_CLASS_FILE, API_DIRECTORY_SET_CURRENT)
                 ? true
                 : false;

#elif TARGET_OS_DOS

    result = ::DosSetCurrentDirectory(dirname.c_str());

#endif

    // if ( !Result )  needed error handling

    return result;
}



void AFileSystem::mkdir_basic(StrView dirname, int flags, int atts) {
    ApiString apiName{dirname};

#if APEAL_OS_WINDOWS
    auto ret = ::CreateDirectoryW(apiName.win(), 0);
    verifyWindowsLastError(ret, dirname);
#endif

#if APEAL_OS_POSIX
    auto ret = ::mkdir(apiName.c_str(), atts);
    verifyErrno(!ret, dirname);
#endif

#if APEAL_OS_XAOS
    tXaosParam parm;

    parm.object = this;
    parm.buffer = dirname.c_str();
    parm.param = att;

    auto res = bool(::XaosApi(&parm, API_CLASS_FILE, API_DIRECTORY_CREATE));
#endif
  
}



String AFileSystem::getSystemDir(SystemDir dirType) {
    String systemDir;
#if APEAL_OS_WINDOWS
    wchar_t dir[512];
    int id = 0;
    switch (dirType) {
    case SystemDir::UsrBin:
      id = CSIDL_PROGRAM_FILES;
      break;
    case SystemDir::UsrAppData:
      id = CSIDL_LOCAL_APPDATA;
      break;
    }
    SHGetFolderPathW(0, id, 0, SHGFP_TYPE_CURRENT, dir);

    systemDir = apiFromUtf16(dir);

    systemDir = cvtFileToPosix(systemDir);
#endif

    return systemDir;
}



void AFileSystem::getHomeDir(String& dir) {
#if APEAL_OS_WINDOWS
#elif APEAL_OS_LINUX
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    dir = homedir;
#endif
}



String AFileSystem::getCurrentDirectory() {

    String dir;

#if APEAL_OS_WINDOWS
    Buffer<wchar_t> buffer{2048};
    for (;;) {
      auto res = (std::size_t)::GetCurrentDirectoryW(DWORD(buffer.size() - 1),
                                                     buffer.get());
      verifyWindowsLastError(res > 0,
                             "getCurrentDirectory"); // TODO: windows error

      if (res > 0) {
        if (res > buffer.size() - 1) {
          buffer.alloc(std::max(res + 1, buffer.size() * 2));
          continue;
        }

        dir = apiFromUtf16(buffer.data());
        break;
      }
    }

    dir = cvtFileToPosix(dir);
#endif

#if APEAL_OS_POSIX

#ifdef __GLIBC__ // && !defined(PATH_MAX)
    char *name = ::get_current_dir_name();
    verify(name, "getCurrentDirectory");
    dir = apiFromUtf8(name);
    free(name);
#else
    Buffer<char> buffer{2048};
    for (;;) {
      auto res = ::getcwd(buffer.get(), buffer.size() - 1);
      if (!res) {
        verifyErrno(errno != ERANGE, "pwd");
        buffer.alloc(buffer.size() * 2);
        continue;
      }
      dir = buffer.data();
      break;
    }

    // char name[PATH_MAX+1] ;
    // auto res = ::getcwd ( name, MAX_PATH ) ;
    // verifyErrno ( res, "getCurrentDirectory" ) ;
    // dir = fromSysApiString(name) ;
#endif // if not glibc
#endif // Unix

#if APEAL_OS_XAOS
    tXaosParam parm;
    parm.object = this;
    parm.buffer = DirectoryName;
    parm.count = *nMaxCount;
    Result = ::XaosApi(&parm, API_CLASS_FILE, API_DIRECTORY_GET_CURRENT)
                 ? true
                 : false;

    if (!parm.result)
      // if ( !Result )
      *nMaxCount = (int)dword(parm.count);
#endif

    return dir;
}



void AFileSystem::rm(StrView filename) {
#if APEAL_OS_WINDOWS
    auto res = ::DeleteFileW(ApiString{filename}.win());
    verifyWindowsLastError(res, filename);
#endif

#if APEAL_OS_POSIX
    auto res = ::remove(ApiString(filename).utf8());
    verifyErrno(!res, filename);
#endif

#if APEAL_OS_XAOS
    tXaosParam parm;

    parm.object = this;
    parm.buffer = file_name.c_str();

    auto result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_DELETE);
#endif
}



void AFileSystem::rename(StrView oldpath, StrView newpath) {
#if APEAL_OS_WINDOWS
    auto flags = MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING |
                 MOVEFILE_WRITE_THROUGH;
    auto result = ::MoveFileExW(ApiString{oldpath}.win(),
                                ApiString{newpath}.win(), flags);
    verifyWindowsLastError(result, oldpath);
#else
    auto result =
        ::rename(ApiString{oldpath}.utf8(), ApiString{newpath}.utf8());
    verifyErrno(0 == result, oldpath);
#endif

#if 0 // DOS, XAOS
        Result = ::DosFileRename(OldName, NewName);

        XaosParam parm;
        parm.object = this;
        parm.buffer = OldName;
        parm.buffer2 = NewName;
        result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_RENAME) ? true : false;
#endif
  }

/*
  // copy file
  bool AFileSystem::cp(StrView old_name, StrView new_name) {
#if APEAL_OS_WINDOWS

    if (::CopyFileW(ApiString{old_name}.win(), ApiString{new_name}.win(),
                    FALSE) == 0)
      return handle_error_();

#elif APEAL_OS_POSIX

    // internal_error() ;
    //  Result = !copy ( OldName, NewName ) ;

#elif APEAL_OS_XAOS

    tXaosParam parm;
    parm.object = this;
    parm.buffer = OldName;
    parm.buffer2 = NewName;
    Result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_RENAME) ? true : false;

#endif

    return true;
}
*/


String AFileSystem::getFullName(StrView filename) {

#if APEAL_OS_WINDOWS

    Buffer<wchar_t> buffer{2048};
    String fullname;
    ApiString apiName{filename};

    for (;;) {
      auto res = (std::size_t)::GetFullPathNameW(
          apiName.win(), (DWORD)buffer.size() - 1, buffer.get(), 0);
      if (res >= buffer.size()) {
        buffer.alloc(std::max(res + 1, buffer.size() * 2));
        continue;
      }
      fullname = apiFromUtf16(buffer.data());
      break;
    }

    // Replace '\' with '/'
    for (auto &c : fullname)
      if (c == '\\')
        c = '/';

    return fullname;
#endif

#if APEAL_OS_POSIX
    char actualpath[PATH_MAX + 1];

    auto res = realpath(String{filename}.c_str(), actualpath);

    verifyErrno(res, "getFullName");

    return actualpath;
#endif

}



bool AFileSystem::exists(StrView filename) {

    ApiString apiName{filename};

    bool result = false;

    bool fromBundle = false;

    if (filename[0] == ':') {
      fromBundle = true;
    }

#if APEAL_OS_WINDOWS

    auto atts = ::GetFileAttributesW(apiName.win());
    if (atts != INVALID_FILE_ATTRIBUTES)
      result = true;

#elif APEAL_OS_POSIX

    if (fromBundle) {
#if APEAL_OS_ANDROID
      struct stat st;
      result = android_assets_stat(apiName.c_str(), &st) >= 0 ? true : false;
#endif
    } else {
      char buffer[sizeof(struct stat) + 256 +
                  1]; // seems iPhone crashes on smaller buffer

      if (::stat(apiName.c_str(), (struct stat *)buffer) != 0 &&
          errno == ENOENT)
        result = false;
      else
        result = true;
    }

#elif APEAL_OS_XAOS

    tXaosParam parm;
    parm.object = NULL;
    parm.buffer = FileName;
    Result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_EXIST) ? true : false;

    result = parm.result;

#else
    internal_error();
#endif

    return result;
  }




bool AFileSystem::getStat(StrView filename, FileInfo *fi) {
    bool res = true;

    // error_file_name_ = file_name;

#if APEAL_OS_WINDOWS

    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(ApiString{filename}.win(), &fd);

    if (hFind != INVALID_HANDLE_VALUE) {
      FindClose(hFind);

      // convert the file information returned by search
      fi->attributes = fd.dwFileAttributes;
      fi->size = fd.nFileSizeHigh;
      fi->size <<= 32;
      fi->size |= fd.nFileSizeLow;

      fi->creation = fromWindowsFileTime(fd.ftCreationTime);
      fi->write = fromWindowsFileTime(fd.ftLastWriteTime);
      fi->access = fromWindowsFileTime(fd.ftLastAccessTime);
    } else {
      res = false; //, error_code = GetLastError() ;
    }

#elif APEAL_OS_POSIX

    // struct stat st ;
    char buffer[sizeof(struct stat) + 256 +
                1]; // seems iPhone crashes on smaller buffer

    // if ( ::stat(file_name.c_str(),(struct stat *)buffer) != 0 && errno ==
    // ENOENT )

    struct stat *st = (struct stat *)buffer;

    if (::stat(ApiString{filename}.utf8(), st) == 0) {
      fi->creation = fromUnixTime(st->st_ctime);
      fi->write = fromUnixTime(st->st_mtime);
      fi->access = fromUnixTime(st->st_atime);
      fi->attributes = st->st_mode;
      fi->size = (Index)st->st_size;
    } else {
      res = false; // TODO, this->error_code = errno ;
    }

#else
    internal_error();
#endif
    return res;
  }

  // Get file attributes
  uint32_t getFileAttributes(StringA fileName) {
#if APEAL_OS_WINDOWS
    auto atts = ::GetFileAttributesW(ApiString(fileName).win());
    verify(atts != (DWORD)-1, "Cannot read file attributes");

#elif APEAL_OS_POSIX

    struct stat buf;

    auto res = ::stat(fileName.c_str(), &buf);

    verify(res == 0, "Cannot read file attributes");

    auto atts = buf.st_mode;

#elif APEAL_OS_XAOS

    tXaosParam parm;

    parm.object = this;
    parm.buffer = FileName;

    auto result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_GET_ATTRIBUTES)
                      ? true
                      : false;

    auto atts = parm.param;

#elif TARGET_OS_DOS
    Result = ::DosFileGetAttributes(FileName, Attributes);
#endif

    return atts;
  }




void AFileSystem::setFileAttributes(StringA filename, uint32_t atts) {
#if APEAL_OS_WINDOWS
    auto res = ::SetFileAttributesW(ApiString(filename).win(), atts);
    verifyWindowsLastError(res, filename);
#elif APEAL_OS_POSIX
    auto res = ::chmod(filename.c_str(), atts);
    verifyErrno(!res, filename);
#elif APEAL_OS_XAOS
    tXaosParam parm;

    parm.object = this;
    parm.buffer = FileName;
    parm.param = Attributes;

    auto result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_SET_ATTRIBUTES)
                      ? true
                      : false;
#elif TARGET_OS_DOS || HOST_OS_DOS
    ::DosFileSetAttributes(FileName, Attributes);
#endif
  }



void AFileSystem::rmdir(StrView dirname) {
#if APEAL_OS_WINDOWS
    auto res = ::RemoveDirectoryW(ApiString{dirname}.win());
    verifyWindowsLastError(res, dirname);
#elif APEAL_OS_POSIX
    auto res = ::rmdir(ApiString{dirname}.utf8());
    verifyErrno(res == 0, dirname);
#endif
  
}

}  // namespace apl

