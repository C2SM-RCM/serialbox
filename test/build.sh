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
    local idir=$1

    # construct cmake arguments
    local CMAKEARGS=(..
           "-DCMAKE_INSTALL_PREFIX=${idir}"
           "-DBoost_NO_SYSTEM_PATHS=TRUE" # http://stackoverflow.com/a/13204207/592024
           "-DBoost_NO_BOOST_CMAKE=TRUE"
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
        --idir|-i) install_dir="$2"; shift 2;;
        --local) install_local="yes"; shift;;
        -z) clean_build="yes"; shift;;
        -- ) shift; break ;;
        * ) break ;;
    esac
done

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

cmakeConfigure "${install_dir}"
if [ $? -ne 0 ]; then
    exitError 4430 ${LINENO} "Unable to configure cmake"
fi

make install 
if [ $? -ne 0 ]; then
    exitError 4420 ${LINENO} "Unable to configure cmake"
fi
