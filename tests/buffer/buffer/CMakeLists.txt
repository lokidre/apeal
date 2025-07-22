cmake_minimum_required(VERSION 3.22)

project(apeal_buffer)

include(../../../scripts/tests.cmake)


addTest(buffer_basic)
addTest(buffer_copy_move)
addTest(buffer_noncopyable)
