/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

//#include "../def.hpp"

#if APEAL_OS_POSIX
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#endif

//#include <iostream>


namespace apl {

struct DirEntry {
    enum {
#if APEAL_OS_POSIX
        att_dir = S_IFDIR,
        att_exec = S_IXUSR
#else
        att_dir = FILE_ATTRIBUTE_DIRECTORY,
        att_exec = 0
#endif
    };

    String name;

#if APEAL_OS_POSIX
    mode_t att{};
    off_t size{};
#else
    unsigned long att{};
    unsigned long size{};
#endif

    Time ctime{}, atime{}, mtime{};

    bool isDir() const noexcept { return att & att_dir; };
    bool isExec() const noexcept { return att & att_exec; };
};

using FileList = std::vector<DirEntry>;
using DirEntryList = std::vector<DirEntry>;

//template <class Entry = DirEntry>
//using AEntryList = std::vector<Entry>;

//template <class file_T=ADirEntry>
//struct AEntryList: public std::vector<file_T> {
//} ;


class ADir {
public:

    using ErrorPolicy = AErrorPolicy;
    using string_type = String;
    using size_type = size_t;


    typedef FileList file_list_type;

public:
    uint32_t Handle{};
    uint32_t Error{};

    bool opt_dirs{};

    string_type name_;


public:
    ~ADir() noexcept { try { close(); } catch(...){}}

    bool open(StrView dirname)
    {
        close();

        name_ = dirname.empty() ? "." : dirname;

#if APEAL_OS_WINDOWS
        if (FindFileOpen)
            DirCloseFind(), FindFileOpen = false;

#elif APEAL_OS_POSIX
        hFind = ::opendir(ApiString(dirname).utf8());

        if (hFind == 0)
            return handle_error_(errno);

#elif APEAL_OS_XAOS

        XaosParam parm;
        parm.object = this;
        parm.buffer = DirName;
        Result = ::XaosApi(&parm, API_CLASS_FILE, API_DIRECTORY_OPEN) ? true : false;

#endif

        return true;
    }


    void close()
    {

#if APEAL_OS_WINDOWS


        if (FindFileOpen)
            DirCloseFind(), FindFileOpen = false;

#elif APEAL_OS_POSIX

        if (hFind) {
            ::closedir(hFind);
            hFind = 0;
        }

#elif APEAL_OS_XAOS

        tXaosParam parm;
        parm.object = this;
        Result = ::XaosApi(&parm, API_CLASS_FILE, API_DIRECTORY_CLOSE) ? true : false;

#endif

        Handle = 0;
    }


    int read(DirEntry* Entries, int count = 1)
    {
        bool proc_count = 1;

#if APEAL_OS_WINDOWS

        for (int ifile = 0; ifile < count; ifile++) {

            if (!FindFileOpen) {  // we have to find the first file
                if (!DirFindFirst(Entries[ifile], "*.*"))
                    return false;
                FindFileOpen = true;
            } else {  // if first file already open
                if (!DirFindNext(Entries[ifile]))
                    return false;
            }

        }  // for count



#elif APEAL_OS_POSIX

        // return 0 ;

        for (int ifile = 0; ifile < count; ifile++) {
            //struct dirent *ent ;
            //union {
            //	dirent d ;
            //	char buff[sizeof(struct dirent) + 256 + 1] ;
            //} buf ;

            auto ent = ::readdir(hFind);

            if (ent == 0)
                break;
            //if ( ::readdir_r ( hFind, &buf.d, &ent ) != 0 || ent == 0 ) {
            //	proc_count = 0 ;
            //	break ;
            //}

            //Entries[ifile].name.assign(un.d.d_name,un.d.d_reclen) ;
            Entries[ifile].name = ent->d_name;
        }

#elif APEAL_OS_XAOS

        tXaosParam parm;

        parm.object = this;
        parm.buffer = Entries;
        parm.count = Count;
        parm.object2 = NULL;  // obsolete - remove

        Result = bool(::XaosApi(&parm, API_CLASS_FILE, API_DIRECTORY_READ));

#endif


        return proc_count;
    }


    int read(StrView mask, DirEntryList& entries)
    {
        bool res = true;

        entries.clear();

#if APEAL_OS_WINDOWS

        DirEntry e;

        string_type find_mask{mask};

        if (DirFindFirst(e, find_mask.c_str())) {
            do {
                if (e.name == "." || e.name == "..")
                    continue;

                //if (opt_dirs && (e.att & FILE_ATTRIBUTE_DIRECTORY) == 0)
                //    continue;

                entries.push_back(e);
            } while (DirFindNext(e));
        }

#elif APEAL_OS_POSIX

        //struct dirent *ent = 0 ;

        //char dirent_buf[sizeof(struct dirent) + NAME_MAX+1] ;

        char stat_buf[sizeof(struct stat) + NAME_MAX+1];
        char path[NAME_MAX+1];
        
        ApiString apiMask{mask};

        for (;;) {
            // TODO: set errno to 0 and check it
            auto ent = ::readdir(hFind);
            if (ent == 0)
                break;
            //while ( ::readdir_r(hFind,(struct dirent *)dirent_buf,&ent) == 0 && ent != 0 ) {

            if (mask.empty() || fnmatch(apiMask.utf8(), ent->d_name, 0) == 0) {

                DirEntry e;

                e.att = 0;
                e.name = ent->d_name;

                if (e.name == "." || e.name == "..")
                    continue;

                strcpy(path, name_.c_str());
                strcat(path, "/");
                strcat(path, ent->d_name);

                //std::cout << path << std::endl ;

                if (lstat(path, (struct stat*)stat_buf) == 0) {
                    struct stat* s = (struct stat*)stat_buf;
                    e.att = s->st_mode;
                }

                entries.push_back(e);
            }
        }


#endif

        return res;
    }

