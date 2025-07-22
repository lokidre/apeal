#


if(WIN32)
    # Windows OpenGl support
#    link_libraries(glu32.lib OpenGL32.lib)

    # Windows necessary libraries
# link_libraries(winmm.lib Imm32.lib Ws2_32.lib)
endif()


if (UNIX)
    # linux
    link_libraries(m c)

    link_libraries(dl)
    
    if (NOT APPLE)
        link_libraries(uuid)
    endif()
#    link_libraries(png z)
endif()


# GUI Type
if (WIN32)
    if ((NOT DEFINED CONFIG_TARGET_CONSOLE) OR ($<NOT:${CONFIG_TARGET_CONSOLE}>))
        set(GUI_TYPE WIN32)
    endif()
endif()


