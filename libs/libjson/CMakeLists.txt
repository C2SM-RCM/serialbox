# ADDING JSON

set ( JSON_HEADERS
"libjson.h"
"JSONOptions.h"
)

set ( JSON_INTERNAL_HEADERS
"_internal/Source/internalJSONNode.h"
"_internal/Source/JSONAllocator.h"
"_internal/Source/JSON_Base64.h"
"_internal/Source/JSONChildren.h"
"_internal/Source/JSONDebug.h"
"_internal/Source/JSONDefs.h"
"_internal/Source/JSONGlobals.h"
"_internal/Source/JSONMemory.h"
"_internal/Source/JSONMemoryPool.h"
"_internal/Source/JSONNode.h"
"_internal/Source/JSONPreparse.h"
"_internal/Source/JSONSharedString.h"
"_internal/Source/JSONSingleton.h"
"_internal/Source/JSONStats.h"
"_internal/Source/JSONStream.h"
"_internal/Source/JSONValidator.h"
"_internal/Source/JSONWorker.h"
"_internal/Source/NumberToString.h"
)

set (JSON_DEFS_HEADERS
"_internal/Source/JSONDefs/GNU_C.h"
"_internal/Source/JSONDefs/Strings_Defs.h"
"_internal/Source/JSONDefs/Unknown_C.h"
"_internal/Source/JSONDefs/Visual_C.h"
)

set( JSON_SOURCES
"_internal/Source/internalJSONNode.cpp"
"_internal/Source/JSONAllocator.cpp"
"_internal/Source/JSONChildren.cpp"
"_internal/Source/JSONDebug.cpp"
"_internal/Source/JSONIterators.cpp"
"_internal/Source/JSONMemory.cpp"
"_internal/Source/JSONNode.cpp"
"_internal/Source/JSONNode_Mutex.cpp"
"_internal/Source/JSONPreparse.cpp"
"_internal/Source/JSONStream.cpp"
"_internal/Source/JSONValidator.cpp"
"_internal/Source/JSONWorker.cpp"
"_internal/Source/JSONWriter.cpp"
"_internal/Source/libjson.cpp"
)

# generatring the library called libjson 

add_library(json_files OBJECT ${JSON_HEADERS} ${JSON_INTERNAL_HEADERS} ${JSON_DEFS_HEADERS} ${JSON_SOURCES} )
add_library(json $<TARGET_OBJECTS:json_files>)

install( TARGETS json DESTINATION lib/ )
install (FILES ${JSON_HEADERS} DESTINATION "include/STELLA/libjson")
install (FILES ${JSON_INTERNAL_HEADERS} DESTINATION "include/STELLA/libjson/_internal/Source")
install (FILES ${JSON_DEFS_HEADERS} DESTINATION "include/STELLA/libjson/_internal/Source/JSONDefs")

