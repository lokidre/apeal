/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <apeal/crc/crc8.hpp>
#include <apeal/file.hpp>
#include <iostream>
#include <vector>


class CRC1 {
public:
    // Constructor to initialize the polynomial
    CRC1() : polynomial(0x03) {}

    // Function to compute CRC1 for the given data
    uint8_t compute(const std::vector<uint8_t>& data) {
        crc = 0x00; // Reset CRC
        for (uint8_t byte : data) {
            crc ^= byte; // XOR with current byte
            for (int i = 0; i < 8; ++i) {
                if (crc & 0x01) { // Check if the lowest bit is set
                    crc = (crc >> 1) ^ polynomial; // Shift right and XOR with polynomial
                } else {
                    crc >>= 1; // Just shift right
                }
                crc &= 0x01; // Keep CRC to 1 bit
            }
        }
        return crc;
    }

private:
    uint8_t polynomial; // The CRC polynomial
    uint8_t crc;       // Current CRC value
};

void body() {
    CRC1 crc1;

    // Example data
    std::vector<uint8_t> data = {'b', 'y', 'd'};

    // Compute the CRC1 for the data
    uint8_t checksum = crc1.compute(data);

    // Output the result
    std::cout << "CRC-1: " << static_cast<int>(checksum) << std::endl;


}
