cmake_minimum_required(VERSION 3.16)

project(apeal-threads)

include(../../../scripts/tests.cmake)

find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

addTest(thread_func)
addTest(thread_method)
addTest(thread_move)
addTest(thread_overloads)
addTest(thread_param)
addTest(thread_pthread)
