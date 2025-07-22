/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
//#include <test_main.h>
//#include <apeal/crc/crc8.hpp>
//#include <apeal/file.hpp>
//
//
//uint8_t crc8GSM(uint8_t data) {
//    uint8_t crc = 0x00; // Initialize CRC to 0
//    for (int i = 0; i < 8; i++) { // Process each bit of the input byte
//        crc ^= (data >> (7 - i)) & 0x01; // Get the current bit from the input
//        if (crc & 0x80) { // Check if the highest bit is set
//            crc = (crc << 1) ^ 0x1D; // Shift and XOR with the polynomial
//        } else {
//            crc <<= 1; // Just shift if not
//        }
//    }
//    return crc; // Return the computed CRC
//}
//
//void body() {
//    char ch = 'a'; // Character to compute CRC for
//    uint8_t asciiValue = static_cast<uint8_t>(ch); // Get ASCII value
//    uint8_t checksum = crc8GSM(ch); // Compute CRC
//    std::cout << "Character: " << ch << " (ASCII: " << +asciiValue << ")" << std::endl;
//    std::cout << "CRC8 (GSM): " << std::dec << static_cast<int>(checksum) << std::endl; // Output in decimal
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
    const std::string data = "aaa";

    std::uint16_t crc;

    crc = CRC::Calculate(data.c_str(), data.size(), CRC::CRC_8());  // SMBUS standard

    std::cout << std::hex << crc << std::endl;

}


