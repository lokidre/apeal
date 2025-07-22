/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <array>
#include <vector>
#include <string>
#include <map>
#include <ctime>

#include "../ascii.hpp"
#include "../error.hpp"
#include "../policy.hpp"
#include "../str/case.hpp"
#include "uri.hpp"
#include "split.hpp"
#include "vector.hpp"

namespace apl {

typedef std::pair<String, String> http_header_pair_type;
//typedef std::multimap<std::string,std::string> http_header_list_type ;

class http_header_list_type: public std::vector<http_header_pair_type> {
public:
    void insert(const std::string& name, const std::string& value)
    {
        push_back(http_header_pair_type(name, value));
    }

    auto find(StrView name)
    {
        auto it = begin();
        auto it_end = end();
        for (; it != it_end; ++it)
            if (it->first == name)
                return it;
        return it;
    }
};

struct HttpCookie {
    using string_type = String;

    String name;
    String value;
    String comment;
    String domain;
    int max_age{-1};
    String path;
    bool secure{};
    int version{1};
    String expires_string;

    void clear()
    {
        *this = {};
    }
};

typedef std::pair<std::string, HttpCookie> http_cookie_list_pair_type;

class http_cookie_list_type: public std::map<std::string, HttpCookie> {
public:
    typedef std::string string_type;

public:
    void insert_cookie(const HttpCookie& c)
    {
        http_cookie_list_type::iterator ci;

        if ((ci = find(c.name)) != end())
            ci->second = c;
        else
            insert(http_cookie_list_pair_type(c.name, c));
    }

    void insert_cookie(const string_type& name, const string_type& value)
    {
        HttpCookie c;
        c.name = name;
        c.value = value;

        insert_cookie(c);
    }

    void remove_cookie(const std::string& n)
    {
        erase(n);
    }

    void get_cookie(const std::string& name, HttpCookie& c)
    {
        c.clear();
        http_cookie_list_type::iterator ci = find(name);
        if (ci != end())
            c = ci->second;
    }
};


class HttpCookieParser: public AError {
public:
    using string_type = String;
    using Str = String;


public:
    bool parse_response(const string_type& cookie_text, HttpCookie& cookie)
    {
        cookie.clear();

        auto semicolon_pos = cookie_text.npos;
        std::size_t curr_pos = 0;

        string_type att_name, att_value;


        // first we need cookie name
        auto non_space_pos = cookie_text.find_first_not_of(' ', curr_pos);
        auto equ_pos = cookie_text.find_first_of('=', non_space_pos);
        auto space_pos = cookie_text.find_first_of(' ', non_space_pos);

        if (equ_pos == cookie_text.npos)
            return false;

        auto val_begin = non_space_pos;
        std::size_t val_end{};

        if (space_pos != cookie_text.npos && space_pos < equ_pos)
            val_end = space_pos;
        else
            val_end = equ_pos;

        cookie.name = cookie_text.substr(val_begin, val_end-val_begin);


        // next comes cookie value
        curr_pos = equ_pos+1;
        non_space_pos = cookie_text.find_first_not_of(' ', curr_pos);

        if (non_space_pos == cookie_text.npos)
            return false;

        val_begin = non_space_pos;

        semicolon_pos = cookie_text.find_first_of(';', non_space_pos);
        space_pos = cookie_text.find_first_of(' ', non_space_pos);

        val_end = cookie_text.size();

        if (semicolon_pos != cookie_text.npos && semicolon_pos < val_end)
            val_end = semicolon_pos;

        if (space_pos != cookie_text.npos && space_pos < val_end)
            val_end = space_pos;

        cookie.value = cookie_text.substr(val_begin, val_end-val_begin);

        curr_pos = val_end;


        // read all attributes of the cookie
        for (;;) {
            // we need semicolon
            semicolon_pos = cookie_text.find_first_of(';', curr_pos);
            if (semicolon_pos == cookie_text.npos)
                break;

            ++semicolon_pos;

            // read attribute name
            if ((non_space_pos = cookie_text.find_first_not_of(' ', semicolon_pos)) == cookie_text.npos)
                break;

            val_begin = non_space_pos, val_end = cookie_text.size();

            if ((equ_pos = cookie_text.find_first_of('=', non_space_pos)) != cookie_text.npos && equ_pos < val_end)
                val_end = equ_pos;

            if ((space_pos = cookie_text.find_first_of(' ', non_space_pos)) != cookie_text.npos && space_pos < val_end)
                val_end = space_pos;

            att_name = cookie_text.substr(val_begin, val_end - val_begin);

            curr_pos = val_end;

            // read atribute value
            att_value.clear();
            if ((val_begin = equ_pos+1) < cookie_text.size()) {
                val_end = cookie_text.size();

                if ((non_space_pos = cookie_text.find_first_not_of(' ', val_begin)) != cookie_text.npos) {

                    val_begin = non_space_pos;
                    val_end = cookie_text.size();


                    semicolon_pos = cookie_text.find_first_of(';', non_space_pos);

                    //if ( semicolon_pos != cookie_text.npos && semicolon_pos < val_end )
                    val_end = semicolon_pos;

                    //if ( (space_pos=cookie_text.find_first_of(' ',non_space_pos)) != cookie_text.npos && space_pos < val_end )
                    //  val_end = space_pos ;

                }

                att_value = cookie_text.substr(val_begin, val_end-val_begin);
            }

            str_lower(att_name);

            if (att_name == "comment")
                cookie.comment = att_value;
            else if (att_name == "domain")
                cookie.domain = att_value;
            else if (att_name == "max_age")
                cookie.max_age = parseInt(att_value, -1);
            else if (att_name == "path")
                cookie.path = att_value;
            else if (att_name == "secure")
                cookie.secure = true;
            else if (att_name == "expires")
                cookie.expires_string = att_value;
            else if (att_name == "version")
                cookie.version = parseInt(att_value, 1);
        }

        return true;
    }


