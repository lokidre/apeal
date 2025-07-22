cmake_minimum_required(VERSION 3.16)

project(apeal-cmdline)

include(../../scripts/tests.cmake)

addTest(cmdline_binding)
addTest(cmdline_info)
