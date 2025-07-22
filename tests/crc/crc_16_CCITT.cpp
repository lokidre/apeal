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
#include <vector>

class CRC16CCITT {
public:
    // Constructor to initialize the polynomial
    CRC16CCITT() : polynomial(0x1021), crc(0xFFFF) {} // Initial value is typically 0xFFFF

    // Function to compute CRC-16-CCITT for the given data
    uint16_t compute(const std::vector<uint8_t>& data) {
        crc = 0xFFFF; // Reset CRC
        for (uint8_t byte : data) {
            crc ^= (byte << 8); // XOR byte into the upper 8 bits of crc
            for (int i = 0; i < 8; ++i) {
                if (crc & 0x8000) { // Check if the highest bit is set
                    crc = (crc << 1) ^ polynomial; // Shift left and XOR with polynomial
                } else {
                    crc <<= 1; // Just shift left
                }
                crc &= 0xFFFF; // Keep CRC to 16 bits
            }
        }
        return crc;
    }

private:
    uint16_t polynomial; // The CRC polynomial
    uint16_t crc;       // Current CRC value
};

void body() {
    CRC16CCITT crc16;

    // Example data
    std::vector<uint8_t> data = {'h'};

    // Compute the CRC-16-CCITT for the data
    uint16_t checksum = crc16.compute(data);

    // Output the result
    std::cout << "CRC-16-CCITT: " << std::hex << checksum << std::endl;

}