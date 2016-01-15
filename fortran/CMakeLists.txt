
SET(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -cpp ")
set(
    SOURCES
    "m_serialize.f90"
    "utils_ppser.f90"
)

add_library(
    FortranSer STATIC ${SOURCES}
)

install( TARGETS FortranSer DESTINATION lib/ )

