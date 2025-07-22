cmake_minimum_required(VERSION 3.22)

project(test_geo_ops)

include(../../../scripts/tests.cmake)

addTest(point_ops)
addTest(size_ops)

