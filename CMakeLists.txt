# Top Level CMakeLists.txt File
cmake_minimum_required( VERSION 2.8 )
# set the installation prefix
# Note: setting this *before* the project imposes a default that the user is
# able to change.
SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install/" CACHE PATH "installation prefix")

project( SerialBox )
enable_language (Fortran)
find_package( Boost )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

set(ENABLE_DOC "OFF" CACHE BOOL "enable building doc" )
if(ENABLE_DOC)
    find_package(LATEX)
    add_subdirectory(doc)
endif(ENABLE_DOC)

set(TEST_SCRIPT ${CMAKE_BINARY_DIR}/run_tests.sh)
file(WRITE ${TEST_SCRIPT} "#!/bin/sh\n")
file(APPEND ${TEST_SCRIPT} "res=0\n")

set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_BINARY_DIR}/fortran )

install(DIRECTORY ${CMAKE_BINARY_DIR}/fortran/ DESTINATION include/fortran FILES_MATCHING PATTERN "*.mod" PATTERN "CMake*" EXCLUDE )

set(CXX11_FLAGS "-std=c++11")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

function(serialbox_add_test test_name )
  #ignore the execute prefix in case it is not a valid executable prefix
  add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}> ${ARGN})
  file(APPEND ${TEST_SCRIPT} "${CMAKE_CURRENT_BINARY_DIR}/${test_name}" " ${ARGN}" "\n")
  file(APPEND ${TEST_SCRIPT} "if [ $? -ne 0 ] ; then\n echo \"Error: problem found in Unittest\"\nfi\n")
  file(APPEND ${TEST_SCRIPT} "res=$((res || $? ))\n")
endfunction(serialbox_add_test)

if( CMAKE_HOST_UNIX )
    message(STATUS "Compiling shared library on Unix")
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
    ## if not compiled and link with -fopenmp, gtest will depend on pthread lib
    find_package(Threads)
    set ( CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINK_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
endif()

# Single precision
set( SINGLEPRECISION "OFF" CACHE BOOL "Single precision" )
if( SINGLEPRECISION )
    add_definitions( -DSINGLEPRECISION )
endif( SINGLEPRECISION )


# OSX specific configuration
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_USE_OWN_TR1_TUPLE=1")
    # CMake requires a RPATH configuration variable set on OSX with CMake >= 3.0
    # (CMake policy CMP0042) to build dynamic libraries. Setting this variable
    # to INSTALL/lib.
    # More information: http://www.cmake.org/Wiki/CMake_RPATH_handling
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_MACOSX_RPATH "${CMAKE_INSTALL_RPATH}")
endif ()

# Serialization
include_directories( SYSTEM libs/libjson )
include_directories( libs/sha256 )
include_directories( libs/gmock-gtest)
include_directories( src)


# Build sources
add_subdirectory( libs/libjson )
add_subdirectory( libs/sha256 )
add_subdirectory( libs/gmock-gtest )
add_subdirectory( src )
add_subdirectory( fortran )
add_subdirectory( tools )
add_subdirectory( unittest )

file(APPEND ${TEST_SCRIPT} "echo \"RETURNING $res\" \n exit $res\n")
file(INSTALL ${TEST_SCRIPT} DESTINATION ${CMAKE_BINARY_DIR}/install
   FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
)

