/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "filename.hpp"

#if APEAL_OS_WINDOWS

#include <Knownfolders.h>
#include <Shlobj.h>

#else
#include <cstdlib>
#endif

namespace apl {

class AFileSystem {

public:
  using size_type = size_t;
  using ErrorPolicy = AErrorPolicy;

  ErrorPolicy error_;

public:
  // modes
  enum {
#if APEAL_OS_WINDOWS
    acc_rusr = 0,
    acc_wusr = 0,
    acc_xusr = 0,
    acc_rgrp = 0,
    acc_wgrp = 0,
    acc_xgrp = 0,
    acc_roth = 0,
    acc_woth = 0,
    acc_xoth = 0,
    acc_sticky = 0,
#else
    acc_rusr = S_IRUSR,
    acc_wusr = S_IWUSR,
    acc_xusr = S_IXUSR,
    acc_rgrp = S_IRGRP,
    acc_wgrp = S_IWGRP,
    acc_xgrp = S_IXGRP,
    acc_roth = S_IROTH,
    acc_woth = S_IWOTH,
    acc_xoth = S_IXOTH,
    acc_sticky = S_ISVTX,
#endif
    mkdir_parents = 0x0100000,
    MKDIR_PARENTS = mkdir_parents,
    acc_dummy,

    acc_rwxu = acc_rusr | acc_wusr | acc_xusr,
    acc_rwxg = acc_rgrp | acc_wgrp | acc_xgrp,
    acc_rwxo = acc_roth | acc_woth | acc_xoth,
  };

public: // host system information functions
  enum class SystemDir {
    UsrBin,
    UsrAppData,
  };

  String getSystemDir(SystemDir dirType);
// {
//     String systemDir;
// #if APEAL_OS_WINDOWS
//     wchar_t dir[512];
//     int id = 0;
//     switch (dirType) {
//     case SystemDir::UsrBin:
//       id = CSIDL_PROGRAM_FILES;
//       break;
//     case SystemDir::UsrAppData:
//       id = CSIDL_LOCAL_APPDATA;
//       break;
//     }
//     SHGetFolderPathW(0, id, 0, SHGFP_TYPE_CURRENT, dir);

//     systemDir = apiFromUtf16(dir);

//     systemDir = cvtFileToPosix(systemDir);
// #endif

//     return systemDir;
//   }

public:
  ////////// File functions //////////////

  // Rename file or directory
  // bool mv ( const String &old_name, const String &new_name ) {  return
  // mv(old_name.c_str(),new_name.c_str()) ;  }

  // copy file
  // bool cp(std::string_view source_path, std::string_view target_path) {
  // return cp(source_path, target_path); }

  // delete a file or file group by mask
  // bool RemoveFile ( const char *file_name ) {  return rm(file_name) ;  }
  // bool RemoveFile ( std::string &file_name ) {  return rm(file_name.c_str())
  // ;  }

  // Get - set file attributes

  // Retrieve file full path names

  // retrieves the file size
  /*
  size_type GetFileSize ( const char *file_name )
  {
    File f ;
    if ( !f.open ( file_name ) )
      return size_type(-1) ;

    uint32_t s ;
    if ( !f.GetSize(&s) )
      return size_type(-1) ;

    f.close() ;

    return s ;
  }
  */

  auto fileSize(StrView filename) {
    File f;
    f.open(filename, FileAccess::Read);
    return f.size();
  }

  bool GetUniqueFileName(char *pTempFile); // needed

  // Returns the temporary directory
  static void tmpdir(String *dir);

  static auto tmpdir() {
    String dir;
    tmpdir(&dir);
    return dir;
  }

//   static auto getHomeDir(String& dir) {
// #if APEAL_OS_WINDOWS
// #elif APEAL_OS_LINUX
//     struct passwd *pw = getpwuid(getuid());
//     const char *homedir = pw->pw_dir;
//     dir = homedir;
// #endif
//   }
  static void getHomeDir(String& dir);
//    {
// #if APEAL_OS_WINDOWS
// #elif APEAL_OS_LINUX
//     struct passwd *pw = getpwuid(getuid());
//     const char *homedir = pw->pw_dir;
//     dir = homedir;
// #endif
//   }

