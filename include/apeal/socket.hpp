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

#if APEAL_OS_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#endif


#if APEAL_OS_POSIX
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#endif


#include "string.hpp"
#include "policy.hpp"
#include "parse/int.hpp"
#include "time.hpp"
#include "buffer.hpp"


#include "error.hpp"
#include "err/esock.hpp"


namespace apl {

class Socket {
public:
#if APEAL_OS_WINDOWS
    using SocketHandle = SOCKET;
#else
    using SocketHandle = int;
#endif

    static constexpr SocketHandle InvalidHandle = SocketHandle(-1);

    struct AddressIn {
        sockaddr_in addr{};

        AddressIn() {
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = INADDR_ANY;
        }

        void set_port(int port) {
            addr.sin_port = htons(static_cast<uint16_t>(port));
        }

        sockaddr_in* addr_in() { return &addr; }
    };

    struct Address {
        addrinfo addr;
    };

    using AddressList = std::vector<Address>;

protected:
    SocketHandle handle_{InvalidHandle};

private:
    bool timeout_disabled_{};
    int timeout_recv_{}, timeout_recv_msec_{};
    int backlog_{};
    String host_;
    int port_{};
    int socketWriteBufferSize_{}, socketReadBufferSize_{};

    Buffer<char> buffer_;
    int buffer_size_policy_;

    bool partial_{};  // read and write might return partial packets

public:
    void timeout_disable() { timeout_disabled_ = true; }
    void timeout_enable() { timeout_disabled_ = false; }

    void set_timeout_recv_sec(int timeout_sec) { timeout_recv_ = timeout_sec, timeout_recv_msec_ = 0, timeout_enable(); }
    void set_timeout_recv_min(int timeout_min) { timeout_recv_ = timeout_min*60, timeout_recv_msec_ = 0, timeout_enable(); }

    void set_partial(bool b) { partial_ = b; }


public:  // Initialization
    Socket() noexcept
    {
#if APEAL_OS_WINDOWS
        WSADATA wsadata;
        WORD vreq = MAKEWORD(2, 2);
        auto res = ::WSAStartup(vreq, &wsadata);
        verify(res == 0, "WSAStartup error");
#endif

        backlog_ = SOMAXCONN;

        timeout_recv_ = 60;
        timeout_recv_msec_ = 0;
        timeout_disabled_ = false;
        buffer_size_policy_ = 4096;

        // string_type host_ ;
        partial_ = false;
    }

    ~Socket() noexcept
    {
        close();

        //#if APEAL_OS_WINDOWS
        //		WSACleanup() ;
        //#endif
    }


public:
    auto handle() const noexcept { return handle_; }

    void set_handle(SocketHandle h)
    {
        handle_ = h;

        socketReadBufferSize_ = socketWriteBufferSize_ = 0;
        socklen_t size = sizeof(socketReadBufferSize_);

#if APEAL_OS_WINDOWS
        //getsockopt(handle_,SOL_SOCKET,SO_SNDBUF,(char *)&socket_write_buffer_size_,&size) ;
        getsockopt(handle_, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)&socketWriteBufferSize_, &size);
        getsockopt(handle_, SOL_SOCKET, SO_RCVBUF, (char*)&socketReadBufferSize_, &size);
#else
        //getsockopt(handle_,SOL_SOCKET,SO_SNDBUF,&socket_write_buffer_size_,&size) ;
        getsockopt(handle_, SOL_SOCKET, SO_SNDBUF, &socketWriteBufferSize_, &size);
        getsockopt(handle_, SOL_SOCKET, SO_RCVBUF, &socketReadBufferSize_, &size);
#endif

        if (socketWriteBufferSize_ <= 0)
            socketWriteBufferSize_ = 1024;
        if (socketReadBufferSize_ <= 0)
            socketReadBufferSize_ = 1024;
    }

    bool createTry(int af = AF_INET, int type = SOCK_STREAM, int proto = IPPROTO_TCP)
    {
        close();

#if APEAL_OS_WINDOWS
        handle_ = ::WSASocketW(af, type, proto, 0, 0, 0);
#else
        handle_ = ::socket(af, type, proto);
#endif
        if (handle_ == InvalidHandle)
            return false;

        set_handle(handle_);

        return true;
    }


