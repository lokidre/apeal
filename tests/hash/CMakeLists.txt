cmake_minimum_required(VERSION 3.16)

project(apeal-hash)

include(../../scripts/tests.cmake)

addTest(hash_md5_file)
addTest(hash_md5_test)
addTest(hash_sha256_file)
addTest(hash_sha256_test)