  static auto getHomeDir() {
    String dir;
    getHomeDir(dir);
    return dir;
  }

  // Get current directory
  static String getCurrentDirectory();
   

  // static void pwd ( String &str ) {  getCurrentDirectory(str) ;  }
  static auto pwd() { return getCurrentDirectory(); }

  //// more advanced functions with directories
  static void rm(StrView filename);

  //
  static bool rmIfExists(StrView filename) {
    if (!exists(filename))
      return false;
    rm(filename);
    return true;
  }

public:
  // file stat
  static bool getStat(StrView filename, FileInfo *fi);

  // Set file attributes
  void setFileAttributes(StringA filename, uint32_t atts);
//    {
// #if APEAL_OS_WINDOWS
//     auto res = ::SetFileAttributesW(ApiString(filename).win(), atts);
//     verifyWindowsLastError(res, filename);
// #elif APEAL_OS_POSIX
//     auto res = ::chmod(filename.c_str(), atts);
//     verifyErrno(!res, filename);
// #elif APEAL_OS_XAOS
//     tXaosParam parm;

//     parm.object = this;
//     parm.buffer = FileName;
//     parm.param = Attributes;

//     auto result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_SET_ATTRIBUTES)
//                       ? true
//                       : false;
// #elif TARGET_OS_DOS || HOST_OS_DOS
//     ::DosFileSetAttributes(FileName, Attributes);
// #endif
//   }

  // Change current directory

  static bool cd(StrView dirname);


public:
  static void mkdir_basic(StrView dirname, int flags = 0,
                          int atts = acc_rwxu | acc_rgrp | acc_xgrp | acc_roth |
                                     acc_xoth);
                                      // {
    // ApiString apiName{dirname};

// #if APEAL_OS_WINDOWS
//     auto ret = ::CreateDirectoryW(apiName.win(), 0);
//     verifyWindowsLastError(ret, dirname);
// #endif

// #if APEAL_OS_POSIX
//     auto ret = ::mkdir(apiName.c_str(), atts);
//     verifyErrno(!ret, dirname);
// #endif

// #if APEAL_OS_XAOS
//     tXaosParam parm;

//     parm.object = this;
//     parm.buffer = dirname.c_str();
//     parm.param = att;

//     auto res = bool(::XaosApi(&parm, API_CLASS_FILE, API_DIRECTORY_CREATE));
// #endif
  // }

  // Creates directory
  static bool mkdir(StrView dirname, int flags = 0,
                    int att = acc_rwxu | acc_rgrp | acc_xgrp | acc_roth |
                              acc_xoth) {
    bool parentCreated = false;

    if ((flags & mkdir_parents) == 0) {
      mkdir_basic(dirname, flags, att);
      return parentCreated;
    }

    // create all parent directories
    String currentDir;

    auto it = dirname.begin(), itEnd = dirname.end();

    for (;;) {
      for (; it != itEnd && (*it == '/' || *it == '\\'); ++it)
        currentDir.push_back(*it);

      for (; it != itEnd && *it != '/' && *it != '\\'; ++it)
        currentDir.push_back(*it);

      if (!currentDir.empty() && currentDir.back() != ':' &&
          !exists(currentDir)) {
        mkdir_basic(currentDir, flags, att);
        parentCreated = true;
      }

      if (it == itEnd)
        break;
    }

    return parentCreated;
  }

  static bool mkdirParents(StrView dirname, int atts = acc_rwxu | acc_rgrp |
                                                       acc_xgrp | acc_roth |
                                                       acc_xoth) {
    return mkdir(dirname, mkdir_parents, atts);
  }

  // If the directory doesn't exist create it and all its parents
  static bool mkdirIfNotExistsParents(StrView dirname,
                                      int atts = acc_rwxu | acc_rgrp |
                                                 acc_xgrp | acc_roth |
                                                 acc_xoth) {
    if (exists(dirname))
      return false;
    mkdirParents(dirname, atts);
    return true;
  }

