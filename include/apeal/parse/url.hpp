/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>

#include "../error.hpp"
#include "../string.hpp"
#include "../policy.hpp"
#include "bool.hpp"
#include "int.hpp"

namespace apl {


struct Url {
    using Ch = typename String::value_type;
    using Char = typename String::value_type;
    using Params = std::unordered_multimap<String, String>;

    String scheme;

    bool isFile() const noexcept { return scheme.empty() || scheme == "file"; }
    bool isLocal() const noexcept { return isFile(); }
    bool isHttp() const noexcept { return scheme == "http"; }
    bool isHttps() const noexcept { return scheme == "https"; }
    bool isFtp() const noexcept { return scheme == "ftp"; }
    bool isFtps() const noexcept { return scheme == "ftps"; }
    bool isS3() const noexcept { return scheme == "s3"; }


    // authority part
    String userinfo;
    String host_name, host_address;

    String host;  // additional
    int host_address_ip[6]{};  // additional

    unsigned port{};
    auto ports() noexcept {  return std::to_string(port); }
    String reg_name;

    // absolute/relative uri
    String net_path, abs_path, rel_path;
    String opaque_part;

    // query
    String query;
    Params params;

    String fragment;

    void clear() noexcept {
        *this = {};
    }

    // convenience function - get url parameters
    String get(StrView name) const noexcept
    {
        auto p = params.find(String{name});  // TODO: fix when STL implemented
        if (p == params.end())
            return {};
        return p->second;
    }

    // get integer parameter, returns 0 if missing or invalid
    int getInt(StrView name, int def = 0) const noexcept {
        return parseInt(get(name), def);
    }

    // boolean value - return false if missing
    bool getBool(StrView name, bool def = false) const noexcept {
        return parseBool(get(name), def);
    }

    String getPath() const noexcept {
        if (!abs_path.empty())
            return abs_path;
        if (!net_path.empty())
            return net_path;
        if (!rel_path.empty())
            return rel_path;
        return {};
    }
};

class UrlParser {
public:
    using Ch = typename String::value_type;
    using Char = typename String::value_type;
    using Obj = Url;

    //using string_type = String;
    //typedef typename string_type::value_type char_type;

    template <class I>
    static bool parse_it(I begin, I end, Obj &obj)
    {
        auto c = begin;
        obj.clear();


        // the first is we have to identify if 

        // try to get schema
        auto schemeEnd = std::find(c, end, ':');

        if (schemeEnd != end) {
            // scheme must start with alpha
            if (uri_alpha(*c)) {
                bool bad_scheme = false;
                auto sch = c + 1;

                // walk until schema and mark and check all letters
                for (;;) {
                    if (*sch == ':')
                        break;

                    if (!uri_scheme_char(*sch)) {
                        bad_scheme = true;
                        break;
                    }
                    ++sch;
                }

                if (!bad_scheme) {
                    obj.scheme.assign(c, schemeEnd);
                    c = schemeEnd;
                    ++c;
                }
            }
        }

        verify(c != end, "Invalid uri: %s", StrView{&*begin, (std::size_t)std::distance(begin, end)});

        // analyze if we have schema
        if (!obj.scheme.empty()) {
            // next should be ( hier_part | opaque_part )
            // hier_part = ( net_path | abs_path ) [ "?" query ]

            if (*c == '/' && c+1!=end && *(c+1)=='/') { // hier part
                process_net_path(c, end, obj);
            } else if (uri_abs_path(*c)) {  // hier part
                process_abs_path(c, end, obj.abs_path);
            } else if (uri_uric_no_slash(*c)) {  // check for opaque part
                process_opaque_part(c, end, obj);
            } else {  // if we are here, then schema was bad guess
                obj.scheme.clear();
                // so it's gonna be relative uri
            }
        }

        // check again for schema name - what if first guess was wrong
        if (obj.scheme.empty()) {
            // relativeURI   = ( net_path | abs_path | rel_path ) [ "?" query ]
            if (*c == '/' && c+1 != end && *(c+1) == '/') { //uri_net_path(c) ) {
                process_net_path(c, end, obj);
            } else if (uri_abs_path(*c)) {
                process_abs_path(c, end, obj.abs_path);
            } else if (uri_rel_path_begin(*c)) {
                process_rel_path(c, end, obj);
            } else if (uri_fragment_begin(*c)) {
                process_fragment(c, end, obj);
            }
        }

        // check for the query
        if (c != end && *c == '?') {
            process_query(c, end, obj);
        }

        if (c != end && uri_fragment_begin(*c)) {
            process_fragment(c, end, obj);
        }

        // skip whitespace at the end
        while (c != end && parse_is_space(*c))
            ++c;

        verify(c == end, "URI doesn't end correctly");  // needed - temporary

        return true;
    }


