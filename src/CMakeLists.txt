include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

# Serialization
include_directories( SYSTEM ${CMAKE_SOURCE_DIR}/libs/libjson )
include_directories( ${CMAKE_SOURCE_DIR}/libs/sha256 )
include_directories( ${CMAKE_SOURCE_DIR}/libs/gmock-gtest)
include_directories( ${CMAKE_SOURCE_DIR}/src)

add_subdirectory( utils )
add_subdirectory( serializer )
add_subdirectory( wrapper )
