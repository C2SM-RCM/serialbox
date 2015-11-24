#!/bin/bash -f

exitError()
{
    echo "ERROR $1: $3" 1>&2
    echo "ERROR     LOCATION=$0" 1>&2
    echo "ERROR     LINE=$2" 1>&2
    exit $1
}


TEMP=`getopt -o h,p:d: --long package:dir: \
             -n 'build.sh' -- "$@"`
eval set -- "$TEMP"

echo "TURU $@"
while true; do
    case "$1" in
        --dir|-d) package_basedir=$2; echo "PEPELINO" ; shift 2;;
        -- ) echo "ASH"; shift; break ;;
        * ) echo "ASJ $1 $2"; break ;;
    esac
done

echo $@

if [[ -z ${package_basedir} ]]; then
    echo "DJ " ${package_basedir}
    exitError 3221 ${LINENO} "package basedir has to be specified"
fi

echo $@

#setFortranEnvironment
#writeModuleList ${base_path}/modules.log loaded "FORTRAN MODULES" ${base_path}/modules_fortran.env

${package_basedir}/test/build.sh

#buildExecutable
#unsetFortranEnvironment

#echo "LL"
