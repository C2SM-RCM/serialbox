# ADDING JSON

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
add_library(json_files OBJECT ${JSON_SOURCES} )
add_library(json $<TARGET_OBJECTS:json_files>)

install( TARGETS json DESTINATION lib/ )

