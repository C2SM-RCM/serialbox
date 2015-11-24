set(
    GENERIC_SOURCES
    "Logger.cpp"
)

set(
    GENERIC_HEADERS
    "Logger.h"
)

# Compilation stage

add_library(utils_files OBJECT ${GENERIC_SOURCES} ${GENERIC_HEADERS})

add_library(
    Utils STATIC
    $<TARGET_OBJECTS:utils_files>
)
install (TARGETS Utils DESTINATION "lib")

# Install headers
install (FILES ${GENERIC_HEADERS} DESTINATION "include/SerTools")
