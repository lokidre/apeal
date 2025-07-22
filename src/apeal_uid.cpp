/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "../include/apeal/uid.hpp"

#include "../include/apeal/endian.hpp"

#include "../3rdparty/base58/base58.h"


#ifdef APEAL_OS_WINDOWS

#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include <intrin.h>

#endif


#include <chrono>
#include <mutex>
#include <array>

namespace apl {

/*
    Id format

    10 bits - node id
    42 bits - timestamp in milliseconds
    12 bits - counter
*/


// Half avalanche
[[maybe_unused]]
static uint32_t half_avalanche_hash32(uint32_t a)
{
    a = (a + 0x7ed55d16) + (a<<12);
    a = (a ^ 0xc761c23c) ^ (a>>19);
    a = (a + 0x165667b1) + (a<< 5);
    a = (a + 0xd3a2646c) ^ (a<< 9);
    a = (a + 0xfd7046c5) + (a<< 3);
    a = (a ^ 0xb55a4f09) ^ (a>>16);

    return a;
}


// Jenkins One-at-a-time hash
static uint32_t hashStart() {
    return 0;
}

[[maybe_unused]]
static uint32_t hashAdd(uint32_t hash, const void* data, int length) {
    for (auto ptr = (const uint8_t*)data; length > 0; ptr++, length--) {
        hash += *ptr;
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    return hash;
}

static uint32_t hashFinish(uint32_t hash) {
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}



#if APEAL_OS_WINDOWS

static uint32_t hashMacAddress(uint32_t hash, IP_ADAPTER_INFO* info)
{
    for (UINT i = 0; i < info->AddressLength; i++) {
        hash = hashAdd(hash, &info->Address[i], sizeof(info->Address[i]));
    }
    return hash;
}

static uint32_t hashMacAddresses(uint32_t hash)
{
    IP_ADAPTER_INFO ai[32];
    ULONG bufSize = sizeof(ai);

    auto dwStatus = GetAdaptersInfo(ai, &bufSize);
    if (dwStatus != ERROR_SUCCESS)
        return hash;

    for (auto adapter = ai; adapter; adapter = adapter->Next) {
        if (adapter->Type != MIB_IF_TYPE_ETHERNET)
            continue;
        hash = hashAdd(hash, adapter->Address, adapter->AddressLength);
    }

    return hash;
}

static uint32_t hashVolume(uint32_t hash) {
    DWORD serialNum = 0;
    char buffer[MAX_PATH+1];

    GetSystemDirectoryA(buffer, MAX_PATH);
    GetVolumeInformationA(buffer, NULL, 0, &serialNum, NULL, NULL, NULL, 0);
    hash = hashAdd(hash, &serialNum, sizeof(serialNum));

    return hash;
}


static uint32_t hashCPU(uint32_t hash) {
    std::array<int, 4> cpui;
    __cpuid(cpui.data(), 0);
    for (auto& d: cpui) {
        hash = hashAdd(hash, (uint16_t*)&d, sizeof(uint16_t));
    }
    return hash;
}

static uint32_t hashMachineName(uint32_t hash)
{
    char computerName[1024];
    DWORD size = 1024;
    GetComputerNameA(computerName, &size);
    int len = (int)strlen(computerName);
    return hashAdd(hash, computerName, len);
}

#else  // Not Windows


static uint32_t hashMacAddresses(uint32_t hash) { return hash; }
static uint32_t hashCPU(uint32_t hash) { return hash; }
static uint32_t hashVolume(uint32_t hash) { return hash; }
static uint32_t hashMachineName(uint32_t hash) { return hash; }


#endif


static uint32_t getNodeId()
{
    auto hash = hashStart();

    hash = hashMacAddresses(hash);
    hash = hashVolume(hash);
    hash = hashCPU(hash);
    hash = hashMachineName(hash);

    return hashFinish(hash);
}


uint64_t generateUid64() noexcept
{
    // Timestamp
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Ms = std::chrono::milliseconds;
    using Us = std::chrono::microseconds;


    // Get the timestamp before using mutex
    auto now = Clock::now();
    auto timestamp = uint64_t(std::chrono::duration_cast<Us>(now.time_since_epoch()).count());

    // Eventually remove for synchronized block
    static std::mutex mtx;
    std::scoped_lock lock(mtx);

    uint64_t uid = 0;

    static uint64_t stimestamp = 0;
    static uint32_t snode = 0;
    static uint32_t scounter = 0;

    // Node Id
    if (snode == 0) {
        snode = getNodeId();
    }


    //uid |= ((snode >> (22-10)) & (~0<<10)) ;  // Get high 10 bits
    uint64_t nodeHigh10 = snode >> 22; // Get highes 10 bits
    uid |= nodeHigh10 << 54;


    if (timestamp > stimestamp) {
        stimestamp = timestamp;
        scounter = 0;
    }
    //auto utimestamp = stimestamp << 22 ;
    //uint64_t timestamp42 = stimestamp & (~uint64_t(0)<<42) ;
    uid |= stimestamp << 12;


    // Counter

    uint32_t ucounter = scounter++;

    if (scounter == 0x3F) {
        stimestamp++;
        scounter = 0;
    }

    uid |= ucounter;


    //uint64_t uid = utimestamp | unode | ucounter ;

    return uid;
}

String formatUid64Base68(std::uint64_t uid) noexcept {
    //auto uid = generateUid64();
    auto bigEndian = to_be_64(uid);
    auto begin = (uint8_t *)&bigEndian;

	return EncodeBase58(begin, begin + sizeof(uid));
}


}  // namespace apl

