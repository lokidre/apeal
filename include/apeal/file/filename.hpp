/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "../string.hpp"
#include <tuple>



#if APEAL_OS_POSIX
#include <pwd.h>
#endif


//
// File name manipulation functions
//

namespace apl {


//
// Inquiry
//

constexpr
bool isBundleFileName(StrView filename) {
    return !filename.empty() && filename.front() == ':';
}


//inline void fixFilePath(String &path) {
//    // Get rid of backslashes completely
//    for (auto &c: path)
//        if (c == '\\')
//            c = '/';
//}


constexpr
bool isDirNameOpen(StrView dirname) noexcept {
    if (dirname.empty())
        return false;

    auto &c = dirname.back();
    return c != '/' && c != '\\';
}




//
// Parsing and splitting
//
template <class View>
constexpr std::pair<View, View> splitFilePathCore(View in) noexcept
{
    std::size_t nameCount{};

    // Start from the back
    auto begin = in.rbegin(), end = in.rend();
    auto it = begin;


    auto sep = it;

    // Try to read the file name
    for (; it != end; ++it, ++nameCount) {
        auto c = *it;
        if (c == ':' || c == '/' || c == '\\') // End of file name
            break;
        sep = it;
    }

    // No dir component
    if (it == end)
        return {{}, in};

    // no file name
    if (it == begin)
        return {in, {}};

    auto name = View{&*sep, nameCount};

    // The directory may need one character
    sep = it;

    // skip any repeating slashes
    for (; it != end; ++it) {
        if (*it != '/' && *it != '\\')
            break;
        sep = it;  // keep the last one
    }

    // Check if this is something like this: "/name"
    if (it == end)
        return {{&*sep, 1}, name};

    std::size_t dirCount{};

    for (; it != end; ++it)
        ++dirCount;

    return {{in.data(), dirCount}, name};
}

template <class View>
constexpr std::pair<View, View> splitFileExtCore(View in) noexcept
{
    std::size_t extCount{};

    auto it = in.rbegin(), end = in.rend();

    auto sep = it;

    for (; it != end; ++it, ++extCount) {

        if ('/' == *it || '\\' == *it)
            return {in, {}};  // The is no extension

        if ('.' == *it)
            break;
        sep = it;  // keep the last one
    }

    // if there is no extension
    if (it == end)
        return {in, {}};

    ++it;  // skip the dot

    // if there is still no extension: ".file" case
    if (it == end)
        return {in, {}};


    View ext{&*sep, extCount};

    std::size_t nameCount{};  // it already points to a character
    for(; it != end; ++it, ++nameCount) {}

    return {{in.data(), nameCount}, ext};
}


template <class View>
constexpr std::tuple<View, View, View> parseFilePathCore(View in) noexcept
{
    auto [path, ext] = splitFileExtCore(in);
    auto [dir, name] = splitFilePathCore(path);

    return {dir, name, ext};
}


// splitFileExt("some/path/file.ext") -> pair{"some/path/file", "ext"}
constexpr 
std::pair<StrView, StrView> splitFileExt(StrView in) noexcept {
    return splitFileExtCore(in); 
}


// splitFilePath("some/path/to/file.ext") -> pair{"some/path/to", "file.ext"}
constexpr 
std::pair<StrView, StrView> splitFilePath(StrView in) noexcept {
    return splitFilePathCore(in); 
}


// parseFilePath("some/path/to/file.ext") -> tuple{"some/path/to", "file", "ext"}
constexpr 
std::tuple<StrView, StrView, StrView> parseFilePath(StrView in) noexcept {
    return parseFilePathCore(in); 
}

// extractFileExt("some/path/to/file.ext") -> "ext"
constexpr
StrView extractFileExt(StrView in) noexcept { 
    return splitFileExt(in).second; 
}


// extractFileName("some/path/to/file.ext") -> "file.ext"
constexpr
StrView extractFileName(StrView in) noexcept { 
    return splitFilePath(in).second; 
}

// extractFileBase("some/path/to/file.ext") -> "file"
constexpr
StrView extractFileBase(StrView in) noexcept { 
    return std::get<1>(parseFilePath(in)); 
}


// extractDirName("some/path/to/file.ext") -> "some/path/to"
constexpr
StrView extractDirName(StrView in, StrView def={}) noexcept {
    auto dir = splitFilePath(in).first;
    return dir.empty() ? def : dir;
}




//
// Modification
//

inline 
void fixDirName(String &name) noexcept {
    if (isDirNameOpen(name))
        name += '/';
}


//
// Generating file names
//

inline 
String makeFileName(StrView name, StrView ext) noexcept
{
    String fn{name};
    if (!ext.empty()) {
        fn += '.';
        fn += ext;
    }
    return fn;
}


// TODO: implement varargs
inline 
String makeFilePath(StrView dir, StrView file) noexcept
{
    if (dir.empty())
        return String{file};

    String path{dir};
    fixDirName(path);
    path += file;
    return path;
}

inline
String makeFilePath(StrView dir, StrView file1, StrView file2) noexcept
{
    return makeFilePath(makeFilePath(dir, file1), file2);
}


inline 
String makeFilePathExt(StrView dir, StrView file, StrView ext) noexcept {
    return makeFilePath(dir, makeFileName(file, ext));
}





//
// Substitute filename in path with filename
//
inline 
String substFileName(StrView path, StrView filename) {
    // Quick and dirty, needs better
    auto idx = path.size();
    while (idx > 0) {
        auto &c = path[idx-1];
        if (c == '/' || c == '\\' || c == ':')
            break;
        --idx;
    }

    if (idx) {
        String s;
        s.reserve(idx + filename.size());
        s = path.substr(0, idx);
        s += filename;
        return s;
    }

    return String{filename};
}



//
// OS dependent function
//

inline 
String cvtFileToPosix(StrView name) noexcept 
{
    String p;
    p.reserve(name.size());
    for (auto&& ch: name) {
        if ('\\' == ch)
            p += '/';
        else
            p += ch;
    }
    return p;
}


//
// Useful functions
//
inline 
String processPathName(StrView filename) noexcept
{
    if (filename.empty())
        return {};

    String name;

#if APEAL_OS_WINDOWS

    // Handle the tilde
    if (filename[0] == '~') {
        wchar_t dirBuffer[MAX_PATH+1];
        DWORD bufferLength = MAX_PATH;
        if (GetCurrentDirectoryW(bufferLength, dirBuffer)) {
            name = apiFromUtf16(dirBuffer);

            // keep only two the drive and the semicolon
            if (name.size() > 2)
                name.erase(2);
            name += filename.substr(1);
        } else {
            name = filename;
        }
    } else {
        name = filename;
    }

#else

    // Handle the tilde
    if (filename[0] == '~') {
        // TODO: implement getpwuid_r
        const char *homedir;

        if ((homedir = getenv("HOME")) == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }

        name = homedir;
        name += filename.substr(1);
    } else {
        name = filename;
    }

#endif

    return name;

}




#if 0 // TODO: matching
inline
bool FileNameMatch(const char *Mask, const char *Name, bool)
{
    int imask, iname;
    char m, n;

    // go through the mask
    imask = iname = 0;
    for (;;) {
        m = Mask[imask], n = Name[iname];

        if (m == '\0' && n == '\0')  // if name and mask finished 
            break;                      // they match

        if (m == '\0') {  // if mask finished
          // previous symbol should've been '*'
            if (imask > 0 && Mask[imask-1] == '*')
                return true;
            // any other case - don't match
            return false;
        }

        if (n == '\0') {  // if name finished
          // all remaining symbols of mask must be wildcards
            for (; Mask[imask]; imask++)
                if (Mask[imask] != '?' && Mask[imask] != '*')
                    return false;

            // if we are here - all remaining symbols in mask are wildcards
            return true;
        }

        //if ( IgnoreCase )
        //  m = Lowercase(m), n = Lowercase(n) ;

        // check for wildcard
        switch (m) {
        case '?':  // pass one symbol - any symbol fits
            imask++, iname++;  // names fit
            break;

        case '*':  // pass all symbols until equal symbols
          // find next not wildcard symbol in the mask
            for (imask++; Mask[imask] == '?' && Mask[imask] == '*'; imask++) {
                if (!Mask[imask])  // if it was the last symbol in mask
                    return true;      // name matches
            }

            // find next symbol in the mask
            m = Mask[imask];

            if (m == 0)
                return true;

            //if ( IgnoreCase )
            //  m = Lowercase(m) ;

            for (; m != Name[iname]; iname++)
                if (Name[iname] == '\0')  // if we didn't find in the name
                    return false;

            // !!!!!!! needed "*cpp" <> "csdfsdfcpp" - should match

            // if we r here - continue
            iname++, imask++;
            break;

        default:
            if (m != n)
                return false;
            imask++, iname++;  // next character
            break;
        }

    }

    return true;
}
#endif


}

