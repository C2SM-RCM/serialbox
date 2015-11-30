set(
    SOURCES
    "SerializationWrapper.h"
    "SerializationWrapper.cpp"
)

add_library(sertools_wrapper_files OBJECT ${SOURCES})


add_library(
    SerToolsWrapper STATIC
    ${SOURCES}
)
target_link_libraries( SerToolsWrapper SerTools Utils json sha256 )
install( TARGETS SerToolsWrapper DESTINATION lib/ )

set (PYTHON_PATH "python/sertools")

INSTALL(DIRECTORY
    "${CMAKE_SOURCE_DIR}/python/"
    DESTINATION
    "${PYTHON_PATH}")

add_library(SerTools_Wrapper SHARED
    $<TARGET_OBJECTS:json_files>
    $<TARGET_OBJECTS:sha256_files>
    $<TARGET_OBJECTS:utils_files>
    $<TARGET_OBJECTS:sertools_files>
    $<TARGET_OBJECTS:sertools_wrapper_files>
)
install(TARGETS SerToolsWrapper DESTINATION "lib")
install(TARGETS SerToolsWrapper DESTINATION ${PYTHON_PATH})
