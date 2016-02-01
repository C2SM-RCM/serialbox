#!/bin/bash
exitError()
{
    echo "ERROR $1: $3" 1>&2
    echo "ERROR     LOCATION=$0" 1>&2
    echo "ERROR     LINE=$2" 1>&2
    exit $1
}

cmakeConfigure()
{
    local fcomp=$1
    local idir=$2

    # construct cmake arguments
    local CMAKEARGS=(..
           "-DCMAKE_Fortran_COMPILER=${fcomp}"
           "-DCMAKE_INSTALL_PREFIX=${idir}"
           "-DCMAKE_CXX_COMPILER=g++"
           "-DCMAKE_C_COMPILER=gcc"
           "-DBOOST_ROOT=${BOOST_PATH}"
    )

    local logfile=`pwd`/cmake.log

    echo "CMAKE ${CMAKEARGS[@]}"
    cmake "${CMAKEARGS[@]}" 2>&1 1>> ${logfile}
}


echo "TEST $@"

TEMP=`getopt -o h,f:,i:,z --long fcompiler:,idir:,local \
             -n 'build' -- "$@"`
if [ $? -ne 0 ]
then
    exitError 4440 "Wrong options $@"
fi

eval set -- "$TEMP"

while true; do
    case "$1" in
        --fcompiler|-f ) fortran_compiler="$2"; shift 2;;
        --idir|-i) install_dir="$2"; shift 2;;
        --local) install_local="yes"; shift;;
        -z) clean_build="yes"; shift;;
        -- ) shift; break ;;
        * ) break ;;
    esac
done

if [[ -z ${fortran_compiler} ]] ; then
    exitError 4410 ${LINENO} "compiler option has to be specified"
fi

base_path=$PWD
build_dir=$base_path/build

mkdir -p $build_dir
if [[ "${clean_build}" == "yes" ]]; then
    rm -rf ${build_dir}
fi

mkdir -p $build_dir

cd $build_dir

if [[ ${install_local} == "yes" ]]; then
    install_dir=${base_path}/install
fi

cmakeConfigure "${fortran_compiler}" "${install_dir}"

make install

