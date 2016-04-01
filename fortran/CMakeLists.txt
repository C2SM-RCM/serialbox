
if(NOT "${CMAKE_Fortran_COMPILER_ID}" MATCHES "PGI")
    set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -cpp ")
endif()

set(
    SOURCES
    "m_serialize.f90"
    "utils_ppser.f90"
)

add_library(
    FortranSer STATIC ${SOURCES}
)

install( TARGETS FortranSer DESTINATION lib/ )

