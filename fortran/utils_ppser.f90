!This file is released under terms of BSD license`
!See LICENSE.txt for more information
!
!+ Module containing procedures for input/output to/from disk
!------------------------------------------------------------------------------

MODULE utils_ppser

!------------------------------------------------------------------------------
!
! Description:
!   This module contains utilities for the pp_ser serialization helper.
!   The functions and subroutines defined in this module are not intended to
!   be used directly: they will be used by the preprocessor at compilation
!   time.
!
! Current Code Owner: ETH, Andrea Arteaga
!  email:  andrea.arteaga@env.ethz.ch
!
! Code Description:
! Language: Fortran 90.
! Software Standards: "European Standards for Writing and
! Documenting Exchangeable Fortran 90 Code".
!============================================================================

USE iso_fortran_env
USE iso_c_binding
USE m_serialize

  IMPLICIT NONE

  TYPE(t_serializer) :: ppser_serializer
  TYPE(t_serializer) :: ppser_serializer_ref
  TYPE(t_savepoint)  :: ppser_savepoint
  LOGICAL            :: ppser_initialized = .false.

  INTEGER            :: ppser_intlength, ppser_reallength
  CHARACTER (LEN=6)  :: ppser_realtype

  ! 0 corresponds to "read"
  ! 1 corresponds to "write"
  INTEGER            :: ppser_mode = 0

PUBLIC :: &
  ppser_serializer, ppser_savepoint, ppser_initialize, &
  ppser_intlength, ppser_reallength, ppser_realtype,   &
  ppser_set_mode, ppser_get_mode, ppser_serializer_ref

CONTAINS

!============================================================================

SUBROUTINE ppser_initialize(directory, prefix, mode, prefix_ref, mpi_rank)
  CHARACTER(LEN=*), INTENT(IN)     :: directory, prefix
  INTEGER, OPTIONAL, INTENT(IN)    :: mode
  CHARACTER(LEN=*), OPTIONAL, INTENT(IN)     :: prefix_ref
  REAL                             :: realvalue
  INTEGER                          :: intvalue
  INTEGER, OPTIONAL, INTENT(IN)    :: mpi_rank
  CHARACTER(LEN=1), DIMENSION(128) :: buffer
  CHARACTER(LEN=15)                :: suffix

  ! Initialize serializer and savepoint
  IF ( .NOT. ppser_initialized ) THEN
    IF ( PRESENT(mpi_rank) ) THEN
      WRITE(suffix, '(A5,I0)') "_rank", mpi_rank
      CALL fs_create_serializer(directory, TRIM(prefix)//TRIM(suffix), 'w', ppser_serializer)
    ELSE 
      CALL fs_create_serializer(directory, TRIM(prefix), 'w', ppser_serializer)
    END IF
    CALL fs_create_savepoint('', ppser_savepoint)
    IF ( PRESENT(mode) ) ppser_mode = mode
    IF ( PRESENT(prefix_ref) ) THEN
      IF ( PRESENT(mpi_rank) ) THEN 
        CALL fs_create_serializer(directory, TRIM(prefix_ref)//TRIM(suffix), 'r', ppser_serializer_ref)
      ELSE
        CALL fs_create_serializer(directory, TRIM(prefix_ref), 'r', ppser_serializer_ref)
      END IF
    END IF
  END IF
  ppser_initialized = .TRUE.

  ! Get data size
  intvalue = 0
  realvalue = 0.
  ppser_intlength = INT(SIZE(TRANSFER(intvalue, buffer)))
  ppser_reallength = INT(SIZE(TRANSFER(realvalue, buffer)))

  ! Get name of real
  ppser_realtype = 'double'

END SUBROUTINE ppser_initialize


SUBROUTINE ppser_finalize()

  IF ( ppser_initialized ) THEN
    CALL fs_destroy_savepoint(ppser_savepoint)
    CALL fs_destroy_serializer(ppser_serializer)
  ENDIF
  ppser_initialized = .FALSE.

END SUBROUTINE ppser_finalize


SUBROUTINE ppser_set_mode(mode)
  INTEGER, INTENT(IN) :: mode

  ppser_mode = mode

END SUBROUTINE ppser_set_mode

FUNCTION ppser_get_mode()
  INTEGER :: ppser_get_mode

  ppser_get_mode = ppser_mode

END FUNCTION ppser_get_mode

END MODULE utils_ppser
