/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "error.hpp"
#include "parse/http.hpp"
#include "parse/uri.hpp"
#include "parse/int.hpp"
#include "zlib.hpp"
#include "policy.hpp"
#include "socket.hpp"
#include "buffer.hpp"
#include "debug.hpp"

namespace apl {


class ABodyStringStream {
public:
    int connect() { return 0; }
    void close() {}

    template <class Response>
    int send(Response& response, void* buffer, int size)
    {
        response.body.append((const char*)buffer, size);
        return size;
    }

    template <class Response>
    int recv(Response& request, char* buffer, int size)
    {
        return 0;
    }
};



template <class ConnectionT, class policy_T=ADefaultPolicy, class body_T = ABodyStringStream >
class Http {
public:
    typedef ConnectionT conn_type;
    using Connection = ConnectionT;
    using BodyStream = body_T;

    typedef typename typemap::find<KStringPolicy, policy_T>::X::Value::string_type string_type;

private:
    typename typemap::find<KErrorPolicy, policy_T>::X::Value error_policy_;

public:
    HttpRequest request;
    HttpResponse response;
    Connection connection;
    BodyStream bodyStream;

    bool keep_alive = false;


private:
    string_type last_host, last_port;
    Url uri_;
    UrlParser uri_parser_;
    HttpParser http_parser_;

    Buffer<uint8_t> buffer_, inflate_buffer_;
    int buffer_size_policy_ = 0;

    string_type request_text, response_text;

    string_type connected_host_;
    int connected_port_ = 0;


public:
    Http() {
        init_();
    }

    // make it noncopyable
    Http(const Http&) = delete;
    Http& operator=(const Http&) = delete;


private:
    void init_()
    {
        buffer_size_policy_ = 124*1024; //8192 ;
        request.accept_encoding = "gzip;q=1.0, deflate;q=0.8, identity;q=0.5";
        request.user_agent = "APEAL HTTP Client";
        keep_alive = false;
    }


public:
    /*
    // TODO: repeated with recv file - consolidate
    void recv_body()
    {
        string_type chunk ;
        Buffer<char> buffer ;

        // receive all the chunks (if we have to)
        if ( response.transfer_encoding == "chunked" ) {

            for(;;) {
                connection.recv_packet ( chunk, http_parser_.eor ) ;

                // parse chunk
                int chunk_size = 0 ;
                for ( auto c = chunk.begin(), c_end = chunk.end() ; c != c_end && isxdigit(*c) ; ++c ) {
                    chunk_size *= 16, chunk_size += parse_int_dig_hex(*c) ;
                }

                if ( chunk_size == 0 )
                    break ;

                const int buffer_size = 1024 ;
                buffer.resize(buffer_size) ;

                int count, remaining = chunk_size ;
                while(remaining) {
                    if ( (count=conn_.recv(buffer.get(), std::min(buffer_size,remaining))) <= 0 )
                        break ;
                    response.body.append(buffer.get(),count) ;
                    remaining -= count ;
                }

                if ( conn_.ready() == 0 )
                    break ;

                connection.recv_packet ( chunk, http_parser_.eor ) ;
            }

        } else {  // transmission is not chunked
            if ( !response.content_length.empty() ) {
                int body_size = 0, total_transferred = 0 ;
                parse_int(response.content_length,body_size) ;

                if ( body_size != 0 ) {
                    const int buffer_size = 1024 ;
                    buffer.resize(buffer_size) ;

                    bool inflate_content = false ;

                    if ( response.content_encoding == "inflate" ) {
                        inflate_content = true ;
                    }

                    int count ;
                    for(;;) {
                        if ( (count=conn_.recv_partial(buffer.get(), std::min(buffer_size,body_size))) <= 0 )
                            break ;
                        response.body.append(buffer.get(),count) ;
                        total_transferred += count ;
                    }

                    if ( total_transferred != body_size ) {
                        throw std::runtime_error("not everything received") ;
                    }
                }
            }
        }
    }
    */



public:

    bool get(StrView url)
    {
        request.method = "GET";
        request.uri = url;
        return query();
    }

    bool post(StrView url, StrView body)
    {
        request.method = "POST";
        request.uri = url;
        request.body = body;
        return query();
    }

