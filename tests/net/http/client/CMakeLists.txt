cmake_minimum_required(VERSION 3.10)

project ( http-client )

include ( ../../../scripts/common.cmake )

macro ( __add_executable _name )
	add_executable(${ARGV})

	if ( TARGET ${_name} )
		if(WIN32)
			target_link_libraries ( ${_name}
					debug zlibd.lib optimized zlib.lib 
					winmm.lib Imm32.lib Ws2_32.lib 
			)
		else()
			target_link_libraries ( ${_name} zlib )
		endif()
	endif()
endmacro()

__add_executable ( http_example http_example.cpp ${CONFIG_APEAL_DIR}/src/apeal_zlib.cpp )
__add_executable ( http_download http_download.cpp ${CONFIG_APEAL_DIR}/src/apeal_zlib.cpp )