    static bool parse(StrView str, Url &obj) {
        return parse_it(str.begin(), str.end(), obj);
    }

    static Url parse(StrView s) {
        Url uri;
        parse(s, uri);
        return uri;
    }


    static String build_host(const Url &obj)
    {
        String text;

        if (!obj.scheme.empty()) {
            text += obj.scheme;
            text += "://";
        }

        if (!obj.userinfo.empty()) {
            text += obj.userinfo;
            text += '@';
        }

        if (!obj.host.empty())
            text += obj.host;

        if (obj.port) {
            text += ':';
            text += std::to_string(obj.port);
        }

        return text;
    }

    static String build(const Url &obj) noexcept
    {
        auto text = build_host(obj);
        //build_host(obj, text);

        if (!obj.abs_path.empty()) {
            if (obj.abs_path[0] != '/')
                text += '/';
            text += obj.abs_path;
        }

        if (!obj.rel_path.empty())
            text += obj.rel_path;

        if (!obj.query.empty())
            text += '?', text += obj.query;

        if (!obj.fragment.empty())
            text += '#', text += obj.fragment;

        return text ;
    }


    template <class I>
    static void escape_string_it(I begin, I end, String &str) noexcept
    {
        str.clear();
        for (auto c = begin; c != end; c++) {
            //if ( uri_alphanum(*c) )
            if (uri_unreserved(*c)) {
                str.append(1, *c);
            } else if (*c == ' ') {  // TODO: probably move somewhere
                str.append(1, '+');
            } else {
                str.append(1, '%');
                str.append(1, dig2c(((*c)>>4)&0xF));
                str.append(1, dig2c((*c)&0xF));
            }
        }
        //return str;
    }

    static void escape_string(StrView str, String &e) noexcept {
        return escape_string_it(str.begin(), str.end(), e); 
    }

    static String escape_string(StrView str) noexcept {
        String esc;
        escape_string_it(str.begin(), str.end(), esc);
        return esc;
    }

    //template <class string_type>
    //static string_type &escape_string(const char *s, string_type &e) { return escape_string(s, s+strlen(s), e); }

    //template <class string_type>
    //static string_type &escape_string(char *s, string_type &e) { return escape_string(s, s+strlen(s), e); }


    // more convenient but with extra copying
    //static String escape_string(StringA s)
    //{
    //    String r;
    //    escape_string(s, r);
    //    return r;
    //}
    static String escape(StrView s) { return escape_string(s); }



    static char dig2c(char digit)
    {
        return (((digit&0xF) > 9) ? (digit&0xf) + 'A' - 10 : (digit&0xf) + '0');
    }

    static char c2dig(char c)
    {
        return (c>='0'&&c<='9') ? c-'0' : (c>='A'&&c<='F') ? c-'A'+10 : (c>='a'&&c<='f') ? c-'a'+10 : -1;
    }


    template <class I, class S>
    static void unescape_string_it(I begin, I end, S &s)
    {
        s.clear();
        for (auto c = begin; c != end; ++c) {
            if (*c == '%') {
                Ch hi{'2'}, lo{'0'};
                ++c;
                if (c != end)
                    hi = *c, ++c;
                if (c != end)
                    lo = *c;

                int hi_int = c2dig(hi);
                int lo_int = c2dig(lo);
                if (hi_int != -1 && lo_int != -1) {
                    s.append(1, (Ch)((hi_int<<4) | lo_int));
                }
            } else if (*c == '+') {
                s += ' ';
            } else {
                s += *c;
            }
        }
        //return s;
    }

    static void unescape_string(StrView str, String &e) { return unescape_string_it(str.begin(), str.end(), e); }

    // more convenient but with extra copying
    static String unescape_string(StrView s)
    {
        String r;
        unescape_string(s, r);
        return r;
    }

    static String encode(StrView s) { return escape_string(s); }
    static String decode(StrView s) { return unescape_string(s); }


private:
    //
    // general URI syntax
    //
    template <class ...C>
    static constexpr bool is_in_list(Ch c, C... sel) noexcept {
        return ((c == sel) || ...);
    }

