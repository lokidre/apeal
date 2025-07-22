/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/time.hpp"


namespace apl {


void Time::setLocal() noexcept{

#if APEAL_OS_WINDOWS

        SYSTEMTIME st;
        ::GetLocalTime(&st);
        setWindowsTime(st);

#elif APEAL_OS_POSIX

        time_t cur_time;
        tm t_;

        time(&cur_time);
        localtime_r(&cur_time, &t_);

        MilliSec = 0;
        Sec = t_.tm_sec;
        Min = t_.tm_min;
        Hour = t_.tm_hour;
        Day = t_.tm_mday;
        Month = t_.tm_mon + 1;
        Year = t_.tm_year + 1900;

        WeekDay = t_.tm_wday;
        YearDay = t_.tm_yday;

        Dst = t_.tm_isdst;
#endif  // Win32 api
}


#if APEAL_OS_WINDOWS
    void Time::setWindowsTime(const SYSTEMTIME& st)
    {
        MilliSec = st.wMilliseconds;
        Sec = st.wSecond;
        Min = st.wMinute;
        Hour = st.wHour;
        Day = st.wDay;
        Month = st.wMonth;
        Year = st.wYear;
        WeekDay = st.wDayOfWeek;
    }
#endif


Time fromUnixTime(time_t ut) noexcept
{
    tm tmtBuf;
    tm* tmt{};

#if APEAL_OS_POSIX
    tmt = ::gmtime_r(&ut, &tmtBuf);
#else
    gmtime_s(&tmtBuf, &ut);
    tmt = &tmtBuf;
#endif

    Time t{};

    t.Sec = tmt->tm_sec;
    t.Min = tmt->tm_min;
    t.Hour = tmt->tm_hour;
    t.Day = tmt->tm_mday;
    t.Month = tmt->tm_mon + 1;
    t.Year = tmt->tm_year + 1900;
    t.WeekDay = tmt->tm_wday;
    t.YearDay = tmt->tm_yday;
    t.Dst = tmt->tm_isdst;

    return t;
}


time_t toUnixTime(const Time& t){
    tm tmt;

    tmt.tm_sec = t.Sec;
    tmt.tm_min = t.Min;
    tmt.tm_hour = t.Hour;
    tmt.tm_mday = t.Day;
    tmt.tm_mon = t.Month - 1;
    tmt.tm_year = t.Year - 1900;
    tmt.tm_wday = t.WeekDay;
    tmt.tm_yday = t.YearDay;
    tmt.tm_isdst = t.Dst;

    return std::mktime(&tmt);
}


#if APEAL_OS_WINDOWS

Time fromWindowsTime(const SYSTEMTIME& st) noexcept{
    Time t{};

    t.MilliSec = st.wMilliseconds;
    t.Sec = st.wSecond;
    t.Min = st.wMinute;
    t.Hour = st.wHour;
    t.Day = st.wDay;
    t.Month = st.wMonth;
    t.Year = st.wYear;
    t.WeekDay = st.wDayOfWeek;

    return t;
}


Time fromWindowsFileTime(const FILETIME& ft) noexcept{
    SYSTEMTIME st;
    FileTimeToSystemTime(&ft, &st);
    return fromWindowsTime(st);
}

SYSTEMTIME toWindowsTime(const Time& t){
    SYSTEMTIME st{};

    st.wMilliseconds = (WORD)t.MilliSec;
    st.wSecond = (WORD)t.Sec;
    st.wMinute = (WORD)t.Min;
    st.wHour = (WORD)t.Hour;
    st.wDay = (WORD)t.Day;
    st.wMonth = (WORD)t.Month;
    st.wYear = (WORD)t.Year;
    st.wDayOfWeek = (WORD)t.WeekDay;

    return st;
}

#endif


uint64_t getTimerTicks() noexcept{
    uint64_t ticks = 0;

#if APEAL_OS_WINDOWS

    ticks = ::GetTickCount64();

#elif APEAL_OS_POSIX
    // #if TARGET_OS_UNIX

    double t = clock();

    ticks = (uint64_t)((t*1000*100)/CLOCKS_PER_SEC);

#endif

    return ticks;
}


} // namespace


