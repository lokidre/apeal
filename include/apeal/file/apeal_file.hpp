/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../debug.hpp"
#include "../string.hpp"


#if APEAL_OS_POSIX

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 1
#endif

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <utime.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <stdlib.h>

#if APEAL_OS_IPHONE
#include "../platform/ios.hpp"
#endif

#if APEAL_OS_ANDROID
#include "../platform/android.hpp"
#endif

#if APEAL_OS_MAC
#include "../platform/macos.hpp"
#endif

#endif

// Qt resource loading support
#ifdef QT_CORE_LIB
#include "../platform/qt.hpp"

APEAL_QT_HEADERS_BEGIN
#include <QFile>
APEAL_QT_HEADERS_END
#endif



namespace apl {

// File information structure
struct FileInfo {
    uint32_t attributes{};  // File attributes
    Time creation;
    Time access;
    Time write;
    Index size{};  // File size
};


enum class FileAccess: unsigned {
#if APEAL_OS_WINDOWS
    Read = GENERIC_READ,
    Write = GENERIC_WRITE,
    //ReadWrite = GENERIC_READ | GENERIC_WRITE,
#else
    Read = 01, // = O_RDONLY,
    Write = 02, // = O_WRONLY,
    //ReadWrite = 0,
#endif
    ReadWrite = Read | Write,
};

template<> struct EnumBitMask<FileAccess>: std::true_type {};

using AFileAccess = FileAccess;



enum class FileCreate: unsigned {

#if APEAL_OS_WINDOWS
    CreateNew = CREATE_NEW,
    OpenExisting = OPEN_EXISTING,
    CreateAlways = CREATE_ALWAYS,
    OpenAlways = OPEN_ALWAYS,
    Truncate = TRUNCATE_EXISTING,
#endif

#if APEAL_OS_POSIX
    CreateNew = (O_CREAT | O_EXCL),
    OpenExisting = 0,
    CreateAlways = (O_CREAT | O_TRUNC),
    OpenAlways = O_CREAT,
    Truncate = O_TRUNC,
#endif

};

template<> struct EnumBitMask<FileCreate>: std::true_type {};

using AFileCreate = FileCreate;


enum class FileOrigin: unsigned {
#if APEAL_OS_WINDOWS
    Begin = FILE_BEGIN,
    Current = FILE_CURRENT,
    End = FILE_END,
#endif

#if APEAL_OS_POSIX
    Begin = SEEK_SET,
    Current = SEEK_CUR,
    End = SEEK_END,
#endif
};

template<> struct EnumBitMask<FileOrigin>: std::true_type {};


enum class FileShare: unsigned {
#if APEAL_OS_WINDOWS
    Read = FILE_SHARE_READ,
    Write = FILE_SHARE_WRITE,
#endif

#if APEAL_OS_POSIX
    // On Unix sharing is handled by the system and are shared
    // The files need to be locked if sharing is not desired
    Read = 0,
    Write = 0,
#endif
    Compat = 0,
};

template<> struct EnumBitMask<FileShare>: std::true_type {};


//
// Lots of file attributes. Some make sense only on Windows some only on Unix
//
enum class FileAttribute: unsigned {

#if APEAL_OS_WINDOWS
    Normal = FILE_ATTRIBUTE_NORMAL,
    ReadOnly = FILE_ATTRIBUTE_READONLY,
    Hidden = FILE_ATTRIBUTE_HIDDEN,
    System = FILE_ATTRIBUTE_SYSTEM,
    Label = 0,
    Dir = FILE_ATTRIBUTE_DIRECTORY,
    Archive = FILE_ATTRIBUTE_ARCHIVE,
    Temp = FILE_ATTRIBUTE_TEMPORARY,
    Compressed = FILE_ATTRIBUTE_COMPRESSED,
    Offline = FILE_ATTRIBUTE_OFFLINE,
    //Exec = 0,
    ReadUser = 0, WriteUser = 0, ExecUser = 0,
    ReadGroup = 0, WriteGroup = 0, ExecGroup = 0,
    ReadOther = 0, WriteOther = 0, ExecOther = 0,
#endif

#if APEAL_OS_POSIX
    Normal = (S_IFREG | S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH),
    ReadOnly = (S_IFREG | S_IRUSR | S_IRGRP | S_IROTH),
    Hidden = Normal,
    System = Normal,
    Label = Normal,
    Dir = S_IFDIR,
    Archive = Normal,
    Temporary = Normal,
    Compressed = Normal,
    Offline = Normal,
    //Exec = ( S_IXUSR ),
    ReadUser = S_IRUSR,
    WriteUser = S_IWUSR,
    ExecUser = S_IXUSR,
    ReadGroup = S_IRGRP,
    WriteGroup = S_IWGRP,
    ExecGroup = S_IXGRP,
    ReadOther = S_IROTH,
    WriteOther = S_IWOTH,
    ExecOther = S_IXOTH,
#endif

