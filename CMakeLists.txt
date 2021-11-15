cmake_minimum_required(VERSION 3.12)
project(native)

aux_source_directory(src SRC_DIR)

aux_source_directory(silk SRC_DIR)

aux_source_directory(lame/libmp3lame SRC_DIR)

aux_source_directory(lame/mpglib SRC_DIR)

include_directories("include" silk lame lame/mpglib lame/libmp3lame)

set(CMAKE_ANDROID_ARM_MODE ON)
set(CMAKE_ANDROID_ARM_NEON ON)
set(CMAKE_ANDROID_STL_TYPE system )

add_library(silklame SHARED ${SRC_DIR})

install(TARGETS silklame
        CONFIGURATIONS Debug
        LIBRARY  DESTINATION  ${CMAKE_CACHEFILE_DIR}/Debug
        RUNTIME  DESTINATION  ${CMAKE_CACHEFILE_DIR}/Debug
        )
install(TARGETS silklame
        CONFIGURATIONS Release
        LIBRARY DESTINATION ${CMAKE_CACHEFILE_DIR}/Release/
        RUNTIME DESTINATION ${CMAKE_CACHEFILE_DIR}/Release/
)