    void parse_request(const string_type& cookie_text, HttpCookie& cookie)
    {
        str_split_trim(cookie_text, '=', cookie.name, cookie.value);
    }

    bool generate_response(const HttpCookie& cookie, string_type& cookie_text)
    {
        cookie_text.clear();

        str_format_append(cookie_text, "%s=%s", cookie.name, cookie.value);

        if (!cookie.comment.empty())
            str_format_append(cookie_text, "; Comment=%s", cookie.comment);
        if (!cookie.domain.empty())
            str_format_append(cookie_text, "; Domain=%s", cookie.domain);
        if (!cookie.path.empty())
            str_format_append(cookie_text, "; Path=%s", cookie.path);
        if (cookie.max_age != -1)
            str_format_append(cookie_text, "; Max-Age=%d", cookie.max_age);
        if (cookie.secure)
            str_format_append(cookie_text, "; Secure");
        //if ( cookie.version != 1 )
        str_format_append(cookie_text, "; Version=%d", cookie.version);

        return true;
    }

    bool generate_request(const HttpCookie& cookie, string_type& cookie_text)
    {
        str_format(cookie_text, "%s=%s", cookie.name, cookie.value); 
        return true;
    }
};


struct HttpRequest {
    using Str = String;
    using Ch = typename Str::value_type;

    using string_type = String;
    using char_type = string_type::value_type;


    string_type method;  // POST | GET ... etc.
    string_type uri;  // http://host[:port][path][?query]
    string_type http_version{"HTTP/1.1"};  // HTTP/1.1

    // general headers
    string_type cache_control;  // no-cache
    string_type connection;
    string_type keep_alive;
    string_type date;  // Sun, 05 Dec 2004 10:15:43 GMT | Sun Dec 5 10:15:43 2004 (asctime() format)
    string_type pragma;
    string_type trailer;
    string_type transfer_encoding;
    string_type upgrade;
    string_type via;
    string_type warning;

    // request headers
    string_type accept;  // */* | text/html, text/xml | image/jpeg; q=0.4, audio/*
    string_type accept_charset;
    string_type accept_encoding;  // compress, gzip | identity; q=0.5, gzip;q=1.0
    string_type accept_language;
    string_type authorization;
    string_type expect;
    string_type from;
    string_type host;
    string_type if_match;
    string_type if_modified_since;
    string_type if_none_match;
    string_type if_range;
    string_type if_unmodified_since;
    string_type max_forwards;
    string_type proxy_authorization;
    string_type range;
    string_type referer;
    string_type te;
    string_type user_agent;

    // entity headers
    string_type allow;
    string_type content_encoding;       // identity | deflate | gzip | compress
    string_type content_language;
    string_type content_length;         // 314
    string_type content_location;
    string_type content_md5;
    string_type content_range;
    string_type content_type;
    string_type expires;
    string_type last_modified;


    // extension
    //string_type content_disposi

    // other headers
    http_header_list_type headers;
    http_cookie_list_type cookies;

    void insert_cookie(const string_type& name, const string_type& value)
    {
        HttpCookie c;
        c.name = name;
        c.value = value;
        cookies.insert_cookie(c);
    }

    void insert_header(const string_type& name, const string_type& content)
    {
        //headers.insert(http_header_pair_type(name,content)) ;
        headers.insert(name, content);
    }

    string_type get_header(const string_type& name)
    {
        auto it = headers.find(name);
        if (it == headers.end())
            return "";

        return it->second;
    }


    string_type body;
    void* body_ptr;
    size_t body_size;

    //AHttpRequest(): http_version("HTTP/1.1") {}

    void clear()
    {
        method.clear(), uri.clear(), http_version.clear();

        cache_control.clear(), connection.clear(), keep_alive.clear(), date.clear(), pragma.clear();
        trailer.clear(), transfer_encoding.clear(), upgrade.clear(), via.clear(), warning.clear();

        accept.clear(), accept_charset.clear(), accept_encoding.clear(), accept_language.clear(), authorization.clear();
        expect.clear(), from.clear(), host.clear(), if_match.clear(), if_modified_since.clear();
        if_none_match.clear(), if_range.clear(), if_unmodified_since.clear();
        max_forwards.clear(), proxy_authorization.clear(), range.clear(), referer.clear(), te.clear(), user_agent.clear();

        allow.clear(), content_encoding.clear(), content_language.clear(), content_length.clear(), content_location.clear();
        content_md5.clear(), content_range.clear(), content_type.clear(), expires.clear(), last_modified.clear();

        headers.clear();
        cookies.clear();
        body.clear(), body_ptr = 0, body_size = 0;
    }


