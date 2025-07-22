/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
//#include "test_main.h"
//#include <iostream>
//#include <vector>
//
//class CRC16ANSI {
//public:
//    // Constructor to initialize the polynomial
//    CRC16ANSI() : polynomial(0xA001), crc(0xFFFF) {} // Initial value is typically 0xFFFF
//
//    // Function to compute CRC-16-ANSI for the given data
//    uint16_t compute(const std::vector<uint8_t>& data) {
//        crc = 0xFFFF; // Reset CRC
//        for (uint8_t byte : data) {
//            crc ^= byte; // XOR byte into CRC
//            for (int i = 0; i < 8; ++i) {
//                if (crc & 0x0001) { // Check if the lowest bit is set
//                    crc = (crc >> 1) ^ polynomial; // Shift right and XOR with polynomial
//                } else {
//                    crc >>= 1; // Just shift right
//                }
//                crc &= 0xFFFF; // Keep CRC to 16 bits
//            }
//        }
//        return crc;
//    }
//
//private:
//    uint16_t polynomial; // The CRC polynomial
//    uint16_t crc;       // Current CRC value
//};
//
//void body() {
//    CRC16ANSI crc16;
//
//    // Example data
//    std::vector<uint8_t> data = {'h','y'};
//
//    // Compute the CRC-16-ANSI for the data
//    uint16_t checksum = crc16.compute(data);
//
//    // Output the result
//    std::cout << "CRC-16-ANSI: " << std::hex << checksum << std::endl;
//
//}


#include "test_main.h"
#include <iostream>
#include "CRC.h"
#include <iomanip>  // Includes ::std::hex
#include <iostream> // Includes ::std::cout
#include <cstdint>  // Includes ::std::uint32_t
#include "string"


void body()
{
    const std::string data = "a";

    std::uint16_t crc;

    crc = CRC::Calculate(data.c_str(), data.size(), CRC::CRC_16_ARC());

    std::cout << std::hex << crc << std::endl;

}

