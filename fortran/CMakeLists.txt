set(
    SOURCES
    "m_serialize.f90"
)

add_library(fortranser_files OBJECT ${SOURCES})

add_library(
    FortranSer STATIC ${SOURCES}
)

install( TARGETS FortranSer DESTINATION lib/ )

