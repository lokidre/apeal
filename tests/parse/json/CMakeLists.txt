cmake_minimum_required(VERSION 3.16)

project(test_json)

include(../../../scripts/tests.cmake)

addTest(json_build)
addTest(json_parse)
