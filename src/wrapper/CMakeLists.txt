set(
    SOURCES
    "SerializationWrapper.h"
    "SerializationWrapper.cpp"
)

add_library(serial_box_wrapper_files OBJECT ${SOURCES})


add_library(
    SerialBoxWrapper STATIC
    ${SOURCES}
)
target_link_libraries( SerialBoxWrapper SerialBox Utils json sha256 )
install( TARGETS SerialBoxWrapper DESTINATION lib/ )

set (PYTHON_PATH "python/serial_box")

INSTALL(DIRECTORY
    "${CMAKE_SOURCE_DIR}/python/"
    DESTINATION
    "${PYTHON_PATH}")

add_library(SerialBox_Wrapper SHARED
    $<TARGET_OBJECTS:json_files>
    $<TARGET_OBJECTS:sha256_files>
    $<TARGET_OBJECTS:utils_files>
    $<TARGET_OBJECTS:serial_box_files>
    $<TARGET_OBJECTS:serial_box_wrapper_files>
)
install(TARGETS SerialBoxWrapper DESTINATION "lib")
install(TARGETS SerialBox_Wrapper DESTINATION ${PYTHON_PATH})
