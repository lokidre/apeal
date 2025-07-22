#
#

file(REAL_PATH "${CMAKE_CURRENT_LIST_DIR}/.." APEAL_DIR)
include(${APEAL_DIR}/.build-config.cmake)


set(APEAL_TESTS_DIR "${APEAL_DIR}/tests")

include_directories(${CONFIG_APEAL_INCLUDE_DIR})


enable_language(CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED on)


include(${CONFIG_APEAL_SCRIPTS_CMAKE_DIR}/project_deps.cmake)
#enable_libpng()

# Adding coloring output to GNU and Clang
add_compile_options($<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>)
add_compile_options($<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>)

add_compile_definitions($<$<CONFIG:Debug>:_DEBUG=1>)  # debug flag
add_compile_definitions(__STDC_WANT_LIB_EXT1__) # request bound checked functions

if (WIN32) # Windows is always Unicode
    add_compile_definitions(_UNICODE UNICODE)
    add_compile_options(-U_MBCS)
endif()


if (MSVC)
    add_compile_options(/EHsc)  # Catch only C++ exceptions, assume extern C never throw exceptions
    #add_compile_options(/MP)    # Enable multiprocessor compilation
    add_compile_options(/MP12)  # Limit number of processor, so the PC can be useful


    #
    # Debug configuration
    #
    add_compile_options($<$<CONFIG:Debug>:/MDd>)  # Code generation Debug DLL
    add_compile_options($<$<CONFIG:Debug>:/fp:except>)  # Enable floating point exceptions
    #add_compile_options($<$<CONFIG:Debug>:/FR>)  # Enable browser information

    #
    # Language tuning
    #
    add_compile_options(/Zc:rvalueCast)     # Enforce type conversion rules
    #add_compile_options(/Zc:preprocessor)   # C++20 __VA_OPT__ preprocessor support
    add_compile_options(/permissive-)       # Make standard conformant


    # Eliminating warning: defaultlib 'library' conflicts with use of other libs; use /NODEFAULTLIB:library
    # Multithreaded (libcmt.lib) ignore:  msvcrt.lib; libcmtd.lib; msvcrtd.lib
    # Multithreaded using DLL (msvcrt.lib)  libcmt.lib; libcmtd.lib; msvcrtd.lib
    # Debug Multithreaded (libcmtd.lib) libcmt.lib; msvcrt.lib; msvcrtd.lib
    # Debug Multithreaded using DLL (msvcrtd.lib)   libcmt.lib; msvcrt.lib; libcmtd.lib
    add_link_options($<$<CONFIG:Debug>:/NODEFAULTLIB:libcmt.lib> $<$<CONFIG:Debug>:/NODEFAULTLIB:msvcrt.lib> $<$<CONFIG:Debug>:/NODEFAULTLIB:libcmtd.lib>)

    #
    # Warnings. Enable all warnings and just disable specific ones
    #
    add_compile_options(/W4)
    add_compile_options(/wd4100) # 'unreferenced formal parameter'
    add_compile_options(/wd4206) # nonstandard extension used: translation unit is empty
    add_compile_options(/wd4505) # unreferenced local function has been removed


    #
    # Optimized configurations
    #
#    add_compile_options($<$<NOT:$<CONFIG:Debug>>:/MT>)  # Static linking for Release
    add_compile_options($<$<NOT:$<CONFIG:Debug>>:/MD>)  # Static linking for Release
endif()

if (APPLE)
    add_compile_options(-Wundef-prefix=TARGET_OS_,TARGET_OS)
endif()

option(FORCE_UNIX "Force Unix compilation")

if (FORCE_UNIX)
    add_definitions(-DTARGET_OS_POSIX=1)
endif()


# Add header files
# file(GLOB_RECURSE apeal_H ${APEAL_INCLUDE_DIR}/*.hpp)
# source_group(TREE ${APEAL_INCLUDE_DIR} FILES ${apeal_H})

# Add common test files
set(test_common_SRC_DIR ${APEAL_TESTS_DIR}/common)
#set(main_test_SRC ${test_common_SRC_DIR}/test_main.cpp)

include(${CONFIG_APEAL_SCRIPTS_CMAKE_DIR}/apeal_sources.cmake)


#file(GLOB_RECURSE test_main_SRC ${test_common_SRC_DIR}/*.cpp ${main_SRC})
file(GLOB_RECURSE test_main_SRC ${test_common_SRC_DIR}/*.cpp)

#source_group(TREE ${test_common_SRC_DIR} PREFX "apeal-test-common" FILES ${main_test_SRC})
source_group("ApealTestCommon" FILES ${test_main_SRC})
#source_group("apeal-test-common" FILES ${main_test_SRC})



macro (addTest targetName)
    include_directories(${APEAL_TESTS_DIR}/common)

    # set(apeal_SRC "")

    # if (NOT "${apeal_SRC_DEPS}" STREQUAL "")
    #     foreach (name ${apeal_SRC_DEPS})
    #         list(APPEND apeal_SRC "${CONFIG_APEAL_SRC_DIR}/apeal_${name}.cpp")
    #     endforeach()
    # endif()
    
    # if (NOT "${apeal_SRC}" STREQUAL "")
    #     source_group("apeal-src" FILES ${apeal_SRC})
    # endif()    
    
    set(targetSrc ${targetName}.cpp)

    add_executable(${targetName} ${targetSrc} ${test_main_SRC} ${apeal_H} ${apeal_SRC} ${main_SRC})


    #
    # Adding libraries
    #
    # if (UNIX)
    #     if (APPLE)
    #         target_link_libraries(${targetName} z)
    #     else()
    #         target_link_libraries(${targetName} z uuid)
    #     endif()
    # endif()

    if (MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${targetName})
        set_property(TARGET ${targetName} PROPERTY VS_GLOBAL_CodeAnalysisRuleSet ${CONFIG_VS_RULESET})
    endif()


endmacro()

