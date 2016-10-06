set(
    SOURCES
    "SerializationWrapper.h"
    "SerializationWrapper.cpp"
)

add_library(serialbox_wrapper_files OBJECT ${SOURCES})


add_library(
    SerialboxWrapper STATIC
    ${SOURCES}
)
target_link_libraries( SerialboxWrapper Serialbox Utils json sha256 )
install( TARGETS SerialboxWrapper DESTINATION lib/ )

add_library(Serialbox_Wrapper SHARED
    $<TARGET_OBJECTS:json_files>
    $<TARGET_OBJECTS:sha256_files>
    $<TARGET_OBJECTS:utils_files>
    $<TARGET_OBJECTS:serialbox_files>
    $<TARGET_OBJECTS:serialbox_wrapper_files>
)
install(TARGETS SerialboxWrapper DESTINATION "lib")

# The python library expects a serialbox dynamic library distributed with itself.
install(TARGETS Serialbox_Wrapper DESTINATION ${PYTHON_PATH}/serialbox/)
