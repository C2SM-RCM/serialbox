
include_directories( SYSTEM ${CMAKE_SOURCE_DIR}/libs/libjson )
include_directories( ${CMAKE_SOURCE_DIR}/src)

add_executable(
    dump
    dump.cpp
)
set_target_properties(
    dump PROPERTIES COMPILE_FLAGS "${CXX11_FLAGS}"
)

add_executable(
    compare
    compare.cpp
)
set_target_properties(
    compare PROPERTIES COMPILE_FLAGS "${CXX11_FLAGS}"
)
target_link_libraries( compare SerialBox Utils json sha256)

target_link_libraries( dump SerialBox Utils json sha256)

install(TARGETS compare DESTINATION "bin")
install(TARGETS dump DESTINATION "bin")
