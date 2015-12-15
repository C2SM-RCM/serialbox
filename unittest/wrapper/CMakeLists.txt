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
target_link_libraries( WrapperUnittest SerialBoxWrapper gmock-gtest SerialBoxWrapper SerialBox SerialBoxWrapper json sha256 Utils )
install( TARGETS WrapperUnittest DESTINATION bin/ )
serial_box_add_test(WrapperUnittest)