  static bool mkdirIfNotExists(StrView dirname, int atts = acc_rwxu | acc_rgrp |
                                                           acc_xgrp | acc_roth |
                                                           acc_xoth) {
    if (exists(dirname))
      return false;
    mkdir(dirname, 0, atts);
    return true;
  }

  // Removes directory
  static void rmdir(StrView dirname);
//    {
// #if APEAL_OS_WINDOWS
//     auto res = ::RemoveDirectoryW(ApiString{dirname}.win());
//     verifyWindowsLastError(res, dirname);
// #elif APEAL_OS_POSIX
//     auto res = ::rmdir(ApiString{dirname}.utf8());
//     verifyErrno(res == 0, dirname);
// #endif
//   }

  /*
  // Get logical drives
  uint32_t GetVolumes()
  {
    uint32_t Volumes = 0 ;

    #if __OS == __OS_DOS || __HOST_OS == __OS_DOS

    Volumes = 0 ;
    byte Phantom ;

    // Get DOS List Of Lists
    tDosListOfLists *pListOfLists = DosGetListOfLists() ;

    // Go through the all fixed and floppy drives
    // The last DPB has a pointer to the next DPB - FFFF:FFFF (by doc).
    // Under Win95 when the last drive was ramdisk, the last DPB
    // has a xxxx:FFFF pointer to the next DPB.
    for ( tDosDPB *CurDPB = (tDosDPB *)pListOfLists->pFirstDPB ; ( (dword)CurDPB
  & 0xFFFF ) != 0xFFFF ; CurDPB = (tDosDPB *)CurDPB->pNextDPB ) {
      // Remove the phantom drives
      if ( DosQueryPhantomDriveMap ( CurDPB->DriveNumber+1, &Phantom ) &&
  Phantom && Phantom-1 != CurDPB->DriveNumber ) continue ; Volumes |= ( 1 <<
  CurDPB->DriveNumber ) ;
    }

    // Get CD-ROM drives
    int nCD ;
    if ( ( nCD = DosMscdGetCdromCount() ) != 0 ) {
      if ( nCD > 32 )
        internal_error() ;
      byte CdDrives[32] ;
      DosMscdGetCdromLetters ( CdDrives ) ;
      for ( int iCD = 0 ; iCD < nCD ; iCD++ )
        Volumes |= ( 1 << CdDrives[iCD] ) ;
    }

    // Get remote drives
    if ( DosIsNetwork() ) {
      char DriveName[16] ;  // temporary drive name
      byte DeviceType ;  // disk or printer
      for ( word iNwk = 0 ;
  DosNwkQueryAssignList(iNwk,DriveName,NULL,NULL,&DeviceType,NULL) ; iNwk++ ) {
        if ( DeviceType != 4 )  // If it's not a drive - look next
          continue ;
        if ( DriveName[1] == ':' )  // Check drive name
          Volumes |= ( 1 << ( ( DriveName[0] | 0x20 ) - 'a' ) ) ;
      }
    }


    #elif APEAL_OS_WINDOWS

    Volumes = ::GetLogicalDrives() ;

    #else
    //internal_error() ;
    #endif

    return Volumes ;
  }

  // Get logical drive stringd
  unsigned long getVolumes ( char *Names, uint32_t nMaxCount )
  {
    unsigned long Volumes = 0 ;

    #if  APEAL_OS_WINDOWS

    Volumes = ::GetLogicalDriveStrings ( nMaxCount, Names ) ;

    #elif __OS == __OS_DOS || __HOST_OS == __OS_DOS

    dword Drives = GetVolumes() ;

    //char *CurDrive = "A:" ;
    Names[0] = Names[1] = '\0' ;
    Volumes = 0 ;
    for ( int iDrive = 0 ; iDrive < 32 ; iDrive++ ) {
      if ( Drives & ( 1 << iDrive ) ) {
        Names[Volumes++] = 'A' + iDrive ;
        Names[Volumes++] = ':' ;
        Names[Volumes++] = '/' ;
        Names[Volumes++] = '\0' ;
      }
    }
    Names[Volumes++] = '\0' ;

    #else
      unused(Names,nMaxCount) ;
    #endif

    return Volumes ;
  }
  */

public:
  // Rename file or directory
  void rename(StrView oldpath, StrView newpath);

