
include_directories( SYSTEM ${CMAKE_SOURCE_DIR}/libs/libjson )
include_directories( ${CMAKE_SOURCE_DIR}/src)
include_directories( ${CMAKE_SOURCE_DIR}/libs/gmock-gtest)

add_subdirectory(serializer)
add_subdirectory(wrapper)
