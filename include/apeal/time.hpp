/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "def.hpp"
#include "util.hpp"
#include "string.hpp"
#include "str/format.hpp"

#include <ctime>
#include <cstdint>

#if APEAL_OS_POSIX
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#endif



namespace apl {

enum class TimeZone: int { None = 0, Local, UTC };

struct Time {
    int MilliSec{};  // Milliseconds (0-999)
    int Sec{};  // (0-59)
    int Min{};  // (0-59)
    int Hour{};  // (0-23)
    int Day{};  // Month day (1-31)
    int Month{};  // (1-12)
    int Year{};  // (0-)

    int WeekDay{};  // (1-7)
    int YearDay{};  // Day of year (0-355)
    int Dst{};   // Daylight saving time

    constexpr Time() noexcept {}
    Time(TimeZone tz) noexcept
    {
        if (tz == TimeZone::Local)
            setLocal();
    }

    void setLocal() noexcept;

#if APEAL_OS_WINDOWS
    void setWindowsTime(const SYSTEMTIME& st);
#endif
};


// Time/Data conversion routines
void toDosTime(Time* t, uint32_t* dt);
void fromDosTime(uint32_t* dt, Time* t);


// inline
Time fromUnixTime(time_t ut) noexcept;
// {
//     tm tmtBuf;
//     tm* tmt{};

// #if APEAL_OS_POSIX
//     tmt = ::gmtime_r(&ut, &tmtBuf);
// #else
//     gmtime_s(&tmtBuf, &ut);
//     tmt = &tmtBuf;
// #endif

//     Time t{};

//     t.Sec = tmt->tm_sec;
//     t.Min = tmt->tm_min;
//     t.Hour = tmt->tm_hour;
//     t.Day = tmt->tm_mday;
//     t.Month = tmt->tm_mon + 1;
//     t.Year = tmt->tm_year + 1900;
//     t.WeekDay = tmt->tm_wday;
//     t.YearDay = tmt->tm_yday;
//     t.Dst = tmt->tm_isdst;

//     return t;
// }

// inline
time_t toUnixTime(const Time& t);
// {
//     tm tmt;

//     tmt.tm_sec = t.Sec;
//     tmt.tm_min = t.Min;
//     tmt.tm_hour = t.Hour;
//     tmt.tm_mday = t.Day;
//     tmt.tm_mon = t.Month - 1;
//     tmt.tm_year = t.Year - 1900;
//     tmt.tm_wday = t.WeekDay;
//     tmt.tm_yday = t.YearDay;
//     tmt.tm_isdst = t.Dst;

//     return std::mktime(&tmt);
// }


#if APEAL_OS_WINDOWS
Time fromWindowsTime(const SYSTEMTIME& st) noexcept;
Time fromWindowsFileTime(const FILETIME& ft) noexcept;
SYSTEMTIME toWindowsTime(const Time& t);
#endif



// Local computer time routines
inline Time getLocalTime() noexcept {
    Time t{TimeZone::Local};
    return t;
}


//
// Time string formatting routines. Don't use any strings.
//

// datestamp format, YYYYMMDD, e.g. 20030110
// timestamp format,           e.g. 20030110130015
// string length is 14 (without terminating null)
// timestamp ex format,        e.g. 20030110120015001
// date format: yyyy-mm-dd
// time format: hh:mm:ss
// datetime format: yyyy-mm-dd hh:mm:ss

// needed obsolete
inline String formatTimestampEx(const Time& t) noexcept {
    return fmt("%04d%02d%02d%02d%02d%02d%03d", t.Year, t.Month, t.Day, t.Hour, t.Min, t.Sec, t.MilliSec);
}


// needed obsolete
inline String formatTimestamp(const Time& t) noexcept {
    return fmt("%04d%02d%02d%02d%02d%02d", t.Year, t.Month, t.Day, t.Hour, t.Min, t.Sec);
}

inline String formatDatestamp(const Time& t) noexcept {
    return fmt("%04d%02d%02d", t.Year, t.Month, t.Day);
}

inline String formatDatestampShort(const Time& t) noexcept {
    return fmt("%02d%02d%02d", t.Year%100, t.Month, t.Day);
}

inline String formatDate(const Time& t) noexcept {
    return fmt("%04d-%02d-%02d", t.Year, t.Month, t.Day);
}

inline String reformatDate(StrView t) noexcept {
    return fmt("%s-%s-%s", t.substr(0, 4), t.substr(4, 2), t.substr(6, 2));
}

inline String formatTime(const Time& t) noexcept {
    return fmt("%02d:%02d:%02d", t.Hour, t.Min, t.Sec);
}

inline String formatDateTime(const Time& t) noexcept {
    return fmt("%04d-%02d-%02d %02d:%02d:%02d", t.Year, t.Month, t.Day, t.Hour, t.Min, t.Sec);
}

inline String formatDateTimeMsec(const Time& t) noexcept {
    return fmt("%04d-%02d-%02d %02d:%02d:%02d.%03d", t.Year, t.Month, t.Day, t.Hour, t.Min, t.Sec, t.MilliSec);
}



//inline String localTimestampText() noexcept {
//    return formatTimestamp(getLocalTime());
//}
//
//inline String localTimestampTextEx() noexcept {
//    return formatTimestampEx(getLocalTime());
//}


/*

template <class string_type>
inline string_type& format_datetime_us(const string_type& t, string_type& s)
{
    if (t.size() < 12) {
        s.clear();
        return s;
    }
    return str_format(s, "%s/%s/%s %s:%s", t.substr(4, 2), t.substr(6, 2), t.substr(0, 4), t.substr(8, 2), t.substr(10, 2));
}

template <class string_type>
inline string_type& format_date_us(const string_type& t, string_type& s)
{
    if (t.size() < 8) {
        s.clear();
        return s;
    }

    return str_format(s, "%s/%s/%s", t.substr(4, 2), t.substr(6, 2), t.substr(0, 4));
}



template <class string_type>
inline string_type format_datetime_us(const string_type& t)
{
    string_type s;
    format_datetime_us(t, s);
    return s;
}


template <class string_type>
inline string_type format_date_us(const string_type& t)
{
    string_type s;
    format_date_us(t, s);
    return s;
}
*/






template <class I>
bool parse_timestamp_basic(I begin, I end, Time& t) noexcept
{
    using Char = typename I::value_type;

    Char buffer[16];
    auto c = begin;

    Index count{};
    for (; count < 14 && c != end; ++c, ++count)
        buffer[count] = *c;

    if (count < 14)
        return false;

    if (!parse_int(buffer +  0, buffer +  4, t.Year))
        return false;

    if (!parse_int(buffer +  4, buffer +  6, t.Month))
        return false;

    if (!parse_int(buffer +  6, buffer +  8, t.Day))
        return false;

    if (!parse_int(buffer +  8, buffer + 10, t.Hour))
        return false;

    if (!parse_int(buffer + 10, buffer + 12, t.Min))
        return false;

    if (!parse_int(buffer + 12, buffer + 14, t.Sec))
        return false;

    return true;
}

template <class input_iterator_type, class time_type>
bool parse_timestamp(const input_iterator_type& begin, const input_iterator_type& end, time_type& t)
{
    return parse_timestamp_basic(begin, end, t);
}

template <class time_type>
bool parse_timestamp(const char* s, time_type& t)
{
    return parse_timestamp_basic(s, s+strlen(s), t);
}


template <class time_type>
bool parse_timestamp(char* s, time_type& t)
{
    return parse_timestamp_basic(s, s+strlen(s), t);
}

template <class input_container_type, class time_type>
bool parse_timestamp(const input_container_type& s, time_type& t)
{
    return parse_timestamp_basic(s.begin(), s.end(), t);
}








#if 0 // TODO: redo

#define TIME_FORMAT_SECONDS   0x00000001
#define TIME_FORMAT_HOUR      0x00000002
#define TIME_FORMAT_MINUTE    0x00000004

#define TIME_FORMAT_DAY       0x00000010
#define TIME_FORMAT_MONTH     0x00000020
#define TIME_FORMAT_YEAR      0x00000040
#define TIME_FORMAT_WEEK_DAY  0x00000080


#define TIME_OPTION_12HOUR    0x00000001
#define TIME_OPTION_2YEAR     0x00000002

struct tTimeFormat {
  dword Options ;
  dword Language ;  // Language
  char TimeSeparator ;  // Time