    std::unordered_map<Str, Str> post_params_;

    // convenienve function - simulation of PHP _POST[] array
    Str _post(StrView name)
    {
        if (body.empty())
            return "";
        // parse the body if necessary
        if (post_params_.empty()) {
            auto p = parseVector<std::vector<std::string>>(body, '&');
            for (auto p_it = p.begin(), p_end = p.end(); p_it != p_end; ++p_it) {
                if (p_it->empty())
                    continue;
                auto [name, value] = str_split(*p_it, '=');
                post_params_[String{name}] = String{value};
            }
        }

        auto p = post_params_.find(String{name});
        if (p == post_params_.end())
            return {};
        return UrlParser::unescape_string(p->second);
    }
};


struct HttpResponse {
    using Str = String;
    using Ch = typename Str::value_type;

    using string_type = String;

    // Status-Line
    string_type http_version{"HTTP/1.1"};  // http version
    int status_code{};
    string_type reason_phrase;

    // general headers
    string_type cache_control;
    string_type connection;
    string_type keep_alive;
    string_type date;
    string_type pragma;
    string_type trailer;
    string_type transfer_encoding;
    string_type upgrade;
    string_type via;
    string_type warning;

    // response headers
    string_type accept_ranges;
    string_type age;
    string_type etag;
    string_type location;
    string_type proxy_authenticate;
    string_type retry_after;
    string_type server;
    string_type vary;
    string_type www_authenticate;

    // entity headers
    string_type allow;
    string_type content_encoding;
    string_type content_language;
    string_type content_length;
    string_type content_location;
    string_type content_md5;
    string_type content_range;
    string_type content_type, content_type_name, content_type_charset;
    string_type expires;
    string_type last_modified;

    // the rest of headers
    http_header_list_type headers;
    http_cookie_list_type cookies;

    void insert_header(const string_type& name, const string_type& content)
    {
        //headers.insert(http_header_pair_type(name,content)) ;
        headers.insert(name, content);
    }


    string_type body;
    void* body_data{};
    size_t body_data_size;

    void clear() noexcept {
        *this = {};
    }
/*
    AHttpResponse(): http_version("HTTP/1.1"), body_data(0) {}

    void clear()
    {
        http_version.clear(), status_code = 200, reason_phrase.clear();
        cache_control.clear(), connection.clear(), keep_alive.clear(), date.clear();
        pragma.clear(), trailer.clear(), transfer_encoding.clear(), upgrade.clear();
        via.clear(), warning.clear();
        accept_ranges.clear(), age.clear(), etag.clear(), location.clear(), proxy_authenticate.clear();
        retry_after.clear(), server.clear(), vary.clear(), www_authenticate.clear();
        allow.clear(), content_encoding.clear(), content_language.clear(), content_length.clear();
        content_location.clear(), content_md5.clear(), content_range.clear();
        content_type.clear(), content_type_name.clear(), content_type_charset.clear();
        expires.clear(), last_modified.clear();
        headers.clear();
        body.clear(), body_data = 0, body_data_size = 0;
    }
*/


    void point_body(const void* pbody, size_t size)
    {
    }

    void point_body(const char* sbody)
    {
    }

    void point_body(string_type& sbody)
    {
    }

};

class HttpParser: public AError {
public:
    using Str = String;
    using Ch = typename Str::value_type;

    using string_type = String;
    using char_type = string_type::value_type;


private:
    char_type space{' '};
    char_type cr{0x0d};
    char_type lf{0x0a};
    char_type eor[5];  // end of request

public:

    //http_cookie_list_type cookies ;

public:
    bool copy_body{true};
    size_t body_pos{};

    size_t text_pos{}, text_line{}, text_col{};

public:

    char_type crlf[3]; //, eor[5] ;

    HttpParser()
    {
        space = ' ';
        cr = 13, lf = 10;
        crlf[0] = cr, crlf[1] = lf, crlf[2] = 0;
        eor[0] = cr, eor[1] = lf, eor[2] = cr, eor[3] = lf, eor[4] = 0;

        //eor[0] = cr, eor[1] = lf, eor[2] = cr, eor[3] = lf, eor[4] = 0 ;
        copy_body = true;
        body_pos = 0;
    }

public:
    const decltype(eor)& end_of_header() { return eor; }