    void create(int af = AF_INET, int type = SOCK_STREAM, int proto = IPPROTO_TCP)
    {
        auto res = createTry(af, type, proto);
        verifySocket(res);
    }

    void close_socket()
    {
        if (handle_ != InvalidHandle) {

#if APEAL_OS_WINDOWS
            ::closesocket(handle_);
#else
            ::close(handle_);
#endif
            handle_ = InvalidHandle;
        }
    }

    void close() {
        close_socket();
        buffer_.release();
        host_.clear();
    }

    int setOpt(int level, int optname, const void* data, int optSize) {
        auto res = setsockopt(handle(), level, optname, (char *)data, optSize);
        return res;
    }

    template <class ValT>
    int setOpt(int level, int optname, ValT val) {
        return setOpt(level, optname, &val, sizeof(val));
    }

    //
    // Information
    //
    String getHostName()
    {
        buffer_.alloc(buffer_size_policy_);

        auto res = ::gethostname(buffer_.get(), (int)buffer_.size());

        verifySocket(res == 0);

        return String{buffer_.get()};
    }

    bool get_addr_info(StrView host, AddressList& address_list, addrinfo* hints = nullptr){
        address_list.clear();

        host_ = host;

        // Newer preferred method, supported by Windows and Linux
        addrinfo* addr{};
        String shost;
        const char *hostname = nullptr;
        if (!host.empty()) {
            shost.assign(host);
            hostname = shost.c_str();
        }

        auto res = ::getaddrinfo(hostname, 0, hints, &addr);
        verifySocket(res == 0);

        for (addrinfo* ad = addr; ad; ad = ad->ai_next) {
            auto& a = address_list.emplace_back();
            a.addr = *ad;
        }

        ::freeaddrinfo(addr);

        return true;
    }

    AddressList getAddrInfo(StrView host = {}, addrinfo* hints = nullptr) {
        AddressList addresList;
        get_addr_info(host, addresList, hints);
        return addresList;
    }



    // 
    // Connection
    //


