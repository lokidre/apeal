#

#message( "prj: ${CMAKE_PROJECT_NAME}" )

# Set C++ Language Standard
enable_language(CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED on)


if ("${CMAKE_PROJECT_NAME}" EQUAL "Project")
    message(FATAL_ERROR, "Set project before including this file")
endif()

include_directories(${CONFIG_INCLUDE_DIRECTORIES})
link_directories(${CONFIG_LINK_DIRECTORIES})

include_directories($ENV{APEAL_INCLUDE_DIRECTORIES})
link_directories($ENV{APEAL_LINK_DIRECTORIES})


if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT EMSCRIPTEN)
    set(LINUX YES)
endif()



# Adding coloring output to GNU and Clang
add_compile_options($<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>)
add_compile_options($<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>)

add_compile_definitions($<$<CONFIG:Debug>:_DEBUG=1>)  # debug flag
add_compile_definitions(__STDC_WANT_LIB_EXT1__) # request bound checked functions


#message ( "msvc: '${MSVC}' comp id: '${CMAKE_CXX_COMPILER_ID}'" )

if (WIN32)  # Windows is always Unicode
    add_compile_definitions(_UNICODE UNICODE)
    add_compile_options(-U_MBCS)
endif()


if (NOT EMSCRIPTEN)
    find_package(Threads REQUIRED)
    link_libraries(Threads::Threads)
endif()

if (EMSCRIPTEN)
    add_compile_options(-sDISABLE_EXCEPTION_CATCHING=0 -sUSE_ZLIB=1)
endif()


if (UNIX)
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++20>)
    endif()
endif()

if (MSVC)
    add_compile_options(/EHsc)  # catch only C++ exceptions, assume extern C never throw exceptions
    add_compile_options(/MP)  # Enable multiprocessor compilation
    #add_compile_options(/MP14)  # Limit number of processor, so the PC can be useful


    #
    # Debug configuration
    #
    add_compile_options($<$<CONFIG:Debug>:/MDd>)  # Code generation Debug DLL
    add_compile_options($<$<CONFIG:Debug>:/fp:except>)  # Enable floating point exceptions
    #add_compile_options ($<$<CONFIG:Debug>:/FR>)  # Enable browser information

    #
    # Language tuning
    #
    add_compile_options(/Zc:rvalueCast)     # Enforce type conversion rules
    #add_compile_options(/Zc:preprocessor)   # C++20 __VA_OPT__ preprocessor support
    add_compile_options(/permissive-)       # Make standard conformant
    add_compile_options("/bigobj")       # Large number of entries support support
    #add_compile_options("/showIncludes")  # show include tree



    # Eliminating warning: defaultlib 'library' conflicts with use of other libs; use /NODEFAULTLIB:library
    # Multithreaded (libcmt.lib) ignore:  msvcrt.lib; libcmtd.lib; msvcrtd.lib
    # Multithreaded using DLL (msvcrt.lib)	libcmt.lib; libcmtd.lib; msvcrtd.lib
    # Debug Multithreaded (libcmtd.lib)	libcmt.lib; msvcrt.lib; msvcrtd.lib
    # Debug Multithreaded using DLL (msvcrtd.lib)	libcmt.lib; msvcrt.lib; libcmtd.lib
    #add_link_options($<$<CONFIG:Debug>:/NODEFAULTLIB:msvcrt.lib> $<$<CONFIG:Debug>:/NODEFAULTLIB:msvcrt.lib> $<$<CONFIG:Debug>:/NODEFAULTLIB:libcmtd.lib>)
    add_link_options($<$<CONFIG:Debug>:/NODEFAULTLIB:libcmt.lib> $<$<CONFIG:Debug>:/NODEFAULTLIB:msvcrt.lib> $<$<CONFIG:Debug>:/NODEFAULTLIB:libcmtd.lib>)

    #
    # Warnings. Enable all warnings and just disable specific ones
    #
    add_compile_options(/W4)
    add_compile_options(/wd4100) # 'unreferenced formal parameter'
    add_compile_options(/wd4206) # nonstandard extension used: translation unit is empty
    add_compile_options(/wd4251) # class '' needs to have dll-interface to be used by clients of class
    add_compile_options(/wd4505) # unreferenced local function has been removed
    add_compile_options(/wd4189) # local variable is initialized but not referenced


    #
    # Optimized configurations
    #
    #add_compile_options($<$<NOT:$<CONFIG:Debug>>:/MT>)  # Static linking for Release
    add_compile_options($<$<NOT:$<CONFIG:Debug>>:/MD>)  # Dynamic linking for Release
endif()