    //static constexpr bool uri_reserved(Char c) { return c==';'||c=='/'||c=='?'||c==':'||c=='@'||c=='&'||c=='='||c=='+'||c=='$'||c==','; }
    static constexpr bool uri_reserved(Char c) { return is_in_list(c, ';', '/', '?', ':', '@', '&', '=', '+', '$', ','); }
    static constexpr bool uri_mark(Char c) { return c=='-'||c=='_'||c=='.'||c=='!'||c=='~'||c=='*'||c=='\''||c=='('||c==')'; }
    static constexpr bool uri_digit(Char c) { return c>='0'&&c<='9'; }
    static constexpr bool uri_lowalpha(Char c) { return c >= 'a' && c <= 'z'; }
    static constexpr bool uri_upalpha(Char c) { return c >= 'A' && c <= 'Z'; }
    static constexpr bool uri_alpha(Char c) { return uri_lowalpha(c) || uri_upalpha(c); }
    static constexpr bool uri_alphanum(Char c) { return uri_alpha(c) || uri_digit(c); }
    static constexpr bool uri_unreserved(Char c) { return uri_alphanum(c) || uri_mark(c); }
    static constexpr bool uri_escaped(Char c) { return c == '%'; }
    static constexpr bool uri_uric(Char c) { return uri_reserved(c) || uri_unreserved(c) || uri_escaped(c); }
    static constexpr bool uri_uric_no_slash(Char c) { return bool(uri_unreserved(c)||uri_escaped(c)||c==';'||c=='?'||c==':'||c=='@'||c=='&'||c=='='||c=='+'||c=='$'||c==','); }
    static constexpr bool uri_pchar(Char c) noexcept { 
        // two additions
        return c==':'||c=='@'||c=='&'||c=='='||c=='+'||c=='$'||c==','||uri_unreserved(c)||uri_escaped(c) || c==';'||c=='|'; 
    }  
    static constexpr bool uri_scheme_char(Ch c) noexcept { return uri_alpha(c) || uri_digit(c) || c=='+' || c=='-' || c=='.'; }
    static constexpr bool uri_userinfo_char(Ch c) noexcept { return uri_unreserved(c)||uri_escaped(c)||c==';'||c==':'||c=='&'||c=='='||c=='+'||c=='$'||c==','; }
    static constexpr bool uri_regname(Ch c) { return bool(uri_unreserved(c)||uri_escaped(c)||c=='$'||c==','||c==';'||c==':'||c=='@'||c=='&'||c=='='||c=='+'); }

    //template <class char_type> inline bool uri_net_path       ( Char c ) {  return bool(c=='/'&&(c+1)=='/') ;  }
    static constexpr bool uri_abs_path(Char c) { return c == '/'; }
    static constexpr bool uri_rel_segment(Char c) { return bool(uri_unreserved(c)||uri_escaped(c)||c==';'||c=='@'||c=='&'||c=='='||c=='+'||c=='$'||c==','); }
    static constexpr bool uri_rel_path_begin(Char c) { return uri_rel_segment(c); }
    static constexpr bool uri_query(Char c) { return bool(c=='?'); }

    static constexpr bool uri_fragment_begin(Char c) { return c == '#'; }

    static constexpr bool uri_opaque_part_char(Char c) { return uri_uric_no_slash(c); }

private:
    template <class I>
    static bool process_net_path(I &c, I end, Obj &obj)
    {
        verify(c != end, "URI parser:premature end");
        ++c;

        verify(c != end, "URI parser:premature end");
        ++c;

        process_authority(c, end, obj);
        if (c != end && uri_abs_path(*c))
            process_abs_path(c, end, obj.abs_path);
        return true;
    }

    template <class I>
    static bool process_authority(I &c, I end, Obj &obj)
    {
        // authority     = server | reg_name
        // server        = [ [ userinfo "@" ] hostport ]
        // userinfo      = *( unreserved | escaped | ";" | ":" | "&" | "=" | "+" | "$" | "," )
        // hostport      = host [ ":" port ]
        // host          = hostname | IPv4address
        // reg_name      = 1*( unreserved | escaped | "$" | "," | ";" | ":" | "@" | "&" | "=" | "+" )
        // IPv4address   = 1*digit "." 1*digit "." 1*digit "." 1*digit
        // port          = *digit

        bool isServer{false};
        bool isUserInfo{false};

        // check if there is user info given
        auto atIt = std::find(c, end, '@');

        if (atIt != end) {
            isUserInfo = true;
            // is_userinfo
            while (*c != '@') {
                if (!uri_userinfo_char(*c)) {
                    isUserInfo = false;
                    break;
                }
                obj.userinfo += process_escaped(c, end);
            }
            if (c!=end)
                ++c;
        }

        if (isUserInfo)
            isServer = true;

        // determine host port
        if (uri_digit(*c)) {
            isServer = process_ipv4address(c, end, obj.host_address);
            obj.host = obj.host_address;
        } else {
            isServer = process_hostname(c, end, obj.host_name);
            obj.host = obj.host_name;
        }

        // check if port is given
        if (isServer) {
            if (c != end && *c == ':') {
                ++c;
                String ports;
                while (c!=end && uri_digit(*c))
                    ports += *c++;
                obj.port = parseUInt(ports); //(unsigned int)std::stoul(sport);
            }
        }

        if (!isServer) {  // reg_name
            if (c != end && !uri_regname(*c))
                throw std::runtime_error("URI parser:premature end");

            while (c != end && uri_regname(*c))
                obj.reg_name += process_escaped(c, end);
        }

        return true;
    }

