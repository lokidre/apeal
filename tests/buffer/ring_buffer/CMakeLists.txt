cmake_minimum_required(VERSION 3.20)

project(apeal-ringbuffer)

include(../../../scripts/tests.cmake)

addTest(ringbuffer_basic)
addTest(ringbuffer_copy)
addTest(ringbuffer_erase)
addTest(ringbuffer_iterator)
addTest(ringbuffer_reverse_iterator)
addTest(ringbuffer_move)
addTest(ring_buffer_torture)
