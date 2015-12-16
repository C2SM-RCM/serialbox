set(
    SOURCES
    "SerializationWrapper.h"
    "SerializationWrapper.cpp"
)

add_library(serialbox_wrapper_files OBJECT ${SOURCES})


add_library(
    SerialBoxWrapper STATIC
    ${SOURCES}
)
target_link_libraries( SerialBoxWrapper SerialBox Utils json sha256 )
install( TARGETS SerialBoxWrapper DESTINATION lib/ )

add_library(SerialBox_Wrapper SHARED
    $<TARGET_OBJECTS:json_files>
    $<TARGET_OBJECTS:sha256_files>
    $<TARGET_OBJECTS:utils_files>
    $<TARGET_OBJECTS:serialbox_files>
    $<TARGET_OBJECTS:serialbox_wrapper_files>
)
install(TARGETS SerialBoxWrapper DESTINATION "lib")

# The python library expects a serialbox dynamic library distributed with itself.
install(TARGETS SerialBox_Wrapper DESTINATION ${PYTHON_PATH}/serialbox/)