    Exec = ExecUser,

    //      att_all_files = FILE_ATTRIBUTE_ALL_FILES,
    //      att_all_dirs = FILE_ATTRIBUTE_ALL_DIRS

};

template<> struct EnumBitMask<FileAttribute>: std::true_type {};



//
// File class
// It's noncopyable because it handles resource, but can be movable
//
class File: NonCopyable {

public:
#if APEAL_OS_WINDOWS
    using Handle = HANDLE;
    static constexpr uintptr_t InvalidHandle = uintptr_t(-1);
#else
    using Handle = int;
    static constexpr Handle InvalidHandle = -1;
#endif
    using string_type = String;

    using Size = std::size_t; // on 64 bit machines it's 64 uint64_t ;
    using size_type = Size;


private:
    Handle handle_{Handle(InvalidHandle)};  // Handle of the file
    bool opened_{false};

private:
    // Shortcuts
    using Att = FileAttribute;

public:
    using CreateMode = FileCreate;
    using AccessMode = FileAccess;
    using ShareMode = FileShare;
    using Origin = FileOrigin;

private:
    // File name
    String name_;
    FileAttribute defaultFileAttributes_{Att::ReadUser | Att::WriteUser | Att::ReadGroup | Att::ReadOther};


    //
    // Using bundles as file sources
    //
    bool fromBundle_{false};

#if APEAL_OS_ANDROID
    bool useAndroidMgr_{false};
    AndroidAssetManager androidMgr_;
#endif

#ifdef QT_CORE_LIB
    bool useQFile_{false};
    QFile qfile_;
#endif





public:  // initializatoin
    File() noexcept {}
    ~File() noexcept {
        try {
            close();
        }
        catch (...) {
            ADebugPrintFunc("Destructor exception");
        }
    }

    // Make it movable
    // TODO:


    //
    // Status
    //

    bool opened() const { return opened_; }
    static bool validHandle(Handle handle) { return handle != Handle(InvalidHandle); }
    bool valid() const { return validHandle(handle_); }



    //
    // File open-creation functions
    //
    void create(StrView filename, CreateMode createMode = CreateMode::CreateAlways, AccessMode accessMode = AccessMode::ReadWrite)
    {
        create(filename, createMode, accessMode, defaultFileAttributes_, ShareMode::Read | ShareMode::Write);
    }