  tTimeFormat ( void )
  {
    Options = 0 ;
  }
} ;

//char *Time2Str ( char *str, tTime time, tTimeFormat format ) ;

//char *Date2Str ( char *str, tDate date, tDateFormat format ) ;

char *SystemTime2Str ( char *str, tSystemTime *Time, tTimeFormat *Format ) ;



#define DELAY_MICRO   3
#define DELAY_MILLI   4
#define DELAY_SEC     5

// resolution values
// microseconds
#define TIMER_RESOLUTION_HIGHEST  0  // max interval 54.919168 ms
                                     // use Intel 8253/8254
#define TIMER_RESOLUTION_HIGH     1  // Intel 8253/8254 + BIOS counter
                                     // (interrupt counter generated by
                                     // channel 0 of 8253/8254)
                                     // max interval - 3599182594048 ns
                                     // or 3599.182594048 sec or
                                     // ~59 min
#define TIMER_RESOLUTION_MIDDLE   2  // use system counter only (BIOS)
                                     // interval - system dependent
#define TIMER_RESOLUTION_LOW      3  // use system counter + system clock
                                     // interval - unlimited
#define TIMER_RESOLUTION_LOWEST   4  // use system clock
                                     // interval - unlimited
#define TIMER_RESOLUTION_EXTRA    5  // use:  channel 0 of 8253/8253
                                     //       system counter (BIOS ticks)
                                     //       system clock




// waitable timer class - should be the same as timer class - needed
class tWaitableTimer : public t_err_except {
  public :
    tWaitableTimer ( void )
    {
      #if APEAL_OS_WINDOWS
      TimerHandle = NULL ;
      #endif
    }