    int read(DirEntryList& entries)
    {
        // if mask is missing presume all files
#if APEAL_OS_WINDOWS
        return read("*.*", entries);
#else
        return read("*", entries);
#endif
    }

    FileList read() {
        FileList dirs;
        read(dirs);
        return dirs;
    }

    FileList read(StrView mask) {
        FileList dirs;
        read(mask, dirs);
        return dirs;
    }


    bool setPosition(Index Position, FileOrigin origin=FileOrigin::Begin)
    {
        bool Result = true;

#if APEAL_OS_WINDOWS


        if (FindFileOpen)
            DirCloseFind(), FindFileOpen = false;

#elif APEAL_OS_XAOS

        tXaosParam parm;

        parm.object = this;
        parm.size = Position;
        parm.param = Origin;

        Result = ::XaosApi(&parm, API_CLASS_FILE, API_DIRECTORY_SET_POSITION) ? true : false;

#elif APEAL_OS_POSIX

        rewinddir(hFind);

#endif


        // needed CurrentPosition = Position

        return Result;
    }


    bool GetSize(uint32_t* Count)
    {
        bool Result = true;

#if APEAL_OS_WINDOWS

        //char FileBuffer[FILE_MAX_PATH+1] ;
        DirEntry e;

        for (*Count = 0; ; (*Count)++) {

            if (!FindFileOpen) {  // we have to find the first file
                if (!DirFindFirst(e, "*.*"))
                    return false;
                FindFileOpen = true;
            } else {  // if first file already open
                if (!DirFindNext(e))
                    break;
            }

        }  // for count

        DirCloseFind();
        FindFileOpen = false;

#elif APEAL_OS_XAOS

        tXaosParam parm;
        parm.object = this;
        Result = ::XaosApi(&parm, API_CLASS_FILE, API_DIRECTORY_GET_SIZE) ? true : false;
        *Count = dword(parm.size);

#else

        // needed
        unused(Count, Result);

#endif

        return Result;
    }



private:
    bool DirectoryOpen{};
    bool FindFileOpen{};
    //char DirCurrent[5] ;


#if APEAL_OS_WINDOWS

    HANDLE hFind{};
    uint32_t FindAttributes{};

#elif APEAL_OS_POSIX

    DIR* hFind{};

#endif

    bool DirFindFirst(DirEntry& e, const String& FileMask)
    {
        bool result = true;
        auto mask = name_;

        if (!mask.empty() && mask[mask.size()-1] != '/')
            mask += '/';

        //fs.complete_dir_name ( SearchMask ) ;

        if (!FileMask.empty())
            mask += FileMask;
        else {
#if APEAL_OS_WINDOWS
            mask += "*.*";
#else
            mask += "*";
#endif
        }


#if APEAL_OS_WINDOWS

        WIN32_FIND_DATAW fd;

        //hFind = ::FindFirstFile ( to_api_string(mask).c_str(), &Win32FindData ) ;
        hFind = ::FindFirstFileW(ApiString{mask}.win(), &fd);

        result = (hFind == INVALID_HANDLE_VALUE) ? false : true;

        if (result) {
            //from_api_string(fd.cFileName, e.name ) ;
            e.name = apiFromUtf16(fd.cFileName);
            e.att = fd.dwFileAttributes;
        }

#endif

        return result;
    }

    bool DirFindNext(DirEntry& e)
    {
        bool Result = true;

#if APEAL_OS_WINDOWS

        WIN32_FIND_DATAW wfd;

        Result = FindNextFileW(hFind, &wfd) ? true : false;

        if (Result == true) {
            e.name = apiFromUtf16(wfd.cFileName);
            e.att = wfd.dwFileAttributes;
        }

#else
        //internal_error() ;
#endif

        return Result;
    }

    bool DirCloseFind(void)
    {
        if (hFind == 0)
            return true;

#if APEAL_OS_WINDOWS

        ::FindClose(hFind);

#elif APEAL_OS_POSIX


        ::closedir(hFind);

#endif

        hFind = 0;

        return true;
    }


private:
    bool handle_error_(const int error_code)
    {
        string_type error_string;
        ErrorPolicy error_policy;

#if APEAL_OS_WINDOWS
        error_string = getErrorStringWindows(error_code);

        error_string += '\n';

#elif APEAL_OS_POSIX
        error_string = getErrorStringUnix(error_code);
#endif

        if (!name_.empty()) {
            error_string += ':';
            error_string += ' '; //" - ", 
            error_string += name_;
        }

        return error_policy(error_string);
    }

    bool handle_error_()
    {
#if APEAL_OS_WINDOWS
        return handle_error_(::GetLastError());
#elif APEAL_OS_POSIX
        return handle_error_(errno);
#elif APEAL_OS_XAOS
        return handle_error_("");
#endif
    }


};

}  // namespace apl