    //public :
      //void set_cookie_list ( http_cookie_list_type *new_cookie_list ) {  cookies = new_cookie_list ;  }


public:
    template <class iterator_type>
    bool parse_request(const iterator_type& begin, const iterator_type& end, HttpRequest& obj)
    {
        bool res = true;
        auto s = begin;//, nl_it, eol_it;
        obj.clear();

        text_pos = text_line = text_col = 0;

        try {
            // copy method
            obj.method.reserve(10);
            for (; s!=end && *s!=0x20; obj.method.append(1, *s), ++s, ++text_pos, ++text_col)
                ;

            // find and copy uri
            obj.uri.reserve(256);
            for (; s != end && *s == 0x20; ++s, ++text_pos, ++text_col)
                ;
            for (; s != end && *s != 0x20; obj.uri.append(1, *s), ++s, ++text_pos, ++text_col)
                ;

            // version time
            obj.http_version.clear();
            for (; s!=end && *s==0x20; ++s, ++text_pos, ++text_col)
                ;
            for (; s!=end && *s!=0x20 && *s!=ASCII_CR; obj.http_version.append(1, *s), ++s, ++text_pos, ++text_col)
                ;


            // go until end of request-line
            for (; s!=end && *s!=ASCII_CR; ++s, ++text_pos, ++text_col)
                ;
            if (s==end)
                throw this;
            ++s, ++text_pos, ++text_col;
            if (s==end || *s!=ASCII_LF)
                throw this;
            ++s, ++text_pos, ++text_col;
            ++text_line, text_col = 0;

            // walk through all letters
            while (s != end) {

                // go until end of line
                auto nl_it = s;
                auto eol_it = s;
                for (;;) {
                    if (nl_it==end)
                        throw this;
                    if (*nl_it==ASCII_CR) {
                        ++nl_it;
                        if (nl_it==end)
                            throw this;
                        if (*nl_it!=ASCII_LF)
                            throw this;
                        ++nl_it;
                        break;
                    }
                    ++eol_it;
                    ++nl_it;
                }

                // break on empty line
                if (eol_it == s) {
                    s = nl_it;
                    break;
                }


                // parse the HTTPS header
                // firstly, skip all spaces
                for (; s!=end && *s==0x20; ++s, ++text_pos, ++text_col)
                    ;
                auto headerNameBegin = s;

                // go until ':' sign or space (after space
                for (; s!=end && *s!=0x20 && *s!=':'; ++s, ++text_pos, ++text_col)
                    ;
                auto header_name_end = s;

                // in case of space - go until ':'
                for (; s!=end && *s!=':'; ++s, ++text_pos, ++text_col)
                    ;

                // check if we ever ended up with end
                if (s==end)
                    throw std::runtime_error("invalid data");

                ++s, ++text_pos, ++text_col;

                // skip spaces
                for (; s!=end && *s!=ASCII_CR && *s==0x20; ++s, ++text_pos, ++text_col)
                    ;
                auto header_value_begin = s;

                // now retrieve the header value
                auto last_nonspace_it = s;
                for (; s != end && *s != ASCII_CR; ++s, ++text_pos, ++text_col)
                    if (*s != 0x20)
                        last_nonspace_it = s;
                auto header_value_end = last_nonspace_it;
                ++header_value_end;  // include last non-space

                if (s == end)
                    throw std::runtime_error("invalid data");

                // now analyze the headers
                if (scmpi(headerNameBegin, header_name_end, "Content-Length"))
                    obj.content_length.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Content-Encoding"))
                    obj.content_encoding.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Host"))
                    obj.host.assign(header_value_begin, header_value_end);


                // general headers
                if (scmpi(headerNameBegin, header_name_end, "Cache-Control"))
                    obj.cache_control.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Connection"))
                    obj.connection.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Keep-Alive"))
                    obj.keep_alive.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Date"))
                    obj.date.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Pragma"))
                    obj.pragma.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Trailer"))
                    obj.trailer.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Transfer-Encoding"))
                    obj.transfer_encoding.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Upgrade"))
                    obj.upgrade.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Via"))
                    obj.via.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Warning"))
                    obj.warning.assign(header_value_begin, header_value_end);

                // request headers
                else if (scmpi(headerNameBegin, header_name_end, "Accept"))
                    obj.accept.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Accept-Charset"))
                    obj.accept_charset.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Accept-Encoding"))
                    obj.accept_encoding.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Accept-Language"))
                    obj.accept_language.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Authorization"))
                    obj.authorization.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Expect"))
                    obj.expect.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "From"))
                    obj.from.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Host"))
                    obj.host.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "If-Match"))
                    obj.if_match.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "If-Modified-Since"))
                    obj.if_modified_since.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "If-None-Match"))
                    obj.if_none_match.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "If-Range"))
                    obj.if_range.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "If-Unmodified-Since"))
                    obj.if_unmodified_since.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Max-Forwards"))
                    obj.max_forwards.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Proxy-Authorization"))
                    obj.proxy_authorization.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Range"))
                    obj.range.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Referer"))
                    obj.referer.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "TE"))
                    obj.te.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "User-Agent"))
                    obj.user_agent.assign(header_value_begin, header_value_end);

                // entity headers
                else if (scmpi(headerNameBegin, header_name_end, "Allow"))
                    obj.allow.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Content-Encoding"))
                    obj.content_encoding.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Content-Language"))
                    obj.content_language.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Content-Length"))
                    obj.content_length.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Content-Location"))
                    obj.content_location.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Content-MD5"))
                    obj.content_md5.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Content-Range"))
                    obj.content_range.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Content-Type"))
                    obj.content_type.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Expires"))
                    obj.expires.assign(header_value_begin, header_value_end);
                else if (scmpi(headerNameBegin, header_name_end, "Last-Modified"))
                    obj.last_modified.assign(header_value_begin, header_value_end);

                else if (scmpi(headerNameBegin, header_name_end, "Cookie")) {
                    HttpCookieParser cookie_parser;
                    HttpCookie cookie;
                    string_type cookie_string;
                    auto c = header_value_begin;

                    for (;;) {
                        cookie_string.clear();
                        while (c != header_value_end && *c != ',' && *c != ';')
                            cookie_string += *c++;
                        if (c != header_value_end)
                            ++c;
                        if (cookie_string.empty())
                            break;
                        cookie_parser.parse_request(cookie_string, cookie);
                        obj.cookies.insert_cookie(cookie);
                    }
                }


                // go to the next line
                s = nl_it, text_col = 0;

            }  // parsing for cycle

            // body
            if (s != end) {
                //body_it = s ;
                body_pos = text_pos;
                if (copy_body)
                    obj.body.assign(s, end);
            }
        }
        catch (...) {
            res = false;
        }

        if (!res)
            this->raise_error();

        return res;
    }


    bool parse_request(const string_type& msg, HttpRequest& obj)
    {
        return parse_request(msg.begin(), msg.end(), obj);
    }

    template <class iterator_type>
    bool parse_response(const iterator_type& begin, const iterator_type& end, HttpResponse& obj)
    {
        // Response     = Status-Line
        //                *(( general-header | response-header | entity-header ) CRLF)
        //                CRLF
        //                [ message-body ]
        // Status-Line    = HTTP-Version SP Status-Code SP Reason-Phrase CRLF

        HttpCookieParser cookie_parser;

        auto s = begin;
        string_type line;

        obj.clear();

        http_parse_get_line(s, end, line);

        auto c = line.begin(), c_end = line.end();
        while (c != c_end && http_is_space(*c))
            ++c;
        while (c != c_end && !http_is_space(*c))
            obj.http_version += *c++;

        string_type sstatus_code;
        while (c != c_end && http_is_space(*c))
            ++c;
        while (c != c_end && !http_is_space(*c))
            sstatus_code += *c++;
        if (!parse_int(sstatus_code, obj.status_code))
            obj.status_code = 0;

        while (c != c_end && http_is_space(*c))
            ++c;
        while (c != c_end)
            obj.reason_phrase += *c++;

        // parse headers
        for (;;) {
            http_parse_get_line(s, end, line);
            if (line.length() == 0)
                break;

            http_header_pair_type hdr;

            hdr.first = str_trim(line.substr(0, line.find_first_of(':')));
            // needed bring to the lower case or better to standard format
            line = line.substr(line.find_first_of(':')+1);
            hdr.second = str_trim(line);

            // general headers
            if (scmpi(hdr.first, "Cache-Control"))
                obj.cache_control = hdr.second;
            else if (scmpi(hdr.first, "Connection"))
                obj.connection = hdr.second;
            else if (scmpi(hdr.first, "Keep-Alive"))
                obj.keep_alive = hdr.second;
            else if (scmpi(hdr.first, "Date"))
                obj.date = hdr.second;
            else if (scmpi(hdr.first, "Pragma"))
                obj.pragma = hdr.second;
            else if (scmpi(hdr.first, "Trailer"))
                obj.trailer = hdr.second;
            else if (scmpi(hdr.first, "Transfer-Encoding"))
                obj.transfer_encoding = hdr.second;
            else if (scmpi(hdr.first, "Upgrade"))
                obj.upgrade = hdr.second;
            else if (scmpi(hdr.first, "Via"))
                obj.via = hdr.second;
            else if (scmpi(hdr.first, "Warning"))
                obj.warning = hdr.second;

            // response headers
            else if (scmpi(hdr.first, "Accept-Ranges"))
                obj.accept_ranges = hdr.second;
            else if (scmpi(hdr.first, "Age"))
                obj.age = hdr.second;
            else if (scmpi(hdr.first, "ETag"))
                obj.etag = hdr.second;
            else if (scmpi(hdr.first, "Location"))
                obj.location = hdr.second;
            else if (scmpi(hdr.first, "Proxy-Authenticate"))
                obj.proxy_authenticate = hdr.second;
            else if (scmpi(hdr.first, "Retry-After"))
                obj.retry_after = hdr.second;
            else if (scmpi(hdr.first, "Server"))
                obj.server = hdr.second;
            else if (scmpi(hdr.first, "Vary"))
                obj.vary = hdr.second;
            else if (scmpi(hdr.first, "WWW-Authenticate"))
                obj.www_authenticate = hdr.second;

            // entity headers
            else if (scmpi(hdr.first, "Allow"))
                obj.allow = hdr.second;
            else if (scmpi(hdr.first, "Content-Encoding"))
                obj.content_encoding = hdr.second;
            else if (scmpi(hdr.first, "Content-Language"))
                obj.content_language = hdr.second;
            else if (scmpi(hdr.first, "Content-Length"))
                obj.content_length = hdr.second;
            else if (scmpi(hdr.first, "Content-Location"))
                obj.content_location = hdr.second;
            else if (scmpi(hdr.first, "Content-MD5"))
                obj.content_md5 = hdr.second;
            else if (scmpi(hdr.first, "Content-Range"))
                obj.content_range = hdr.second;
            else if (scmpi(hdr.first, "Content-Type")) {
                obj.content_type = hdr.second;

                auto&& v = parseStdVectorTrim<StrView>(obj.content_type, ';');
                if (v.size() >= 1)
                    obj.content_type_name = v[0];
                if (v.size() >= 2)
                    obj.content_type_charset = v[1];

            } else if (scmpi(hdr.first, "Expires"))
                obj.expires = hdr.second;
            else if (scmpi(hdr.first, "Last-Modified"))
                obj.last_modified = hdr.second;
            else if (scmpi(hdr.first, "Set-Cookie")) {
                HttpCookieParser cookie_parser;
                HttpCookie cookie;
                cookie_parser.parse_response(hdr.second, cookie);
                obj.cookies.insert_cookie(cookie);
            } else {
                obj.headers.insert(hdr.first, hdr.second);
            }
        }

        // [message-body]
        if (s != end)
            obj.body.assign(s, end);

        return true;
    }



    bool parse_response(const string_type& msg, HttpResponse& obj)
    {
        return parse_response(msg.begin(), msg.end(), obj);
    }


    bool generate_request(const HttpRequest& obj, string_type& msg)
    {
        msg.clear();

        //
        // Request          = Request-Line
        //                    *(( general-header | request-header | entity-header ) CRLF)
        //                    CRLF
        //                    [ message-body ]
        //

        // Request-Line        = Method SP Request-URI SP HTTP-Version CRLF
        msg.append(obj.method);
        msg.append(1, ' ');
        msg.append(obj.uri);
        msg.append(1, ' ');
        msg.append(obj.http_version);
        msg += crlf;

        // general headers
        {
            if (obj.cache_control.length())
                msg.append("Cache-Control: "), msg.append(obj.cache_control), msg.append(crlf);
            if (obj.connection.length())
                msg.append("Connection: "), msg.append(obj.connection), msg.append(crlf);
            if (!obj.keep_alive.empty())
                msg.append("Keep-Alive: "), msg += obj.keep_alive, msg += crlf;
            if (obj.date.length())
                msg.append("Date: "), msg.append(obj.date), msg.append(crlf);
            if (obj.pragma.length())
                msg.append("Pragma: "), msg.append(obj.pragma), msg.append(crlf);
            if (obj.trailer.length())
                msg.append("Trailer: "), msg.append(obj.trailer), msg.append(crlf);
            if (obj.transfer_encoding.length())
                msg.append("Transfer-Encoding: "), msg.append(obj.transfer_encoding), msg.append(crlf);
            if (obj.upgrade.length())
                msg.append("Upgrade: "), msg.append(obj.upgrade), msg.append(crlf);
            if (obj.via.length())
                msg.append("Via: "), msg.append(obj.via), msg.append(crlf);
            if (obj.warning.length())
                msg.append("Warning: "), msg.append(obj.warning), msg.append(crlf);
        }


        // request headers
        {
            if (!obj.accept.empty())
                msg.append("Accept: "), msg.append(obj.accept), msg.append(crlf);
            if (!obj.accept_charset.empty())
                msg.append("Accept-Charset: "), msg.append(obj.accept_charset), msg.append(crlf);
            if (!obj.accept_encoding.empty())
                msg.append("Accept-Encoding: "), msg.append(obj.accept_encoding), msg.append(crlf);
            if (!obj.accept_language.empty())
                msg.append("Accept-Language: "), msg.append(obj.accept_language), msg.append(crlf);
            if (!obj.authorization.empty())
                msg.append("Authorization: "), msg.append(obj.authorization), msg.append(crlf);
            if (!obj.expect.empty())
                msg.append("Expect: "), msg.append(obj.expect), msg.append(crlf);
            if (!obj.from.empty())
                msg.append("From: "), msg.append(obj.from), msg.append(crlf);
            if (!obj.host.empty())
                msg.append("Host: "), msg.append(obj.host), msg.append(crlf);
            if (!obj.if_match.empty())
                msg.append("If-Match: "), msg.append(obj.if_match), msg.append(crlf);
            if (!obj.if_modified_since.empty())
                msg.append("If-Modified-Since: "), msg.append(obj.if_modified_since), msg.append(crlf);
            if (!obj.if_none_match.empty())
                msg.append("If-None-Match: "), msg.append(obj.if_none_match), msg.append(crlf);
            if (!obj.if_range.empty())
                msg.append("If-Range: "), msg.append(obj.if_range), msg.append(crlf);
            if (!obj.if_unmodified_since.empty())
                msg.append("If-Unmodified-Since: "), msg.append(obj.if_unmodified_since), msg.append(crlf);
            if (!obj.max_forwards.empty())
                msg.append("Max-Forwards: "), msg.append(obj.max_forwards), msg.append(crlf);
            if (!obj.proxy_authorization.empty())
                msg.append("Proxy-Authorization: "), msg.append(obj.proxy_authorization), msg.append(crlf);
            if (!obj.range.empty())
                msg.append("Range: "), msg.append(obj.range), msg.append(crlf);
            if (!obj.referer.empty())
                msg.append("Referer: "), msg.append(obj.referer), msg.append(crlf);
            if (!obj.te.empty())
                msg.append("TE: "), msg.append(obj.te), msg.append(crlf);
            if (!obj.user_agent.empty())
                msg.append("User-Agent: "), msg.append(obj.user_agent), msg.append(crlf);
        }



        // entity headers
        {
            if (!obj.allow.empty())
                msg.append("Allow: "), msg.append(obj.allow), msg.append(crlf);
            if (!obj.content_encoding.empty())
                msg.append("Content-Encoding: "), msg.append(obj.content_encoding), msg.append(crlf);
            if (!obj.content_language.empty())
                msg.append("Content-Language: "), msg.append(obj.content_language), msg.append(crlf);
            if (!obj.content_length.empty())
                msg.append("Content-Length: "), msg.append(obj.content_length), msg.append(crlf);
            if (!obj.content_location.empty())
                msg.append("Content-Location: "), msg.append(obj.content_location), msg.append(crlf);
            if (!obj.content_md5.empty())
                msg.append("Content-MD5: "), msg.append(obj.content_md5), msg.append(crlf);
            if (!obj.content_range.empty())
                msg.append("Content-Range: "), msg.append(obj.content_range), msg.append(crlf);
            if (!obj.content_type.empty())
                msg.append("Content-Type: "), msg.append(obj.content_type), msg.append(crlf);
            if (!obj.expires.empty())
                msg.append("Expires: "), msg.append(obj.expires), msg.append(crlf);
            if (!obj.last_modified.empty())
                msg.append("Last-Modified: "), msg.append(obj.last_modified), msg.append(crlf);
        }


        //if ( obj.content_length.empty() && !obj.body.empty() ) {
        //  msg.append("Content-Length: "), str_format_append(msg,"%d",int(obj.body.size())), msg.append(crlf) ;
        //}

        for (auto hdr = obj.headers.begin(), hdr_end = obj.headers.end(); hdr != hdr_end; ++hdr) {
            msg.append(hdr->first);
            msg.append(": ");
            msg.append(hdr->second);
            msg.append(crlf);
        }


        // generate cookies
        if (!obj.cookies.empty()) {
            msg.append("Cookie: ");

            for (auto cookie_begin = obj.cookies.begin(), cookie_it = cookie_begin, cookie_end = obj.cookies.end(); cookie_it != cookie_end; ++cookie_it) {
                if (cookie_it != cookie_begin) {
                    msg.append("; ");
                }

                msg.append(cookie_it->second.name);
                msg.append(1, '=');
                msg.append(cookie_it->second.value);
            }

            msg.append(crlf);
        }


        // CRLF
        msg.append(crlf);

        // [message-body]
        if (obj.body.length()) {
            msg.append(obj.body);
        }

        return true;
    }


    template <class response_type>
    bool generate_response_headers(const response_type& obj, string_type& msg)
    {
        // general headers
        {
            if (!obj.cache_control.empty())
                msg.append("Cache-Control: "), msg.append(obj.cache_control), msg.append(crlf);
            if (!obj.connection.empty())
                msg.append("Connection: "), msg.append(obj.connection), msg.append(crlf);
            if (!obj.keep_alive.empty())
                msg.append("Keep-Alive: "), msg.append(obj.keep_alive), msg.append(crlf);
            if (!obj.date.empty())
                msg.append("Date: "), msg.append(obj.date), msg.append(crlf);
            if (!obj.pragma.empty())
                msg.append("Pragma: "), msg.append(obj.pragma), msg.append(crlf);
            if (!obj.trailer.empty())
                msg.append("Trailer: "), msg.append(obj.trailer), msg.append(crlf);
            if (!obj.transfer_encoding.empty())
                msg.append("Date: "), msg.append(obj.transfer_encoding), msg.append(crlf);
            if (!obj.upgrade.empty())
                msg.append("Upgrade: "), msg.append(obj.upgrade), msg.append(crlf);
            if (!obj.via.empty())
                msg.append("Via: "), msg.append(obj.via), msg.append(crlf);
            if (!obj.warning.empty())
                msg.append("Warning: "), msg.append(obj.warning), msg.append(crlf);
        }


        // response headers
        {
            if (!obj.accept_ranges.empty())
                msg.append("Accept-Ranges: "), msg.append(obj.accept_ranges), msg.append(crlf);
            if (!obj.age.empty())
                msg.append("Age: "), msg.append(obj.age), msg.append(crlf);
            if (!obj.etag.empty())
                msg.append("ETag: "), msg.append(obj.etag), msg.append(crlf);
            if (!obj.location.empty())
                msg.append("Location: "), msg.append(obj.location), msg.append(crlf);
            if (!obj.proxy_authenticate.empty())
                msg.append("Proxy-Authenticate: "), msg.append(obj.proxy_authenticate), msg.append(crlf);
            if (!obj.retry_after.empty())
                msg.append("Retry-After: "), msg.append(obj.retry_after), msg.append(crlf);
            if (!obj.server.empty())
                msg.append("Server: "), msg.append(obj.server), msg.append(crlf);
            if (!obj.vary.empty())
                msg.append("Vary: "), msg.append(obj.vary), msg.append(crlf);
            if (!obj.www_authenticate.empty())
                msg.append("WWW-Authenticate: "), msg.append(obj.www_authenticate), msg.append(crlf);
        }

        // entity headers
        {
            if (!obj.allow.empty())
                msg.append("Allow: "), msg.append(obj.allow), msg.append(crlf);
            if (!obj.content_encoding.empty())
                msg.append("Content-Encoding: "), msg.append(obj.content_encoding), msg.append(crlf);
            if (!obj.content_language.empty())
                msg.append("Content-Language: "), msg.append(obj.content_language), msg.append(crlf);
            if (!obj.content_length.empty())
                msg.append("Content-Length: "), msg.append(obj.content_length), msg.append(crlf);
            if (!obj.content_location.empty())
                msg.append("Content-Location: "), msg.append(obj.content_location), msg.append(crlf);
            if (!obj.content_md5.empty())
                msg.append("Content-MD5: "), msg.append(obj.content_md5), msg.append(crlf);
            if (!obj.content_range.empty())
                msg.append("Content-Range: "), msg.append(obj.content_range), msg.append(crlf);
            if (!obj.content_type.empty())
                msg.append("Content-Type: "), msg.append(obj.content_type), msg.append(crlf);
            if (!obj.expires.empty())
                msg.append("Expires: "), msg.append(obj.expires), msg.append(crlf);
            if (!obj.last_modified.empty())
                msg.append("Last-Modified: "), msg.append(obj.last_modified), msg.append(crlf);
        }

        // the rest of headers
        for (auto hdr = obj.headers.begin(), hdr_end = obj.headers.end(); hdr!=hdr_end; ++hdr) {
            msg.append(hdr->first);
            msg += ':';
            msg += ' ';
            msg.append(hdr->second);
            msg.append(crlf);
        }

        // cookies
        string_type stemp;
        for (auto& cookie: obj.cookies) {
            HttpCookieParser cookie_parser;
            cookie_parser.generate_response(cookie.second, stemp);
            if (!stemp.empty())
                msg.append("Set-Cookie: "), msg.append(stemp), msg.append(crlf);
        }

        return true;
    }


    template <class object_type>
    bool generate_response(const object_type& obj, string_type& msg)
    {
        string_type s;

        //Response = Status-Line
        //          *(( general-header | response-header | entity-header ) CRLF)
        //          CRLF
        //          [ message-body ]


        msg.clear();

        // generate status line
        //Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
        str_format_append(msg, "%s %d %s%s", obj.http_version, obj.status_code, obj.reason_phrase, crlf);

        generate_response_headers(obj, msg);

        // end of header
        msg.append(crlf);

        // response body
        if (!obj.body.empty())
            msg.append(obj.body);

        return true;
    }


