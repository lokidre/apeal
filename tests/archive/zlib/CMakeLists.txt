cmake_minimum_required(VERSION 3.16)

project(apeal-test-zlib)

include(../../../scripts/tests.cmake)

#set(apeal_SRC ${APEAL_SRC_DIR}/apeal_zlib.cpp)

addTest(gzip-inflate)


# add_executable ( gzip-inflate gzip-inflate.cpp ${apeal_SRC} )
# add_executable ( gzip-inflate-feed gzip-inflate-feed.cpp ${apeal_SRC} )
# add_executable ( gzip-inflate-string gzip-inflate-string.cpp ${apeal_SRC} )

# add_executable ( gzip-deflate gzip-deflate.cpp ${apeal_SRC} )
# add_executable ( gzip-deflate-feed gzip-deflate-feed.cpp ${apeal_SRC} )


#if(WIN32)
#    target_link_libraries(gzip-inflate debug zlibd.lib optimized zlib.lib)
#   target_link_libraries  ( gzip-inflate-feed debug zlibd.lib optimized zlib.lib )
#   target_link_libraries  ( gzip-inflate-string debug zlibd.lib optimized zlib.lib )

#   target_link_libraries  ( gzip-deflate debug zlibd.lib optimized zlib.lib )
#   target_link_libraries  ( gzip-deflate-feed debug zlibd.lib optimized zlib.lib )
#endif()
