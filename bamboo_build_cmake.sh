#!/bin/bash
set -euo pipefail

echo "#############################"
echo "# LOAD MODULES"
type module >& /dev/null || source /mnt/software/Modules/current/init/bash
module purge
module load git
module load gcc/6.4.0
module load ccache/3.3.4
module load boost/1.60
module load ninja/1.7.1
module load cmake/3.2.2
module load hdf5-tools/1.8.19
module load zlib/1.2.8
module load htslib/1.3.1
export CCACHE_COMPILERCHECK='%compiler% -dumpversion'

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
    -DPacBioBAM_build_docs=OFF \
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
module load gcc/6.4.0
module load hdf5-tools/1.8.19
module load zlib/1.2.8
module load htslib/1.3.1
module load samtools
module load cram/0.7

make -f cram.mk \
    XUNIT="--xunit-file=$PWD/test-reports/blasr-cram_xunit.xml" \
    cramfast
exit $?
