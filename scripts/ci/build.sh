#!/bin/bash
set -euo pipefail

echo "#############################"
echo "# LOAD MODULES"
type module >& /dev/null || source /mnt/software/Modules/current/init/bash
module purge
module load git
module load gcc
CCACHE_BASEDIR=$PWD
module load ccache
module load boost
module load ninja
module load cmake
module load hdf5-tools
module load zlib
module load htslib

echo "#############################"
echo "# PRE-BUILD HOOK"
echo "## Check formatting"
./tools/check-formatting --all

(cd libcpp && git clean -xdf)
(cd pbbam && git clean -xdf)

echo "#############################"
echo "# BUILD"
rm -rf build
mkdir -p build
cd build
cmake \
    -DCMAKE_BUILD_TYPE=ReleaseWithAssert \
    -DHDF5_ROOT=$HDF5_DIR \
    -GNinja \
  ..
sed -i -e 's@/-I@/ -I@g' build.ninja
ninja
cd ..

echo "#############################"
echo "# TEST"

export PATH=$PWD/build:$PATH
mkdir -p test-reports
module purge
module load gcc
module load hdf5-tools
module load zlib
module load htslib
module load samtools
module load cram

#make -f cram.mk \
#    XUNIT="--xunit-file=$PWD/test-reports/blasr-cram_xunit.xml" \
#    cramfast
make -f cram.mk \
    cramfast
exit $?
