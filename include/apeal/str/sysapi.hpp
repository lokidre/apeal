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
#include "../debug/debug_trap.hpp"
#include "../index.hpp"


#include <codecvt>
#include <span>
#include <version>

#include "../windows/windows.hpp"

namespace apl {

inline Index apiUtf8ToUtf16(StrView utf8, std::basic_string<wchar_t> &utf16) noexcept
{
    Index length{};

#if APEAL_OS_WINDOWS
    utf16.clear();
    utf16.resize(1024);
    for (;;) {
        length = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), utf16.data(), (int)utf16.size()-1);
        if (length)
            break;
        if (!length) {
            if (auto err = ::GetLastError(); ERROR_INSUFFICIENT_BUFFER == err) {
                utf16.resize(utf16.size()*2);
                continue;
            }
            // In every other case we just fail
            return 0;
        }
    }
    utf16[length] = 0;
    utf16.resize(length);
    //buffer.resize(length);  // The string is null terminated
#else
    ADebugTrap();
    //std::wstring_convert<std::codecvt<char16_t, char8_t, std::mbstate_t>> conv;
    //utf16 = conv.from_bytes(utf8);
#endif

    return length;
}


inline Index apiUtf16ToUtf8(std::basic_string_view<wchar_t> utf16, std::basic_string<char> &utf8) noexcept
{
    utf8.clear();

#if APEAL_OS_WINDOWS
    utf8.resize(1024);
    int length{};
    for (;;) {
        length = ::WideCharToMultiByte(CP_UTF8, 0,
                utf16.data(), (int)utf16.size(),
                utf8.data(), (int)utf8.length() - 1,
                0, 0);

        if (length)
            break;
        if (length == 0) {
            auto err = ::GetLastError();
            if (err == ERROR_INSUFFICIENT_BUFFER) {
                utf8.resize(utf8.length() * 2);
                continue;
            }
            // In every other case we just fail
            utf8.clear();
            return 0;
        }
    }
    utf8[length] = 0;
    utf8.resize(length);
#else
    ADebugTrap();
    //std::wstring_convert<std::codecvt_utf8<ASysApiChar>, ASysApiChar> ucs2conv;
    //return ucs2conv.to_bytes(str);
#endif

    return utf8.length();
}

/*
inline String apiFromUtf16(const wchar_t *str) noexcept
{
    String buffer; // start with some large number

#if APEAL_OS_WINDOWS
    buffer.resize(1024);
    int length{};
    for (;;) {
        //length = ::WideCharToMultiByte(CP_UTF8, 0, str.data(), str.size(), buffer.data(), (int)buffer.length()-1, 0, 0);
        length = ::WideCharToMultiByte(CP_UTF8, 0, str, -1, buffer.data(), (int)buffer.length()-1, 0, 0);
        if (length)
            break;
        if (length == 0) {
            auto err = ::GetLastError();
            if (err == ERROR_INSUFFICIENT_BUFFER) {
                buffer.resize(buffer.length()*2);
                continue;
            }
            // In every other case we just fail
            return String{};
        }
    }
    buffer.resize(std::size_t(length) - 1);  // The string is null terminated
#else
    ADebugTrap();
    //std::wstring_convert<std::codecvt_utf8<ASysApiChar>, ASysApiChar> ucs2conv;
    //return ucs2conv.to_bytes(str);
#endif

    return buffer;
}
*/

inline String apiFromUtf16(const wchar_t *str) noexcept
{
    String utf8;
    apiUtf16ToUtf8({str, wcslen(str)}, utf8);
    return utf8;
}

//inline String apiFromWin(std::basic_string_view<const wchar_t> str) noexcept
inline String apiFromWin(const wchar_t* str) noexcept
{
    return apiFromUtf16(str);
}

inline String apiFromUtf8(const char* str) noexcept
{
    return String{str};
}


struct ApiString {

//#if __has_cpp_attribute(__cpp_lib_constexpr_string)
//    constexpr
//#endif
    ApiString() noexcept {}

//#if __has_cpp_attribute(__cpp_lib_constexpr_string)
//    constexpr
//#endif
    ApiString(std::string_view i) noexcept: haveView_{true}, view_{i} {}

    auto& operator=(std::string_view i) {
        haveView_ = haveUtf8_ = haveUtf16_ = false;

        haveView_ = true;
        view_ = i;
        return *this;
    }

    auto& operator=(std::wstring i) {
        haveView_ = haveUtf8_ = haveUtf16_ = false;

        haveUtf16_ = true;
        utf16_ = i;
        utf16Length_ = i.size();
        return *this;
    }




    auto empty() const noexcept {
        if (haveUtf8_ && !utf8_.empty())
            return false;
        if (haveUtf16_ && !utf16_.empty())
            return false;
        if (haveView_ && !view_.empty())
            return false;
        return true;
    }

    void prepareUtf16()
    {
        if (haveUtf16_)
            return;

        if (haveView_) {
            utf16Length_ = apiUtf8ToUtf16(view_, utf16_);
            haveUtf16_ = true;
            return;
        }

        ADebugTrap();
    }

    const wchar_t *utf16() {
        prepareUtf16();
        return utf16_.data();
    }

    Index utf16Length() {
        prepareUtf16();
        return utf16Length_;
    }

    const char *utf8() {
        if (haveUtf8_)
            return utf8_.data();
        if (haveView_) {
            utf8_.assign(view_);
            haveView_ = true;
            return utf8_.data();
        }
        if (haveUtf16_) {
            apiUtf16ToUtf8(utf16_, utf8_);
            haveUtf8_ = true;
            return utf8_.data();
        }
        ADebugTrap();
        return nullptr;
    }

    const char *c_str() noexcept { return utf8();  }

    // Windows API string
    auto win() {  return utf16();  }
    auto winLength() {  return (int)utf16Length();  }

    std::span<const char> data() {
        if (haveView_)
            return view_;
        ADebugTrap();
        return {};
    }

    constexpr auto charSize() const {  return sizeof(char);  }

    auto& operator+=(const std::string_view str) {
        if (haveUtf8_) {
            haveUtf16_ = haveView_ = false;
            utf8_ += str;
            return *this;
        }

        if (haveView_) {
            haveUtf16_ = haveView_ = false;
            haveUtf8_ = true;
            utf8_ = view_;
            utf8_ += str;
            return *this;
        }

        if (haveUtf16_) {
            std::basic_string<wchar_t> temp;
            apiUtf8ToUtf16(str, temp);
            utf16_ += temp;
            return *this;
        }

        haveUtf8_ = true;
        utf8_ = str;
        return *this;
    }

private:
    bool haveView_{};
    std::string_view view_{};

    bool haveUtf16_{};
    Index utf16Length_{};
    //std::vector<wchar_t> utf16_{};
    std::basic_string<wchar_t> utf16_{};

    bool haveUtf8_{};
    std::string utf8_{};
};


}  // namespace apl
