MODULE m_ser

  CONTAINS

  SUBROUTINE serialize(a)
    REAL(KIND=8), DIMENSION(:,:,:) :: a

    !$ser init directory='.' prefix='SerialboxTest'
    !$ser savepoint sp1
    !$ser mode write
    !$ser data ser_a=a

  END SUBROUTINE serialize

  SUBROUTINE dummy(a)
    IMPLICIT NONE
    REAL(KIND=8), DIMENSION(:,:,:) :: a
    !$ser init directory='.' prefix='SerialboxTest'
    !$ser savepoint sp1
    !$ser mode write
    !$ser data ser_a=a
  END SUBROUTINE dummy

END MODULE m_ser
