#!/bin/bash
set -euo pipefail

echo "#############################"
echo "# LOAD MODULES"
source /mnt/software/Modules/current/init/bash
module load git gcc/4.9.2 ccache boost ninja cmake/3.7.2 hdf5-tools/1.8.16 zlib/1.2.8-cloudflare

echo "#############################"
echo "# PRE-BUILD HOOK"
echo "## Check formatting"
./tools/check-formatting --all

echo "#############################"
echo "# BUILD"
echo "## Create build directory "
if [ ! -d build ] ; then mkdir build ; fi

echo "## Build source"
( cd build &&\
  rm -rf * &&\
  CMAKE_BUILD_TYPE=ReleaseWithAssert cmake -GNinja -DHDF5_ROOT=$HDF5_DIR .. &&\
  ninja )

( cd build &&\
  rm -rf * &&\
  CMAKE_BUILD_TYPE=ReleaseWithAssert cmake -GNinja -DHDF5_LIBRARIES=$HDF5_DIR/lib -DHDF5_INCLUDE_DIRS=$HDF5_DIR/include .. &&\
  ninja )

echo "#############################"
echo "# TEST"
echo "## ctest"
# ( cd build && ninja check )