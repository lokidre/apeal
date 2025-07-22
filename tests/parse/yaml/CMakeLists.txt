cmake_minimum_required(VERSION 3.20)

project(apeal_parse_yaml)

include(../../../scripts/tests.cmake)

addTest(yaml_basic)
addTest(yaml_process)
addTest(yaml_dump)
addTest(yaml_gen)
addTest(yaml_make)
