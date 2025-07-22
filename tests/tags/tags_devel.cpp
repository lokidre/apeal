/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <cstdint>
#include <string>

struct StringToTag1 {

    template<class T, int N>
    constexpr explicit StringToTag1(const T (&a)[N]): value(a[0] * 30 + a[1]) {}

    const char value;
};


struct Tag1 {
    const char val;

    template<int N>
    constexpr Tag1(const char (&s)[N]): val(StringToTag1(s).value) {}
};

struct SampleData1 {
    int val;
    Tag1 tag;
};

constexpr static SampleData1 data[] = {
    {1, "1"},
    {0, "  "},
};

// Little endian
template<typename T, int N>
constexpr T calcTagValueBody(const char (&a)[N], int i) {
    return i == 0 ? a[i] : a[i] * 256 + calcTagValueBody<T, N>(a, i - 1);
}

template<typename T, int N>
constexpr T calcTagValue(const char (&a)[N]) { return calcTagValueBody<T>(a, N - 1); }



//template <typename T, int N>
//inline std::string buildTagStringBody ( const char (&a)[N], int i ) {  return i == 0 ? a[i] : a[i] * 256 + calcTagValueBody<T,N>(a,i-1) ;  }

//template <typename T>
//inline std::string buildTagString ( T v ) {  return v ? std::string(1,char(v)) + buildTagString(v/256) : ;  }

template<typename T>
inline std::string buildTagString(T v) {
    std::string s;

    s.reserve(sizeof(T));

    while (v) {
        s.append(1, char(v));
        v >>= 8;
    }
    return s;
}


//// Big endian
//template <typename T, int N>
//constexpr T calcTagValueBody ( const char (&a)[N], int i ) {  return i == N-2 ? a[i] : a[i] * 256 + calcTagValueBody<T,N>(a,i+1) ;  }

//template <typename T, int N>
//constexpr T calcTagValue ( const char (&a)[N] ) {  return calcTagValueBody<T> ( a, 0 ) ;  }


struct Tag2 {
    using value_type = uint16_t;

    const value_type value;


    /*
        template <int I = 0, int N>
        constexpr uint16_t calcValue ( value_type v, const char (&a)[N] ) {
            return v ;
        }

        template <int I,int N>
        constexpr uint16_t calcValue ( value_type v, const char (&a)[N] ) {
            //if ( I == N )
            //	return v ;
            return v * 256 + calcValue<I-1,N>(v,i+1) ;
        }
        */


    template<int N>
    //constexpr PentixTag2(const char (&a)[N]): value(a[0]*30 + a[1]) {  }
    constexpr Tag2(const char (&a)[N]): value(calcTagValue<value_type>(a)) {
    }


    //operator std::string () {  return std::to_string(value) ; } ;
    std::string string() { return buildTagString(value); }

};

struct SampleData2 {
    int val;
    Tag2 tag;
};

constexpr static SampleData2 data2[] = {
    {1, "1"},
    {20, "20"},
};


void body() {
    //constexpr auto a = "aaa" ;

    auto a = data2[1];

    uint16_t b = '20';

    std::string s = a.tag.string();



    constexpr auto tag = StringToTag1("a").value;

    int aaaa = 0;
}
