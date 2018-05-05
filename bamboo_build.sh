#!/usr/bin/env bash
set -e

echo "################"
echo "# DEPENDENCIES #"
echo "################"

echo "## Load modules"
type module >& /dev/null || . /mnt/software/Modules/current/init/bash

module purge

module load gcc
module load ccache

module load meson
module load ninja

module load boost
module load hdf5-tools
module load cram

module load libblasr
module load pbbam


BOOST_ROOT="${BOOST_ROOT%/include}"
# unset these variables to have meson discover all
# boost-dependent variables from BOOST_ROOT alone
unset BOOST_INCLUDEDIR
unset BOOST_LIBRARYDIR

export CC="ccache gcc"
export CXX="ccache g++"
export CCACHE_BASEDIR="${PWD}"

if [[ $USER == bamboo ]]; then
  export CCACHE_DIR=/mnt/secondary/Share/tmp/bamboo.${bamboo_shortPlanKey}.ccachedir
  export CCACHE_TEMPDIR=/scratch/bamboo.ccache_tempdir
fi

# call the main build script
export ENABLED_TESTS="true"
bash scripts/ci/build.sh