    void create(StrView filename, CreateMode createMode, AccessMode accessMode, Att attributes, ShareMode shareMode)
    {
        // if file open - close it
        close();

        name_ = processPathName(filename);
        fromBundle_ = isBundleFileName(name_);


#if APEAL_OS_POSIX
        int flags = 0;
        mode_t mode = (mode_t)defaultFileAttributes_;

        if (isSet(accessMode, AccessMode::Read) && isSet(accessMode, AccessMode::Write))
            flags |= O_RDWR;
        else if (isSet(accessMode, AccessMode::Read))
            flags |= O_RDONLY;
        else if (isSet(accessMode, AccessMode::Write))
            flags |= O_WRONLY;

        if (createMode == CreateMode::CreateAlways)
            flags |= O_CREAT | O_TRUNC;
        else if (createMode == CreateMode::OpenAlways)
            flags |= O_CREAT;
        else if (createMode == CreateMode::Truncate)
            flags |= O_TRUNC;
#endif


        //
        // Try to use bundle
        //
        if (fromBundle_) {
#if APEAL_OS_ANDROID
            handle_ = androidMgr_.openFile(name_);
            if (handle_ != InvalidHandle) {
                useAndroidMgr_ = true;
                opened_ = true;
                return;
            }
#endif
            // If we can't load from bundle, then just continue reqular, let it throw error if any
#ifdef QT_CORE_LIB
            QIODevice::OpenMode qmode{};
            if (isSet(accessMode, AccessMode::Read) && isSet(accessMode, AccessMode::Write))
                qmode = QIODevice::ReadWrite;
            else if (isSet(accessMode, AccessMode::Read))
                qmode = QIODevice::ReadOnly;
            else if (isSet(accessMode, AccessMode::Write))
                qmode = QIODevice::WriteOnly;

            qfile_.setFileName(apl::toQt(name_));
            if (qfile_.open(qmode)) {
                opened_ = true;
                useQFile_ = true;
                qfile_.handle();
                return;
            }
#endif
            fromBundle_ = false;  // fallback
        }


#if APEAL_OS_WINDOWS
        handle_ = ::CreateFileW(
            ApiString{name_}.win(),     // Zero terminated file name
            (DWORD)accessMode,  // Open mode in Windows NT format
            (DWORD)shareMode,  // Share mode
            NULL,  // Security
            (DWORD)createMode,
            (DWORD)attributes,
            NULL  // Template Flag (Windows 95 not supported)
        );

        verifyResult(handle_ != INVALID_HANDLE_VALUE);
        opened_ = true;
#endif

#if APEAL_OS_POSIX
        handle_ = ::open64(name_.c_str(), flags, mode);

        verifyResult(handle_ != -1);
        opened_ = true;
#endif

#if APEAL_OS_XAOS
        // needed
        XaosParam parm;
        parm.object = this;
        parm.buffer = name_.c_str();
        parm.param = create_mode;
        parm.param2 = AccessMode;
        parm.param3 = FileFlagAttribute;
        parm.param4 = ShareMode;
        Result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_CREATE) ? true : false;
#endif
    }


    // Open file
    void open(StrView filename, AccessMode accessMode = AccessMode::Read | AccessMode::Write)
    {
        auto shareMode = isSet(accessMode, AccessMode::Write) ? ShareMode::Compat : ShareMode::Read;
        create(filename, CreateMode::OpenExisting, accessMode, Att::Normal, shareMode);
    }

    void close()
    {
        if (!opened())
            return;

        if (fromBundle_) {
#if APEAL_OS_ANDROID
            if (useAndroidMgr_) {
                androidMgr_.closeFile();
                opened_ = false;
                return;
            }
#endif

#ifdef QT_CORE_LIB
            if (useQFile_) {
                qfile_.close();
                opened_ = false;
                return;
            }
#endif
        }

#if APEAL_OS_WINDOWS
        auto result = ::CloseHandle(handle_);
        ADebugTrapAssert(result);
#endif

#if APEAL_OS_POSIX
        [[maybe_unused]]
        auto res = ::close(handle_);
        ADebugTrapAssert(res == 0);
#endif

#if APEAL_OS_XAOS
        XaosParam parm;
        parm.object = this;
        result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_CLOSE) ? true : false;
#endif
        handle_ = Handle(InvalidHandle);
        opened_ = false;
    }

    // Read-write file
    Index readSome(void *buffer, Index size)
    {
        if (fromBundle_) {
#if APEAL_OS_ANDROID
            if (useAndroidMgr_)
                return androidMgr_.readFile(buffer, size);
#endif

#ifdef QT_CORE_LIB
            if (useQFile_) {
                auto count = qfile_.read((char *)buffer, size);
                verify(count >= 0, "%s: %s", fromQt(qfile_.errorString()), name_);
                return static_cast<Index>(count);
            }
#endif
        }

#if APEAL_OS_WINDOWS
        DWORD count = 0;

        auto result = ::ReadFile(handle_, buffer, (DWORD)size, &count, 0);

        verifyResult(result);

        return static_cast<Index>(count);
#endif


#if APEAL_OS_POSIX
        auto count = ::read(handle_, buffer, (size_t)size);

        verifyResult(count >= 0);

        return Size(count);
#endif

#if APEAL_OS_XAOS
        tXaosParam parm;
        parm.object = this;
        parm.buffer = Buffer;
        parm.param = nBytes;
        Result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_READ) ? true : false;
