
#
# LibTiff
macro(enable_libtiff)
    find_package(TIFF REQUIRED)
    add_compile_definitions(HAVE_LIBTIFF=1)
    include_directories(${TIFF_INCLUDE_DIRS})
    link_directories(${TIFF_LIBRARY_DIRS})
    link_libraries(${TIFF_LIBRARIES})
endmacro()

#
# Supports ZLIB_ROOT
#macro(enable_zlib)
#    find_package(ZLIB REQUIRED)
#    add_compile_definitions(HAVE_ZLIB=1)
#    include_directories(${ZLIB_INCLUDE_DIRS})
#    link_directories(${ZLIB_LIBRARY_DIRS})
#    link_libraries(${ZLIB_LIBRARIES})
#endmacro()

#macro(enable_libpng)
#    enable_zlib()

#    find_package(PNG REQUIRED)
#    add_compile_definitions(HAVE_LIBPNG=1)
#    #add_definitions(${PNG_DEFINITIONS})
#    include_directories(${PNG_INCLUDE_DIRS})
#    link_directories(${PNG_LIBRARY_DIRS})
#    link_libraries(${PNG_LIBRARIES})
#endmacro()

macro(enable_openjpeg)
    set(openjpeg_cached ${OPENJPEG_DIR})
    find_package(OpenJPEG REQUIRED)
    if (NOT openjpeg_cached)
        message("-- Found OpenJPEG: ${OPENJPEG_DIR} (ver: ${OPENJPEG_MAJOR_VERSION}.${OPENJPEG_MINOR_VERSION}.${OPENJPEG_BUILD_VERSION})")
    endif()
    add_compile_definitions(HAVE_OPENJPEG=1)
    include_directories(${OPENJPEG_INCLUDE_DIRS})
    #link_directories(${OPENJPEG_LIBRARY_DIRS})
    link_libraries(${OPENJPEG_LIBRARIES})
endmacro()

# on Windows:
#   requires pkconfig to be installed and 
#   PKG_CONFIG_PATH env variable to be properly set where libturbokpeg.pc is
macro(enable_libturbojpeg)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(LibJPEG REQUIRED QUIET libjpeg)
    add_compile_definitions(HAVE_LIBJPEG=1)
    include_directories(${LibJPEG_INCLUDE_DIRS})
    link_libraries(${LibJPEG_LINK_LIBRARIES})
    message("-- Found JPEG: ${LibJPEG_LINK_LIBRARIES} (ver: ${LibJPEG_VERSION})")

    pkg_check_modules(LibTurboJPEG REQUIRED QUIET libturbojpeg)
    add_compile_definitions(HAVE_LIBTURBOJPEG=1)
    include_directories(${LibTurboJPEG_INCLUDE_DIRS})
    link_libraries(${LibTurboJPEG_LINK_LIBRARIES})
    message("-- Found Turbo JPEG: ${LibTurboJPEG_LINK_LIBRARIES} (ver: ${LibTurboJPEG_VERSION})")
endmacro()



#
# OpenCV
macro(enable_opencv)
    set(OpenCV_DIR ${CONFIG_OPENCV_DIR})
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS})
    link_directories(${OpenCV_LIBRARY_DIRS})
    link_libraries(${OpenCV_LIBS})
    if (MSVC)
        add_compile_options(/wd5054) # operator '+': deprecated between enumerations of different types
    endif()
endmacro()


#
# Boost
macro (enable_boost)
    if (${CONFIG_BOOST_ROOT})
        set(BOOST_ROOT ${CONFIG_BOOST_ROOT})
    endif()
    find_package(Boost REQUIRED)
    link_directories(${Boost_LIBRARY_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
endmacro()

macro (enable_openssl)
    find_package(OpenSSL REQUIRED)
    include_directories(${OPENSSL_INCLUDE_DIRS})
    link_libraries(${OPENSSL_LIBRARIES})
endmacro()

#
# AWS
macro (enable_aws)
    set(BUILD_SHARED_LIBS ON CACHE STRING "Link to shared libraries by default.")
    find_package(AWSSDK REQUIRED COMPONENTS s3)
    add_compile_definitions(HAVE_AWS=1)
    add_compile_definitions(_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING=1)
    include_directories(${AWSSDK_INCLUDE_DIRS})
    link_directories(${AWSSDK_LIBRARY_DIRS})
    link_libraries(${AWSSDK_LINK_LIBRARIES})
endmacro()

macro (enable_gtk4)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK4 REQUIRED gtk4)

    include_directories(${GTK4_INCLUDE_DIRS})
    link_directories(${GTK4_LIBRARY_DIRS})
    link_libraries(${GTK4_LINK_LIBRARIES})

    add_definitions(${GTK4_CFLAGS_OTHER})
endmacro()

#
# Qt. Uses Qt5_DIR
macro (enable_qt5)
    add_compile_definitions(QT_NO_KEYWORDS=1)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    link_libraries(Qt5::Widgets)
endmacro()

#
# Qt. Uses Qt6_DIR
#
macro (enable_qt6)
    add_compile_definitions(QT_NO_KEYWORDS=1)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    find_package(Qt6 COMPONENTS Widgets REQUIRED)
    link_libraries(Qt6::Widgets Qt6::Gui)
endmacro()


#
# wxWidgets might use wxWidgets_ROOT_DIR env var
#
macro (enable_wxwidgets)
    find_package(wxWidgets REQUIRED COMPONENTS net core base)
    if(wxWidgets_USE_FILE) # not defined in CONFIG mode
        include(${wxWidgets_USE_FILE})
    endif()
    include_directories(${wxWidgets_INCLUDE_DIRS})
    link_directories(${wxWidgets_LIBRARY_DIRS})
    link_libraries(${wxWidgets_LIBRARIES})
endmacro()



#
# CPU flags
#
macro (enable_avx2)
    if (MSVC)
        add_compile_options(/arch:AVX2)
    endif()
endmacro()

macro (enable_avx512)
    if (MSVC)
        add_compile_options(/arch:AVX512)
    endif()
endmacro()

