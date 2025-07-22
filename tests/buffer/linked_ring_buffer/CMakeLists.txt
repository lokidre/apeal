cmake_minimum_required(VERSION 3.22)

project(apeal_linked_ring_buffer)

include(../../../scripts/tests.cmake)


addTest(linked_ring_buffer_basic)
addTest(linked_ring_buffer_torture)
