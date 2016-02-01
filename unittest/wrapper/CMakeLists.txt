set(
    SOURCES
    "SerializerUnittest.cpp"
    "ReadWriteUnittest.cpp"
    "SavepointUnittest.cpp"
    "FieldUnittest.cpp"
)

add_executable(
    WrapperUnittest
    ${SOURCES}
)
target_link_libraries( WrapperUnittest SerialboxWrapper gmock-gtest SerialboxWrapper Serialbox SerialboxWrapper json sha256 Utils )
install( TARGETS WrapperUnittest DESTINATION bin/ )
serialbox_add_test(WrapperUnittest)