    ~tWaitableTimer()
    {
      Destroy() ;
    }

    enum {
      #if APEAL_OS_WINDOWS
      wait_signaled = WAIT_OBJECT_0,
      wait_timeout = WAIT_TIMEOUT,
      wait_error = WAIT_FAILED,
      wait_infinite = INFINITE
      #else
      wait_signaled = 1,
      wait_timeout = 2,
      wait_error,
      wait_infinite = 0x7FFFFFFF
      #endif
    } ;

    bool Create ( void ) ;
    void Destroy ( void ) ;

    bool Set ( int Period ) ;

    int Wait ( int MilliSec = wait_infinite ) ;

  private :
    #if APEAL_OS_WINDOWS
    HANDLE TimerHandle ;
    #endif

} ;


// timer class
class tTimer {
  public :  // accessible information
    qword StartCounter ;
    qword Count ;  // tick counter - system dependent
    qword Frequency ;  // ticks per second - system dependent
    dword NanoSec ;  // nanoseconds - 10^-9
    dword MicroSec ;  // mikroseconds - 10^-6
    dword MilliSec ;  // milliseconds - 10^3
    dword Sec ;  // seconds - up to ~136 years
    dword Resolution ;  // available timer resolution in microseconds

  public :
    tTimer ( void ) ;
    ~tTimer() ;

    // timer open/close
    bool Open ( void ) ;
    void Close ( void ) ;

    // timer setup
    bool SetResolution ( dword Resolution ) ;

    // delay
    bool Delay ( dword DelayTime, int Units ) ;

    // time count
    bool Start ( void ) ;
    bool Read ( void ) ;
    bool Stop ( void ) ;

    // timer events

  private :  // internal data
    bool  TimerStarted ;

    #if __OS == __OS_DOS || __OS == __OS_NONE || __OS == __OS_XAOS
    dword StartTimerTicks ;
    dword StartBiosTicks ;
    dword StartClockSec ;
    dword StartClockMin ;
    dword StartClockHour ;
    dword StartClockMonth ;
    dword StartClockYear ;

    t_timer_data timer_data ;  // 8253/8254 timer data

    #endif

    #if APEAL_OS_WINDOWS
    //HINSTANCE *hWinMM ;
    //MMRESULT (* timeBeginPeriod)(UINT) ;
    //MMRESULT (* timeEndPeriod)(UINT) ;
    //MMRESULT (* timeGetDevCaps)(LPTIMECAPS,UINT) ;
    //MMRESULT (* timeGetSystemTime)(LPMMTIME,UINT) ;
    //DWORD    (* timeGetTime)(VOID) ;
    //MMRESULT (* timeKillEvent)(UINT) ;
    //MMRESULT (* timeSetEvent)(UINT,UINT,LPTIMECALLBACK,DWORD,UINT) ;

    #endif

    //void PrepareData ( void ) ;
} ;

#endif  // comment


//void DelaySec(uint32_t Sec);
//void Delay(uint32_t MilliSec);
//void DelayMicro(uint32_t MicroSec);


// ---- Timer Ticks  ------
// inline
uint64_t getTimerTicks() noexcept;
// {
//     uint64_t ticks = 0;

// #if APEAL_OS_WINDOWS

//     ticks = ::GetTickCount64();

// #elif APEAL_OS_POSIX
//     // #if TARGET_OS_UNIX

//     double t = clock();

//     ticks = (uint64_t)((t*1000*100)/CLOCKS_PER_SEC);

// #endif

//     return ticks;
// }


#if 0 // TODO: implement
inline uint32_t GetTicksDiff(uint32_t StartTick, uint32_t EndTick, uint32_t Range = 0)
{
    if (EndTick >= StartTick)
        return EndTick - StartTick;  // needed check range

    unused(Range);

    uint32_t Diff = (0xFFFFFFFF - (StartTick-EndTick)) + 1;
    return Diff;
}
#endif


#if APEAL_OS_POSIX
// under Unix implement Windows like Sleep function
inline void sleep(int msec) noexcept
{
    struct timeval t;

    t.tv_sec = msec/1000;
    t.tv_usec = (msec%1000)*1000;  // tv_usec is in microseconds

    select(0, 0, 0, 0, &t);
}

// sleep in milliseconds
//inline void sleep(int msec) { Sleep(msec); }


#else

inline void sleep(int msec) noexcept { ::Sleep(msec); }

#endif

inline void sleep_msec(int msec) noexcept {  sleep(msec);  }
inline void sleep_sec(double sec) noexcept {  sleep_msec(int(sec * 1000));  }
inline void sleep_min(double sec) noexcept {  sleep_msec(int(sec * 60 * 1000));  }

}  // apl namespace

