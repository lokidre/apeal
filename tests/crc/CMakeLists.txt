cmake_minimum_required(VERSION 3.20)

project(apeal_crc)

include(../../scripts/tests.cmake)

addTest(crc_8)
addTest(crc_8_SMBUS)
addTest(crc_64)
addTest(crc_adler32)
addTest(crc_1)
addTest(crc_16_CCITT)
addTest(crc_16_ARC)
addTest(crc_32_ISO)
addTest(crc_64_ISO)
