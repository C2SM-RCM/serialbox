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
    local enable_single=$1
    local cxx_=$2
    

    # construct cmake arguments
    local CMAKEARGS=(..
           "-DSINGLEPRECISION=${enable_single}"
           "-DCMAKE_CXX_COMPILER=${cxx_}"
    )

    local logfile=`pwd`/cmake.log

    cmake "${CMAKEARGS[@]}" 2>&1 1>> ${logfile}
}


TEMP=`getopt -o h --long target:,std:,prec: \
             -n 'build' -- "$@"`

eval set -- "$TEMP"

while true; do
    case "$1" in
        --single|-s) single_precision="ON"; shift 2;;
        -- ) shift; break ;;
        * ) break ;;
    esac
done

if [[ -z ${single_precision} ]] ; then
    single_precision="OFF"
fi

base_path=$PWD
build_dir=$base_path/build

mkdir -p $build_dir

cd $build_dir

cmakeConfigure "${single_precision}" 

echo "BASE " $base_path