    //
    // Host
    //
    void bind(int port) {
        struct sockaddr_in addr {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(static_cast<std::uint16_t>(port));
        addr.sin_addr.s_addr = INADDR_ANY;

        auto res = ::bind(handle_, (struct sockaddr*)&addr, sizeof(addr));

        verifySocket(res == 0);
    }

    void bind(const AddressIn& addr) {
        auto res = ::bind(handle_, (struct sockaddr*)&addr.addr, sizeof(addr.addr));
        verifySocket(res == 0);
    }


    void listen()
    {
        auto res = ::listen(handle_, backlog_);
        verifySocket(res == 0);
    }


    bool accept(Socket& targetSocket, AddressIn* addr = nullptr)
    {
        AddressIn adr;

        sockaddr_in* padr = addr ? addr->addr_in() : adr.addr_in();

        socklen_t addrLen = sizeof(sockaddr_in);
        std::memset(padr, 0, addrLen);

        auto h = ::accept(handle_, (sockaddr*)padr, &addrLen);

        verifySocket(h != InvalidHandle);

        targetSocket.set_handle(h);

        return true;
    }


    //
    // Client
    //
    // bool connect(StrView host, StrView port)
    // {
    //     int port_number = 0;
    //     parse_int(port, port_number);
    //     return this->connect(host, port_number);
    // }

    bool connect(StrView host, int port)
    {
        close();

        host_ = host;

        AddressList addresses;

        addrinfo hints{};

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;


        addrinfo* addr_list;
        int res = 0;

        auto portString = std::to_string(port);

        res = ::getaddrinfo(String{host}.c_str(), portString.c_str(), &hints, &addr_list);

        verifySocket(res == 0);

        // try to connect to all addresses found
        for (addrinfo* ad = addr_list; ad; ad = ad->ai_next) {
            if (!createTry(ad->ai_family, ad->ai_socktype, ad->ai_protocol))
                continue;

            if (::connect(handle_, ad->ai_addr, (int)ad->ai_addrlen) == -1) {
                close_socket();
                continue;
            }

            break;  // connected
        }

        ::freeaddrinfo(addr_list);


        // if we are here, then we couldn't connect
        verifySocket(handle_ != InvalidHandle);

        return true;
    }

    bool connect(const Address& addr)
    {
        create(addr.addr.ai_family, addr.addr.ai_socktype, addr.addr.ai_protocol);

        auto res = ::connect(handle_, addr.addr.ai_addr, (int)addr.addr.ai_addrlen);

        verifySocket(res != -1);

        return true;
    }


    int ready()
    {
        int cnt = 0;

        if (ioctl_(FIONREAD, &cnt) == -1)
            return 0;

        return cnt;
    }


    //
    // Sending data
    //

    // lowlevel sendto function implementation
    int sendTo(const void* data, int size, int flags, const struct sockaddr* to, int toLen) {
        auto res = ::sendto(handle(), (const char*)data, size, 0, to, toLen);
        return (int)res;
    }


    int send(StrView s) { return send(s.data(), (int)s.size()); }

    int send(char c) { return send(&c, 1); }

    int send(const void* data, int size)
    {
        auto d = (const char*)data;
        int sent = 0; //, total_count=0 ;
        int itry, tries = 5;
        int count = 0;


        while (sent < size) {  // cycle while we have something to send

            for (itry = 0; itry < tries; ++itry) {
                count = (int)::send(handle_, d, std::min(size - sent, socketWriteBufferSize_), 0);

                if (count > 0)  // sending was successful
                    break;

                verifySocket(count != 0);  // 0 - disconnected

                //if ( count == 0 ) {  // disconnected
                    //handle_error_() ;
                //	return sent ;
                //}

#if APEAL_OS_WINDOWS
                if (WSAGetLastError() == WSAEINTR)
                    continue;
#else
                if (errno == EINTR)
                    continue;
#endif

                //handle_error_() ;
                return sent;
            }

            verifySocket(itry <= tries);

            sent += count;
            d += count;

            if (partial_)
                break;
        }

        return sent;
    }

    int recvSome(void* data, int size)
    {
        char* p = (char*)data;
        int count = -1;
        int readyCount{};

        if (!timeout_disabled_) {

            auto res = ioctl_(FIONREAD, &readyCount);

            verifySocket(res != -1);

            if (readyCount == 0) {
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(handle_, &rfds);
                struct timeval to;
                to.tv_sec = timeout_recv_;
                to.tv_usec = timeout_recv_msec_;

                int selectResult = ::select(FD_SETSIZE, &rfds, 0, 0, &to);

                verifySocket(selectResult > 0);
            }
        }

        count = (int)::recv(handle_, p, size, 0);

        verifySocket(count >= 0);

        //if ( count < 0 ) {
        //	handle_error_() ;
        //	return count ;
        //}

        return count;
    }


    int recv(void* data, int count)
    {
        char* p = (char*)data;
        int left_count = count;
        int read_count, total_count(0);

        while (left_count > 0) {
            read_count = recvSome(p, left_count);

            verifySocket(read_count > 0);
            //if ( read_count < 0 ) {
            //	handle_error_() ;
            //	return -1;
            //}

            //if ( read_count == 0 ) {
            //	handle_error_() ;
            //	return read_count ;
            //}

            left_count -= read_count;
            p += read_count;
            total_count += read_count;

            if (partial_)
                break;
        }

        return total_count;
    }



    bool checkEndOfPacket(const std::string& packet, const std::string& eop)
    {
        if (packet.length() < eop.length())
            return false;

        if (packet.substr(packet.length() - eop.length()) == eop)
            return true;

        return false;
    }

    int recv_string(int length, String& packet)
    {
        buffer_.alloc(socketReadBufferSize_);

        packet.clear();
        packet.reserve(length);

        int totalCount{};
        while (totalCount < length) {
            int count = recvSome(buffer_.get(), std::min(socketReadBufferSize_, length-totalCount));

            if (count <= 0)
                return count;

            packet.append(buffer_.get(), count);
            totalCount += count;
        }
        return totalCount;
    }


    template <class C, class D>
    int recv_packet(C& packet, const D& end_of_packet)
    {
        packet.clear();
        buffer_.alloc(socketReadBufferSize_);

        // timeout support
        bool packetReceived{};

        // now keep reading the data until 0
        for (;;) {
            int readyCount{};

            for (;;) {
                auto res = ioctl_(FIONREAD, &readyCount);

                verifySocket(res != -1);

                if (readyCount != 0)
                    break;

                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(handle_, &rfds);

                //struct timeval to{};
                //to.tv_sec = timeout_recv_;
                //to.tv_usec = 0;

                //res = ::select(FD_SETSIZE, &rfds, 0, 0, &to);
                res = ::select(FD_SETSIZE, &rfds, 0, 0, 0);
                verifySocket(res != 0);


                // test if there is data available
                auto count = ::recv(handle_, buffer_.get(), 1, MSG_PEEK);
                verifySocket (count > 0);
            }

            // read the data and analyze it
            auto readSize = std::min((int)buffer_.size(), readyCount);

            // peek the data
            readSize = (int)::recv(handle_, buffer_.get(), readSize, MSG_PEEK);

            verifySocket(readSize > 0);

            // walk through the data and check for the end of packet 
            // copy the end-of-packet too
            int acceptedCount = 0;
            while (acceptedCount < readSize) {
                packet.append(&buffer_[acceptedCount], 1);
                acceptedCount++;

                if (checkEndOfPacket(packet, end_of_packet)) {
                    packetReceived = true;
                    break;
                }
            }

            // read only the correct amount of data
            if (acceptedCount) {
                readSize = acceptedCount;

                auto ptr = (char*)buffer_.get();
                auto bytesleft = readSize;

                do {
                    int rc{};

                    do {
                        rc = (int)::recv(handle_, ptr, bytesleft, 0);
#if APEAL_OS_WINDOWS
                    } while (rc < 0 && WSAGetLastError() == WSAEINTR);
#else
                    } while (rc < 0 && errno == EINTR);
#endif
                    verifySocket(rc > 0);
    
                    bytesleft -= rc;
                    ptr += rc;
                } while (bytesleft > 0);
            }

            if (packetReceived)
                break;
        }

        return (int)packet.size();
    }


public:
    bool set_blocking(bool b = true)
    {
        int param = b ? 0 : 1;

        auto ret = ioctl_(FIONBIO, &param);

        return ret != -1;
    }

private:



