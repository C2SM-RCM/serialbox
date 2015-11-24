set( SHA256_SOURCES
"sha256.h"
"sha256.c"
)

add_library(sha256_files OBJECT ${SHA256_SOURCES} )
add_library(sha256 $<TARGET_OBJECTS:sha256_files>)

install( TARGETS sha256 DESTINATION lib/ )

