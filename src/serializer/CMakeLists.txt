set(
    GENERIC_SOURCES
    "MetainfoSet.cpp"
    "DataFieldInfo.cpp"
    "Savepoint.cpp"
    "Checksum.cpp"
    "OffsetTable.cpp"
    "FieldsTable.cpp"
    "FileFormat.cpp"
    "CentralizedFileFormat.cpp"
    "Serializer.cpp"
    "BinarySerializer.cpp"
)

set(
    GENERIC_HEADERS
    "SerializationException.h"
    "TypeName.h"
    "MetainfoSet.h"
    "DataFieldInfo.h"
    "KBoundary.h"
    "Checksum.h"
    "Savepoint.h"
    "OffsetTable.h"
    "IJKSize.h"
    "IJKBoundary.h"
    "FieldsTable.h"
    "FileFormat.h"
    "CentralizedFileFormat.h"
    "Serializer.h"
    "BinarySerializer.h"
)

# Compilation stage

add_library(serialbox_files OBJECT ${GENERIC_SOURCES} ${GENERIC_HEADERS})

add_library(
    Serialbox STATIC ${GENERIC_SOURCES}
)
install (TARGETS Serialbox DESTINATION "lib")

# Install headers
install (FILES ${GENERIC_HEADERS} DESTINATION "include/Serialbox")
