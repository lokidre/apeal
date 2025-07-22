cmake_minimum_required(VERSION 3.20)

project(apeal_str_trim)

include(../../../scripts/tests.cmake)

addTest(str_trim)
addTest(str_trim_test)
addTest(str_trim_types)
addTest(str_trim_update)
#add_executable ( str_trim str_trim.cpp )
#add_executable ( str_trim_types str_trim_types.cpp )
