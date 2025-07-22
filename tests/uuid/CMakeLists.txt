cmake_minimum_required(VERSION 3.16)

project(apeal-uuid)

include(../../scripts/tests.cmake)

addTest(uid64_base58)
addTest(uid64_hex)
addTest(uid64_tests)

addTest(uuid_base58)
