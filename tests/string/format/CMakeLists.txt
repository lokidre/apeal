cmake_minimum_required(VERSION 3.20)

project(apeal_format)

include(../../../scripts/tests.cmake)

addTest(fmt_char)
addTest(fmt_custom_derived)
#addTest(fmt_custom_type)
addTest(fmt_float)
addTest(fmt_geo)
addTest(fmt_int)
addTest(fmt_vector)
addTest(int_test)