  // Returns file's full name
  String getFullName(StrView filename);


public:
  static bool exists(StrView filename);
//   {
//     ApiString apiName{filename};

//     bool result = false;

//     bool fromBundle = false;

//     if (filename[0] == ':') {
//       fromBundle = true;
//     }

// #if APEAL_OS_WINDOWS

//     auto atts = ::GetFileAttributesW(apiName.win());
//     if (atts != INVALID_FILE_ATTRIBUTES)
//       result = true;

// #elif APEAL_OS_POSIX

//     if (fromBundle) {
// #if APEAL_OS_ANDROID
//       struct stat st;
//       result = android_assets_stat(apiName.c_str(), &st) >= 0 ? true : false;
// #endif
//     } else {
//       char buffer[sizeof(struct stat) + 256 +
//                   1]; // seems iPhone crashes on smaller buffer

//       if (::stat(apiName.c_str(), (struct stat *)buffer) != 0 &&
//           errno == ENOENT)
//         result = false;
//       else
//         result = true;
//     }

// #elif APEAL_OS_XAOS

//     tXaosParam parm;
//     parm.object = NULL;
//     parm.buffer = FileName;
//     Result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_EXIST) ? true : false;

//     result = parm.result;

// #else
//     internal_error();
// #endif

//     return result;
//   }

  //
  // Directory reading functions
  //
  static auto readDir(StrView dirname, StrView mask = "*") {
    ADir dir;
    dir.open(dirname);
    return dir.read(mask);
  }

private:
  String error_file_name_;

  bool handle_error_(int error_code) {
    String error_string;
    ErrorPolicy error_policy;

#if APEAL_OS_WINDOWS
    error_string = getErrorStringWindows(error_code);
#elif (APEAL_OS_POSIX)
    error_string = getErrorStringUnix(error_code);
#endif

    if (!error_file_name_.empty())
      error_string += " - ", error_string += error_file_name_;

    return error_policy(error_string);
  }

  bool handle_error_() {
#if APEAL_OS_WINDOWS
    return handle_error_(::GetLastError());
#elif APEAL_OS_POSIX
    return handle_error_(errno);
#elif APEAL_OS_XAOS
    return handle_error_("");
#endif
  }
};

using FileSystem = AFileSystem;
using FS = FileSystem;

//
// Shortcuts
//

using AFS = AFileSystem;

//
// Extracting components
//

//// returns pair: file-name, file-ext
// inline std::pair<String, String> splitFileExt(StrView name) noexcept { return
// AFS::splitExt(name); }

//// Returns pair: file-dir, file-name
// inline std::pair<String, String> splitFilePath(StrView name) noexcept {
// return AFS::splitPath(name); }

// inline std::tuple<String, String, String> splitPathComponents(StrView in)
// noexcept { return AFS::splitPathComponents(in); }

// inline auto removeFileExt(StrView path) noexcept { return
// AFS::removeExt(path); } inline auto extractFileName(StrView path) noexcept {
// return AFS::extractBase(path); } inline auto extractBaseName(StrView path)
// noexcept { return AFS::extractBase(path); } inline auto
// extractDirName(StrView path) noexcept { return AFS::extractDir(path); }

//
// File and directory manipulations

inline bool fileExists(StrView filename) {
  return FileSystem::exists(filename);
}

inline bool mkdirIfNotExistsParents(StrView dirname,
                                    int atts = FileSystem::acc_rwxu |
                                               FileSystem::acc_rgrp |
                                               FileSystem::acc_xgrp |
                                               FileSystem::acc_roth |
                                               FileSystem::acc_xoth) {
  return FileSystem::mkdirIfNotExistsParents(dirname, atts);
}

inline bool setCurrentDirectory(StrView dirname) { return FileSystem::cd(dirname); }

inline bool getFileStat(StrView filename, FileInfo *fi) {
  return FileSystem::getStat(filename, fi);
}

inline FileInfo getFileStat(StrView filename) {
  FileInfo fi;
  if (!getFileStat(filename, &fi))
    throwError("Cannot get stat for: %s", filename);
  return fi;
}

} // namespace apl
