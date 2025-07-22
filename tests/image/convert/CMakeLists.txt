cmake_minimum_required(VERSION 3.20)

project(apeal_image_convert)

include(../../../scripts/tests.cmake)

enable_libtiff()
enable_libturbojpeg()
enable_openjpeg()

addTest(image_convert)