#endif
    }

    Index writeSome(const void *buffer, Index size)
    {
        verify(!fromBundle_, "TODO: File::write not implemented");

#if APEAL_OS_WINDOWS

        DWORD bytes = 0;

        auto result = ::WriteFile(handle_, buffer, (DWORD)size, &bytes, NULL);

        verifyResult(result);

        return static_cast<Index>(bytes);

#elif APEAL_OS_POSIX

        //if ( fromBundle_ ) {
        //  #if APEAL_OS_ANDROID
        //  count = androidMgr_.writeFile ( buffer, size ) ;
        //  #endif
        //} else {
        auto count = ::write(handle_, buffer, (size_t)size);
        verifyResult(count >= 0);
        return static_cast<Index>(count);
        //}

#elif __OS == __OS_XAOS
        XaosParam parm;
        parm.object = this;
        parm.buffer = Buffer;
        parm.param = nBytes;
        result = ::xaos_api(&parm, API_CLASS_FILE, API_FILE_WRITE) ? true : false;
#endif
    }

    Index read(void *buffer, Index size)
    {
        auto ptr = reinterpret_cast<uint8_t*>(buffer);
        Index total{};
        while (total < size) {
            auto count = readSome(ptr, size - total);
            if (count == 0)
                break;
            total += count;
            ptr += count;
        }
        return total;
    }

    Index write(const void *buffer, Index size)
    {
        auto ptr = (const uint8_t*)buffer;
        Index total = 0;
        while (total < size) {
            auto count = writeSome(ptr, size - total);
            if (count == 0)
                break;
            total += count;
            ptr += count;
        }
        return total;
    }



    // Write string to a file
    Index write(StrView s) { return write(s.data(), s.size()); }

    template <class Container>
    Index writeContainer(const Container& data) {
        return write(data.data(), data.size());
    }

    // Write one character to a file
    template <class C>
    Size writeChar(C c) {
        return write(&c, sizeof(c));
    }

    // Flush file buffers
    void flush()
    {
        verify(!fromBundle_, "TODO: File::flush() not implemented");

#if APEAL_OS_WINDOWS

        ::FlushFileBuffers(handle_);

#elif APEAL_OS_POSIX

        ::fsync(handle_);

#elif APEAL_OS_XAOS

        tXaosParam parm;
        parm.object = this;
        Result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_FLUSH) ? true : false;

#endif
    }


    // Sets file position
    Index seek(Index position, Origin origin = Origin::Begin)
    {
        verify(!fromBundle_, "TODO: File::seek not implemented");

#if APEAL_OS_WINDOWS
        LARGE_INTEGER apiPosition;
        apiPosition.QuadPart = position;

        LARGE_INTEGER apiNewPosition;
        auto result = ::SetFilePointerEx(handle_, apiPosition, &apiNewPosition, (DWORD)origin);

        verifyResult(result);

        return static_cast<Index>(apiNewPosition.QuadPart);

#elif APEAL_OS_POSIX

        //if ( fromBundle_ ) {
        //  #if APEAL_OS_ANDROID
        //  androidMgr_.seek ( position, origin ) ;
        //  #endif
        //} else {
        auto result = ::lseek64(handle_, (off64_t)position, (int)origin);
        
        verifyResult(result != -1);
        return static_cast<Index>(result);

#elif APEAL_OS_XAOS
        tXaosParam parm;
        parm.object = this;
        parm.size = Offset;
        parm.param = Origin;
        Result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_SET_POSITION) ? true : false;
#endif
    }

    Index position()
    {
        verify(!fromBundle_, "TODO: File::position() not implemented");
        //if ( fromBundle_ ) {
        //  #if APEAL_OS_ANDROID
        //  position = androidMgr_.seek ( 0, SEEK_CUR ) ;
        //  #endif
        //} else {

#if APEAL_OS_WINDOWS
        LARGE_INTEGER liPos{};

        auto result = ::SetFilePointerEx(handle_, {}, &liPos, FILE_CURRENT);

        verifyResult(result);

        return static_cast<Index>(liPos.QuadPart);
#endif

#if APEAL_OS_POSIX
        auto pos = ::lseek64(handle_, 0, SEEK_CUR);
        verifyResult(pos != -1);
        return static_cast<Index>(pos);
#endif
    }


    // Sets EOF (Cuts) file at current file pointer position
    void trunc()
    {
        verify(!fromBundle_, "TODO: File::trunc() not implemented");

#if APEAL_OS_WINDOWS

        auto result = ::SetEndOfFile(handle_);
        verifyResult(result);

#elif APEAL_OS_POSIX

        auto length = ::lseek(handle_, 0, SEEK_CUR);
        verifyResult(length != -1);

        auto result = ::ftruncate(handle_, length);
        verifyResult(result != -1);

#elif APEAL_OS_XAOS

        tXaosParam parm;
        parm.object = this;
        Result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_TRUNCATE) ? true : false;

