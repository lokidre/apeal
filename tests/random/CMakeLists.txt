cmake_minimum_required(VERSION 3.20)

project(apeal_random)

include(../../scripts/tests.cmake)

addTest(random_basic)
addTest(random_uniform)
addTest(random_threads)
addTest(random_pseudo)

