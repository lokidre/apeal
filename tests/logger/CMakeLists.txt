cmake_minimum_required(VERSION 3.16)

project(apeal-logger)

include(../../scripts/tests.cmake)

addTest(logger_basic)
addTest(logger_custom)
