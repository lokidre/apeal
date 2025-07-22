cmake_minimum_required(VERSION 3.17)

project(apeal-poolbuffer)

include(../../../scripts/tests.cmake)

addTest(poolbuffer_basic)
addTest(poolbuffer_iterator)
