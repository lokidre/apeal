cmake_minimum_required(VERSION 3.22)

project(apeal-fixedbuffer)

include(../../../scripts/tests.cmake)

addTest(fixedbuffer_basic)
addTest(fixedbuffer_copy_move)
