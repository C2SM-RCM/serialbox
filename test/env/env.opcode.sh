#!/bin/bash

# This script contains functions for setting up machine specific compile
# environments for the dycore and the Fortran parts. Namely, the following
# functions must be defined in this file:
#
# setupDefaults            setup global default options for this platform
# setCppEnvironment        setup environment for dycore compilation
# unsetCppEnvironment      restore environment after dycore compilation
# setFortranEnvironment    setup environment for Fortran compilation
# unsetFortranEnvironment  restore environment after Fortran compilation

# Setup global defaults and variables
#
# upon exit, the following global variables need to be set:
#   targets           list of possible targets (e.g. gpu, cpu)
#   compilers         list of possible compilers for Fortran parts
#   target            default target
#   compiler          default compiler to use for Fortran parts
#   debug             build in debugging mode (yes/no)
#   cleanup           clean before build (yes/no)
#   cuda_arch         CUDA architecture version to use (e.g. sm_35, use blank for CPU target)
#
setupDefaults()
{
    # available options
    targets=(cpu gpu)
    compilers=(pgi gnu)
    fcompiler_cmds=(mpif90)


    # default options
    target="gpu"
    compiler="pgi"
    cuda_arch="sm_35"

    #fortran compiler command
    fcompiler_cmd="mpif90"

}

# This function loads modules and sets up variables for compiling in C++
#
# upon entry, the following global variables need to be set:
#   compiler          Compiler to use to compile the Fortran parts of the code
#
# upon exit, the following global variables need to be set:
#   old_prgenv        Default PrgEnv-XXX module loaded on Cray machines
#   dycore_gpp        C++ compiler for dycore
#   dycore_gcc        C compiler for dycore
#   cuda_gpp          C++ used by nvcc as backend
#   boost_path        path to the Boost installation to use
#   use_mpi_compiler  use MPI compiler wrappers?
#   mpi_path          path to the MPI installation to use
#
setCppEnvironment()
{
    # switch to programming environment (only on Cray)
    old_prgenv=" "
    #old_prgenv=`module list -t 2>&1 | grep 'PrgEnv-'`
    #if [ -z "${old_prgenv}" ] ; then
    #    module load PrgEnv-gnu
    #else
    #    module swap ${old_prgenv} PrgEnv-gnu
    #fi
    
    # standard modules (part 1)
    module load cmake/2.8.11.2

    # Fortran compiler specific modules and setup
    case "${compiler}" in
    pgi )
        module load mvapich2/1.9-pgi-opcode3-13.6
        module load gcc/4.7.2
        ;;
    gnu )
        module load mvapich2/1.9-gcc-opcode3-4.6.3
        module load gcc/4.7.2
        ;;
    * )
        echo "ERROR: Unsupported compiler encountered in setCppEnvironment" 1>&2
        exit 1
    esac

    # standard modules (part 2)
    module load cuda/5.5

    # set global variables
    if [ "${compiler}" == "gnu" ] ; then
        dycore_openmp=ON   # OpenMP only works if GNU is also used for Fortran parts
    else
        dycore_openmp=OFF  # Otherwise, switch off
    fi
    dycore_gpp='g++'
    dycore_gcc='gcc'
    cuda_gpp='g++'
    boost_path=/apps/todi/boost/1.49/include/boost-1.49.0
    use_mpi_compiler=ON
    mpi_path=${MPI_ROOT}
}

# This function unloads modules and removes variables for compiling in C++
#
# upon entry, the following global variables need to be set:
#   compiler          Compiler to use to compile the Fortran parts of the code
#   old_prgenv        Default PrgEnv-XXX module loaded on Cray machines
#
unsetCppEnvironment()
{
    # remove standard modules (part 2)
    module unload cuda

    # remove Fortran compiler specific modules
    case "${compiler}" in
    pgi )
        module unload gcc/4.7.2
        module unload mvapich2/1.9-pgi-opcode3-13.6
        ;;
    gnu )
        module unload gcc/4.7.2
        module unload mvapich2/1.9-gcc-opcode3-4.6.3
        ;;
    * )
        echo "ERROR: Unsupported compiler encountered in unsetCppEnvironment" 1>&2
        exit 1
    esac

    # remove standard modules (part 1)
    module unload cmake

    ## restore programming environment (only on Cray)
    #if [ -z "${old_prgenv}" ] ; then
    #    module unload PrgEnv-gnu
    #else
    #    module swap PrgEnv-gnu ${old_prgenv}
    #fi
    unset old_prgenv

    # unset global variables
    unset dycore_openmp
    unset dycore_gpp
    unset dycore_gcc
    unset cuda_gpp
    unset boost_path
    unset use_mpi_compiler
    unset mpi_path
}

# This function loads modules and sets up variables for compiling the Fortran part
#
# upon entry, the following global variables need to be set:
#   compiler          Compiler to use to compile the Fortran part of the code
#
# upon exit, the following global variables need to be set:
#   old_prgenv        Default PrgEnv-XXX module loaded on Cray machines
#
setFortranEnvironment()
{
    # switch to GNU programming environment (only on Cray machines)
    old_prgenv=" "
    #old_prgenv=`module list -t 2>&1 | grep 'PrgEnv-'`
    #if [ -z "${old_prgenv}" ] ; then
    #    module load PrgEnv-${compiler}
    #else
    #    module swap ${old_prgenv} PrgEnv-${compiler}
    #fi

    # standard modules (part 1)
    module load cmake/2.8.11.2

    # compiler specific modules
    case "${compiler}" in
    pgi )
        module load pgi/13.10
        module load mvapich2/1.9-pgi-opcode3-13.6
        module load netcdf/4.2-pgi
        module unload pgi/13.2
        ;;
    gnu )
        module load mvapich2/1.9-gcc-opcode3-4.6.3
        module load gcc/4.7.2
        module load netcdf/4.2-gcc
        module unload gcc/4.6.3
        ;;
    * )
        echo "ERROR: Unsupported compiler encountered in setFortranEnvironment" 1>&2
        exit 1
    esac

    # standard modules (part 2)
    module load cuda/5.5
}

# This function unloads modules and removes variables for compiling the Fortran parts
#
# upon entry, the following global variables need to be set:
#   compiler          Compiler to use to compile the Fortran parts of the code
#   old_prgenv        Default PrgEnv-XXX module loaded on Cray machines
#
unsetFortranEnvironment()
{
    # remove standard modules (part 2)
    module unload cuda

    # remove compiler specific modules
    case "${compiler}" in
    pgi )
        module unload netcdf/4.2-pgi
        module unload pgi/13.10
        module unload mvapich2/1.9-pgi-opcode3-13.6
        ;;
    gnu )
        module unload netcdf/4.2-gcc
        module unload gcc/4.7.2
        module unload mvapich2/1.9-gcc-opcode3-4.6.3
        ;;
    * )
        echo "ERROR: Unsupported compiler encountered in unsetFortranEnvironment" 1>&2
        exit 1
    esac

    # remove standard modules (part 1)
    module unload cmake

    # swap back to original programming environment (only on Cray machines)
    #if [ -z "${old_prgenv}" ] ; then
    #    module unload PrgEnv-${compiler}
    #else
    #    module swap PrgEnv-${compiler} ${old_prgenv}
    #fi
    unset old_prgenv
}

