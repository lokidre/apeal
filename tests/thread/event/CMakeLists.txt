cmake_minimum_required(VERSION 3.16)

project(apeal_event)

include(../../../scripts/tests.cmake)

find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

addTest(event)