#endif
    }


    Index getSize()
    {
        if (fromBundle_) {
#if APEAL_OS_ANDROID
            if (useAndroidMgr_)
                return androidMgr_.getSize();
#endif

#ifdef QT_CORE_LIB
            if (useQFile_)
                return qfile_.size();
#endif
        }

#if APEAL_OS_WINDOWS
        LARGE_INTEGER liSize{};

        auto result = ::GetFileSizeEx(handle_, &liSize);

        verifyResult(result);

        return static_cast<Index>(liSize.QuadPart);
#endif

#if APEAL_OS_POSIX
        auto current = ::lseek64(handle_, 0, SEEK_CUR);
        verifyResult(current != -1);

        auto size = ::lseek64(handle_, 0, SEEK_END);
        verifyResult(current != -1);

        current = ::lseek64(handle_, current, SEEK_SET);
        verifyResult(current != -1);

        return static_cast<Index>(size);
#endif
    }

    auto size() { return getSize(); }


    // Get-set file creation, last access and last write times
    void getTime(Time *crtTime, Time *accTime, Time *wrtTime)
    {
#if APEAL_OS_WINDOWS

        FILETIME fCreationTime, fAccessTime, fWriteTime;

        auto result = GetFileTime(handle_, &fCreationTime, &fAccessTime, &fWriteTime);
        verifyResult(result);

        // Convert WinNT file time format to system time format
        SYSTEMTIME sysTime;

        if (crtTime) {
            result = FileTimeToSystemTime(&fCreationTime, &sysTime);
            verifyResult(result);
            *crtTime = fromWindowsTime(sysTime);
        }

        if (accTime) {
            result = FileTimeToSystemTime(&fAccessTime, &sysTime);
            verifyResult(result);
            *accTime = fromWindowsTime(sysTime);
        }

        if (wrtTime) {
            result = FileTimeToSystemTime(&fWriteTime, &sysTime);
            verifyResult(result);
            *wrtTime = fromWindowsTime(sysTime);
        }

#elif APEAL_OS_POSIX

        struct stat st;
        auto result = ::fstat(handle_, &st);
        verifyResult(result != -1);

        if (crtTime)
            *crtTime = fromUnixTime(st.st_ctime);
        if (accTime)
            *accTime = fromUnixTime(st.st_atime);
        if (wrtTime)
            *wrtTime = fromUnixTime(st.st_mtime);

#elif APEAL_OS_XAOS

        tXaosParam parm;
        parm.object = this;
        parm.object2 = LastWriteTime;
        parm.object3 = CreationTime;
        parm.object4 = LastAccessTime;
        Result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_GET_TIME) ? true : false;

#endif
    }

    Time getTime() {
        Time wrtTime;
        getTime(0, 0, &wrtTime);
        return wrtTime;
    }

#if 0 // TODO: implement
    void SetTime(tSystemTime *pTime) { SetTime(0, 0, pTime); }
    void SetTime(tSystemTime *CreationTime, tSystemTime *AccessTime, tSystemTime *WriteTime)
    {
#if APEAL_OS_WINDOWS

        FILETIME Win32CreationFileTime;
        FILETIME Win32LastAccessFileTime;
        FILETIME Win32LastWriteFileTime;
        FILETIME *pWin32CreationFileTime = NULL;
        FILETIME *pWin32LastAccessFileTime = NULL;
        FILETIME *pWin32LastWriteFileTime = NULL;

        SYSTEMTIME Win32CreationSystemTime;
        SYSTEMTIME Win32LastAccessSystemTime;
        SYSTEMTIME Win32LastWriteSystemTime;


        // Convert time parameters to file time
        if (CreationTime) {
            pWin32CreationFileTime = &Win32CreationFileTime;
            System2Win32SystemTime(CreationTime, &Win32CreationSystemTime);
            ::SystemTimeToFileTime(&Win32CreationSystemTime, &Win32CreationFileTime);
        }

        if (AccessTime) {
            pWin32LastAccessFileTime = &Win32LastAccessFileTime;
            System2Win32SystemTime(AccessTime, &Win32LastAccessSystemTime);
            ::SystemTimeToFileTime(&Win32LastAccessSystemTime, &Win32LastAccessFileTime);
        }

        if (WriteTime) {
            pWin32LastWriteFileTime = &Win32LastWriteFileTime;
            System2Win32SystemTime(WriteTime, &Win32LastWriteSystemTime);
            ::SystemTimeToFileTime(&Win32LastWriteSystemTime, &Win32LastWriteFileTime);
        }

        // Call Win32 api to set file time
        auto result = ::SetFileTime((HANDLE)handle_, pWin32CreationFileTime, pWin32LastAccessFileTime, pWin32LastWriteFileTime);
        verifyResult(result);

#elif APEAL_OS_POSIX

        utimbuf tb = {0, 0};

        if (AccessTime)
            SystemToUnixTime(*AccessTime, tb.actime);
        if (WriteTime)
            SystemToUnixTime(*WriteTime, tb.modtime);

        auto result = ::utime(name_.c_str(), &tb);
        verifyResult(result != -1);

#elif APEAL_OS_XAOS

        tXaosParam parm;
        parm.object = this;
        parm.object2 = LastWriteTime;
        parm.object3 = CreationTime;
        parm.object4 = LastAccessTime;
        Result = ::XaosApi(&parm, API_CLASS_FILE, API_FILE_SET_TIME) ? true : false;

#endif
    }
