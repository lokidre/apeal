/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include "test_main.h"
#include <iostream>
#include "CRC.h"
#include <iomanip>  // Includes ::std::hex
#include <iostream> // Includes ::std::cout
#include <cstdint>  // Includes ::std::uint32_t
#include "string"
//class CRC32 {
//public:
//    CRC32() {
//        // Initialize the CRC32 table
//        for (uint32_t i = 0; i < 256; ++i) {
//            uint32_t crc = i;
//            for (int j = 0; j < 8; ++j) {
//                if (crc & 1) {
//                    crc = (crc >> 1) ^ polynomial; // Shift and XOR
//                } else {
//                    crc >>= 1; // Just shift
//                }
//            }
//            table[i] = crc;
//        }
//    }
//
//    // Function to compute CRC32 for the given data
//    uint32_t compute(const std::vector<uint8_t>& data) {
//        uint32_t crc = 0xFFFFFFFF; // Initial value
//        for (uint8_t byte : data) {
//            uint8_t index = (crc ^ byte) & 0xFF; // Get the index from the current CRC and byte
//            crc = (crc >> 8) ^ table[index]; // Update CRC
//        }
//        return ~crc; // Final XOR
//    }
//
//private:
//    static const uint32_t polynomial = 0xEDB88320; // CRC32 polynomial
//    uint32_t table[256]; // CRC32 lookup table
//};
//
//void body() {
//    CRC32 crc32;
//
//    // Example data
//    std::vector<uint8_t> data = {'E', 'x', 'a', 'm', 'p', 'l', 'e', ' ', 'd', 'a', 't', 'a'};
//
//    // Compute the CRC32 for the data
//    uint32_t checksum = crc32.compute(data);
//
//    // Output the result
//    std::cout << "CRC32: " << std::hex << checksum << std::endl;
//
//}


void body()
{
    const std::string data = "a";

    std::uint32_t crc;

    crc = CRC::Calculate(data.c_str(), data.size(), CRC::CRC_32());

    std::cout << std::hex << crc << std::endl;

}
