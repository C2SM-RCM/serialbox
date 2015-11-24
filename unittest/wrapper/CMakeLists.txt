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
target_link_libraries( WrapperUnittest SerToolsWrapper gmock-gtest SerToolsWrapper SerTools SerToolsWrapper json sha256 Utils )
install( TARGETS WrapperUnittest DESTINATION bin/ )
sertools_add_test(WrapperUnittest)