#endif

    // Get information about open file
    bool get_info(FileInfo &fi)
    {
        bool result = true;

#if APEAL_PLATFORM_DOS

        tWin32FileInfo Win32FileInfo;

        result = ::dos_file_get_info((word)handle_, &Win32FileInfo);

        internal_error();

#elif APEAL_OS_WINDOWS

        BY_HANDLE_FILE_INFORMATION wfi;

        result = GetFileInformationByHandle(handle_, &wfi);
        if (result) {
            fi.attributes = wfi.dwFileAttributes;
            fi.size = wfi.nFileSizeHigh;
            fi.size <<= 32;
            fi.size += wfi.nFileSizeLow;
            fi.creation = fromWindowsFileTime(wfi.ftCreationTime);
            fi.access = fromWindowsFileTime(wfi.ftLastAccessTime);
            fi.write = fromWindowsFileTime(wfi.ftLastWriteTime);
        }


#elif APEAL_OS_XAOS

        internal_error();
        result = false;

#else
        throwError("File::get_info Not implemented");

        //internal_error() ;

#endif

        return result;
    }



public:  // service functions
    void save(StrView filename, const void *d, Index s) {
        create(filename, CreateMode::CreateAlways, AccessMode::Write, Att::Normal, ShareMode::Read);

        write(d, s);
        flush();
        close();
    }

    template <class D>
    void save(StrView filename, const D &data) {
        save(filename, data.data(), data.size());
    }


private:
    [[noreturn]]
    void throwFileError(int errorCode)
    {
#if APEAL_OS_WINDOWS
        auto errorString = getErrorStringWindows(errorCode);
#else
        auto errorString = getErrorStringUnix(errorCode);
#endif
        if (!name_.empty()) {
            if (errorString.back() == '.')
                errorString.pop_back();

            errorString += ": \'" + name_ + "\'";
        }

        throwError(errorString);
    }

    [[noreturn]]
    void throwFileError()
    {
#if APEAL_OS_WINDOWS
        throwFileError(::GetLastError());
#else
        throwFileError(errno);
#endif
    }

    void verifyResult(bool cond) {
        if (!cond)
            throwFileError();
    }

};


//
// Shortcuts
// Useful for quick saving and loading small files
//
inline
void saveFile(StrView filename, const void *data, Index size)
{
    File file;
    file.create(filename, FileCreate::CreateAlways, FileAccess::Write);
    file.write(data, size);
}

template <class Container>
void saveFile(StrView filename, const Container &buffer) {
    saveFile(filename, buffer.data(), buffer.size());
}

inline void saveFileString(StrView filename, StrView data) {
    saveFile(filename, data.data(), data.size());
}

template <class Container>
void loadFile(StrView filename, Container* buffer)
{
    File file;
    file.open(filename, FileAccess::Read);
    auto size = file.size();
    buffer->resize(size);
    file.read(buffer->data(), size);
}

inline
auto loadFile(StrView filename) {
    std::string data;
    loadFile(filename, &data);
    return data;
}


}  // namespace apl
