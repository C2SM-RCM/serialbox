#!/bin/bash -ex

# setup module environment
# NOTE: the location of the base bash script and module initialization
#       vary from system to system, so you will have to add the location
#       if your system is not supported below

exitError()
{
    echo "ERROR: $2" 1>&2
    exit $1
}

# setup empty defaults
host=""       # name of host
queue=""      # standard queue to submit jobs to
nthreads=""   # number of threads to use for parallel builds
mpilaunch=""  # command to launch an MPI executable (e.g. aprun)

# setup machine specifics
if [ "`hostname | grep todi`" != "" ] ; then
    . /etc/bash.bashrc
    . /opt/modules/default/init/bash
    host="todi"
    queue="day"
    nthreads=16
    mpilaunch="aprun"
elif [ "`hostname | grep opcode`" != "" ] ; then
    . /etc/bashrc
    . /etc/profile.d/modules.sh
    host="opcode"
    queue="primary"
    nthreads=8
    mpilaunch="mpirun"
elif [ "`hostname | grep lema`" != "" ] ; then
    . /etc/bash.bashrc
    . /opt/modules/default/init/bash
    host="lema"
    queue="dev"
    nthreads=12
    mpilaunch="aprun"
elif [ "`hostname | grep dom`" != "" ] ; then
    . /etc/bashrc
    . /apps/dom/Modules/3.2.10/init/bash
    host="dom"
    queue="normal"
    nthreads=8
    mpilaunch="mpiexec"
elif [ "`hostname | grep daint`" != "" ] ; then
    . /etc/bash.bashrc
    . /opt/modules/default/init/bash
    host="daint"
    queue="normal"
    nthreads=8
    mpilaunch="aprun"
elif [ "`hostname | grep jupiter`" != "" ] ; then
    . /etc/bash.bashrc
    . /opt/modules/default/init/bash
    host="jupiter"
    queue="batch"
    nthreads=8
    mpilaunch="aprun"
elif [ "`hostname | grep castor`" != "" ] ; then
    . /etc/bashrc
    . /apps/castor/Modules/default/init/bash
    host="castor"
    queue="normal"
    nthreads=6
    mpilaunch="mpiexec"
elif [ "`hostname | grep santis`" != "" ] ; then
    . /etc/bash.bashrc
    . /opt/modules/default/init/bash
    host="santis"
    queue="normal"
    nthreads=8
    mpilaunch="aprun"
elif [ "`hostname | grep durian`" != "" ] ; then
    shopt -s expand_aliases
    alias module='echo $* 2>/dev/null 1>/dev/null'
    host="durian"
    queue="normal"
    nthreads=4
    mpilaunch="mpirun"
fi

# make sure everything is set
test -n "${host}" || exitError 2001 "Variable <host> could not be set (unknown machine `hostname`?)"
test -n "${queue}" || exitError 2002 "Variable <queue> could not be set (unknown machine `hostname`?)"
test -n "${nthreads}" || exitError 2003 "Variable <nthreads> could not be set (unknown machine `hostname`?)"
test -n "${mpilaunch}" || exitError 2004 "Variable <mpilaunch> could not be set (unknown machine `hostname`?)"

