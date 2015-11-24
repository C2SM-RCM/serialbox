 set(
    SOURCES
    "MetainfoUnittest.cpp"
    "OffsetTableUnittest.cpp"
    "FieldsTableUnittest.cpp"
    "CentralizedFileFormatUnittest.cpp"
    "SerializerUnittest.cpp"
    "BinarySerializerUnittest.cpp"
)

add_executable(
    SerializerUnittest
    ${SOURCES}
)

sertools_add_test(SerializerUnittest)
target_link_libraries( SerializerUnittest gmock-gtest SerTools json sha256 Utils )
install( TARGETS SerializerUnittest DESTINATION bin/ )

