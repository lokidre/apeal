cmake_minimum_required(VERSION 3.16)

project(apeal-console)

include(../../scripts/tests.cmake)

addTest(print)
addTest(console)
addTest(stream)
addTest(impl)
