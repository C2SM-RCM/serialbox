set( GMOCK_GTEST_SOURCES
"gmock-gtest-all.cc"
"gmock_main.cc"
)

set( GTEST_INSTALL_HEADERS
    "gtest/gtest.h"
)

include_directories ( "." ) 
include_directories ( "./gtest" ) 

# generatring the library called gmock-gtest 

add_library( gmock-gtest ${GMOCK_GTEST_SOURCES} )

install( TARGETS gmock-gtest DESTINATION lib/ )

install (FILES ${GTEST_INSTALL_HEADERS} DESTINATION "include/STELLA/gtest")