    bool query()
    {
        string_type last_host, last_port;
        //bool conn_created = false, ;
        bool conn_connected = false;

        int maxTries = 3;

        // needed move somewhere
        response.body.clear();


        uri_parser_.parse(request.uri, uri_);


        request.host = uri_.host;

        if (!uri_.port) {
            if (uri_.scheme == "https")
                uri_.port = 443;
            else
                uri_.port = 80;
        }

        if (request.connection.empty())
            request.connection = "close";

        if (!request.body.empty())
            request.content_length = std::to_string(request.body.length());

        if (request.method.empty())
            request.method = "GET";

        request.uri = uri_.abs_path;
        if (!uri_.query.empty())
            request.uri += '?', request.uri += uri_.query;

        http_parser_.generate_request(request, request_text);


        for (int itry = 0; itry < maxTries; itry++) {  // browsing cycle (limit against some exploit)
            try {
                bool need_connect = true;

                if (keep_alive && uri_.host == connected_host_ && uri_.port == connected_port_)
                    need_connect = false;

                if (need_connect) {
                    if (!connection.connect(uri_.host, uri_.port))
                        continue;
                    last_host = uri_.host, last_port = uri_.port;
                    conn_connected = true;
                    connected_host_ = uri_.host, connected_port_ = uri_.port;
                }

                connection.send(request_text);
                connection.set_timeout_recv_sec(20);
                connection.recv_packet(response_text, http_parser_.end_of_header());
                http_parser_.parse_response(response_text, response);

                bool gzip = response.content_encoding == "gzip";
                bool inflate = response.content_encoding == "inflate";
                bool useZlib = gzip || inflate;

                ZLib zlib;

                if (useZlib)
                    zlib.inflateStringBegin(&response.body, gzip);
                else
                    bodyStream.connect();

                buffer_.resize(buffer_size_policy_);

                if (response.transfer_encoding == "chunked") {
                    string_type chunk;

                    for (;;) {
                        connection.recv_packet(chunk, http_parser_.crlf);

                        // parse chunk
                        int chunkSize = 0;

                        for (const auto& c : chunk) {
                            if (c == 0x0A || c == 0x0D)  // Skip CRLF
                                continue;
                            if (!((c >= '0' && c <= '9') || (c >='A' && c<='F') || (c>='a' && c <= 'f')))
                                break;
                            chunkSize *= 16, chunkSize += parse_int_dig_hex(c);
                        }

                        //debug_printf ( "chunk: %08X", chunk_size ) ;
                        if (chunkSize == 0)
                            break;


                        int remaining = chunkSize;
                        while (remaining) {
                            auto count = connection.recvSome(buffer_.get(), std::min((int)buffer_.size(), remaining));
                            if (count <= 0)
                                break;

                            if (useZlib) {
                                zlib.inflateStringFeed(buffer_.get(), count);
                            } else {
                                if (!bodyStream.send(response, buffer_.get(), count))
                                    return false;
                            }

                            remaining -= count;
                        }

                        // receive the last CRLF
                        connection.recv(buffer_.get(), 2);
                    }

                } else {
                    if (!response.content_length.empty()) {
                        int bodySize{}, totalTransferred{};
                        parse_int(response.content_length, bodySize);

                        if (bodySize != 0) {

                            while (totalTransferred < bodySize) {
                                int count = connection.recvSome(buffer_.get(), std::min((int)buffer_.size(), bodySize));

                                if (count <= 0)
                                    break;

                                if (useZlib) {
                                    zlib.inflateStringFeed(buffer_.get(), count);
                                } else {
                                    if (!bodyStream.send(response, buffer_.get(), count))
                                        return false;
                                }

                                totalTransferred += count;
                            }

                            if (totalTransferred != bodySize) {
                                return false;
                            }
                            bodyStream.close();
                        }

                    } else if (useZlib) {
                        for (;;) {
                            int count = connection.recvSome(buffer_.get(), (int)buffer_.size());

                            if (count == 0)
                                break;

                            zlib.inflateStringFeed(buffer_.get(), count);
                        }
                    }
                }

                if (useZlib)
                    zlib.inflateStringEnd();
                else
                    bodyStream.close();

                if (!keep_alive || response.connection == "close") {
                    connection.close();
                    connected_host_.clear();
                    connected_port_ = 0;
                }

                // analyze the response code
                {
                    //int status_code = 0 ;
                    //parse_int(response.status_code,status_code) ;

                    if (response.status_code >= 100 && response.status_code < 400) {
                        if (response.status_code == 301 || response.status_code == 302 || response.status_code == 303 || response.status_code == 307) {  // moved
                            request.uri = response.location;
                            continue;
                        }
                    }
                }

                break;  // if we are here, then the transmission has finished
            }
            catch (...) {
                connected_host_.clear();
                connected_port_ = 0;
                if (itry == maxTries - 1)
                    throw std::runtime_error("Cannot connect to: " + uri_.host);
            }
        }

        return true;
    }
};

}  // namespace apl