public:
    // date statement
    string_type& generate_current_date(string_type& date_string)
    {
        // TODO _gmtime_s
        char time_buff[256];
        time_t t;
        time(&t);
        tm gmt; //= gmtime(&t) ;
#if APEAL_OS_POSIX
        gmtime_r(&t, &gmt);
#else
        gmtime_s(&gmt, &t);
#endif
        strftime(time_buff, 256, "%a, %d %b %Y %H:%M:%S GMT", &gmt);
        date_string = time_buff;
        return date_string;
    }

private:
    template <class iterator_type>
    void http_parse_get_line(iterator_type& s, const iterator_type& end, string_type& line)
    {
        line.clear();
        while (s != end && *s != 0x0D && (s+1) != end && *(s+1) != 0x0A)
            line += *s++;

        if (s != end)
            ++s;
        if (s != end)
            ++s;
    }

    bool http_is_space(typename string_type::value_type c) const noexcept { return c == ' ' || c == '\t' || c == '\r' || c == '\n' ? true : false; }

    static bool scmpi(StrView s, StrView text) noexcept {
        if (s.size() != text.size())
            return false;

        for ( auto t = text.begin(); auto i: s) {
            if (std::tolower(*t) != std::tolower(i))
                return false;
            ++t;
        }

        return true;
    }

    template <class I>
    static bool scmpi(I i, I end, StrView text) noexcept {
        if (std::distance(i, end) != text.size())
            return false;
        for ( auto t = text.begin(); i != end; ++i, ++t)
            if (std::tolower(*t) != std::tolower(*i))
                return false;
        return true;
    }
};

}  // namespace apl