    int ioctl_(int cmd, int* param)
    {
#if APEAL_OS_WINDOWS
        unsigned long lparam{};
        auto res = ::ioctlsocket(handle_, cmd, &lparam);
        *param = (int)lparam;
        return res;
#else
        return ::ioctl(handle_, cmd, param);
#endif
    }


    template <class Container>
    bool checkEndOfPacket(const Container& packet, const Container& eop)
    {
        if (packet.length() < eop.length())
            return false;

        if (packet.substr(packet.length() - eop.length()) == eop)
            return true;

        return false;
    }

protected:
    String buildErrorString() {
        //String s = "socket: ";
        String s = "Socket Error: ";

        s += getErrorStringSocket(getErrorCodeSocket());

        if (!host_.empty()) {
            s += ": ";
            s += host_;
        }
        if (port_)
            s += std::to_string(port_) + ": ";
        return s;
    }

    void verifySocket(bool cond) {
        if (cond)
            return;
        handleError(buildErrorString());
    }

    void verifySocket(bool cond, StrView s) {
        if (cond)
            return;
        handleError(fmt("%s: %s", s, buildErrorString()));
    }

    void handleError(StrView text) {
        throwError(text);
    }

    /*
protected :
    bool handle_error_ ( const String &error_string)
    {
        ErrorPolicy error_policy ;

        if ( host_.empty() )
            return error_policy(error_string) ;

        string_type es = "socket:" ;
        es += host_ ;
        es += ":" ;
        es += error_string ;

        return error_policy(es) ;
    }

    bool handle_error_ ( int error_code)
    {
        string_type error_string ;
        get_error_string_socket(error_code,error_string) ;
        return handle_error_(error_string) ;
    }

    bool handle_error_()
    {
        return handle_error_(get_error_code_()) ;
    }


    int get_error_code_()
    {
#if APEAL_OS_WINDOWS
        return ::WSAGetLastError() ;
#else
        return errno ;
#endif
    }
    */

};

}  // namespace apeal