    template <class I>
    static void process_abs_path(I &c, I end, String& abs_path)
    {
        abs_path += process_escaped(c, end);
        process_path_segments(c, end, abs_path);
    }

    template <class I>
    static void process_rel_path(I &c, I end, Obj &obj)
    {
        if (c == end || !uri_rel_segment(*c))
            throw std::runtime_error("URI parser:premature end");

        obj.rel_path.clear();

        while (c != end && uri_rel_segment(*c))
            obj.rel_path += process_escaped(c, end);

        // process absolute path
        if (c != end && uri_abs_path(*c)) {
            process_abs_path(c, end, obj.rel_path);
        }
    }


    template <class I>
    static void process_opaque_part(I &c, I end, Obj &obj)
    {
        if (c == end || !uri_uric_no_slash(*c))
            throw std::runtime_error("URI parser:premature end");

        obj.opaque_part += process_escaped(c, end);

        while (c != end && uri_uric(*c))
            obj.opaque_part += process_escaped(c, end);
    }

    template <class I>
    static void process_fragment(I &c, I end, Obj &obj)
    {
        if (c != end)
            ++c;

        while (c != end && uri_uric(*c))
            obj.fragment += process_escaped(c, end);
    }

    template <class I>
    static bool process_path_segments(I &c, I end, String &segments)
    {
        while (c != end && (uri_pchar(*c) || *c == '/'))  //  needed experiment - have ; in path || *c == ';' )
            segments += process_escaped(c, end);
        return true;
    }

    template <class I>
    static bool process_ipv4address(I &c, I, String &address)
    {
        //std::string ip ;
        //size_t total_count = 0 ;
        //const char *total_begin = txt ;
        address.clear();

        // has to be digit or dot (first has to be digit)
        for (int ip_dig = 0; ip_dig < 4; ip_dig++) {
            //const char *begin = txt ;
            //count = 0 ;
            //ip.clear() ;

            if (!uri_digit(*c))
                return false;
            while (uri_digit(*c))
                address.append(1, *c), ++c; //total_count++, txt++ ;
            if (ip_dig < 3 && *c != '.')
                return false;
            if (*c == '.')
                address.append(1, *c), ++c; //total_count++, txt++ ;
        }

        //address.assign(total_begin,total_count) ;

        return true;
    }

    template <class I>
    static bool process_hostname(I &c, I end, String &hostname)
    {
        // read the host name
        if (!uri_alphanum(*c))
            return false;

        hostname.clear();

        while (c != end && (uri_alphanum(*c) || *c=='-' || *c=='.'))
            hostname.append(1, *c), ++c;

        return true;
    }

    template <class I>
    static Char process_escaped(I &c, I end) noexcept
    {
        if (c == end)
            return ' ';

        if (uri_escaped(*c)) {
            ++c;
            if (c == end)
                return ' ';

            int hi = c2dig(*c);
            ++c;

            if (c == end)
                return ' ';
            int lo = c2dig(*c);
            ++c;

            if (hi != -1 && lo != -1) {
                return (Char)((hi<<4)|lo);
            }
            return ' ';
        }

        auto r = *c;
        if (c != end)
            ++c;
        return r;
    }

    template <class I>
    static void process_query(I &c, I end, Obj &obj)
    {
        if (c != end)
            ++c;
        String name, value;
        //char ch;

        obj.query.assign(c, end);

        while (c != end) {
            name.clear(), value.clear();

            // read name
            while (c != end && *c != '=')
                name += process_escaped(c, end);

            if (c == end)
                break;

            if (*c == '=') {
                ++c;
                //process_escaped(c, end);

                while (c != end && *c != '&')
                    value += process_escaped(c, end);
            }

            if (c != end && *c == '&')
                ++c; //ch = process_escaped(c, end);

            obj.params.insert({name, value});
        }
    }

};

inline
Url parseUrl(StrView text) {  return UrlParser::parse(text); }

}  // apl namespace


