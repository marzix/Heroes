project(Heroes2)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

set( CMAKE_C_FLAGS "-std=c99")
set( CMAKE_CXX_FLAGS "-std=c++11 -static-libstdc++")

SET(LIBS
    SDL2_image
    SDL2_ttf
    SDL2_mixer
    SDL2)

if(WIN32)

    SET(LIB_DIRS
        "${PROJECT_SOURCE_DIR}/../dependency/SDL2/lib/windows_i686/"
        "${PROJECT_SOURCE_DIR}/../dependency/SDL2_TTF/lib/windows_i686/"
        "${PROJECT_SOURCE_DIR}/../dependency/SDL2_Image/lib/windows_i686/"
        "${PROJECT_SOURCE_DIR}/../dependency/SDL2_Mixer/lib/windows_i686/"
        )

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

    SET(LIB_DIRS
        "${PROJECT_SOURCE_DIR}/../dependency/SDL2/lib/Linux_x64/"
        "${PROJECT_SOURCE_DIR}/../dependency/SDL2_TTF/lib/Linux_x64/"
        "${PROJECT_SOURCE_DIR}/../dependency/SDL2_Image/lib/Linux_x64/"
        "${PROJECT_SOURCE_DIR}/../dependency/SDL2_Mixer/lib/Linux_x64/"
        )

    if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
        message(FATAL_ERROR "Linux platform require you to set CMAKE_BUILD_TYPE to Debug, RelWithDebInfo or Release.")
    endif()
else()
    message(FATAL_ERROR "Unknown platform.")
endif()


include_directories( "${PROJECT_SOURCE_DIR}/../dependency/SDL2/include"
                     "${PROJECT_SOURCE_DIR}/../dependency/SDL2_TTF/include"
                     "${PROJECT_SOURCE_DIR}/../dependency/SDL2_Image/include"
                     "${PROJECT_SOURCE_DIR}/../dependency/SDL2_Mixer/include" )
link_directories( ${LIB_DIRS} )
if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SRC_LIST} "Heroes2.rc")
else()
    add_executable(${PROJECT_NAME} ${SRC_LIST})
endif()
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${LIBS} )

