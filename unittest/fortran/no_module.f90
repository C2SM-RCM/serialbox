program driver

  integer :: ncol = 1, nlay = 42, ngpt = 256

  call solver(ncol, nlay, ngpt)

end program driver


subroutine solver(ncol, nlay, ngpt)

    integer   :: ncol, nlay, ngpt

    !$ser init prefix='output' prefix_ref='input' directory='.'
    !$ser mode read
    !$ser savepoint lw_solver_input

end subroutine solver
