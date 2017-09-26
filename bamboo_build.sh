#!/bin/bash -e
mkdir -p .distfiles/gtest
if [ ! -e .distfiles/gtest/release-1.7.0.tar.gz ]; then
  curl -sL http://ossnexus/repository/unsupported/distfiles/googletest/release-1.7.0.tar.gz \
    -o .distfiles/gtest/release-1.7.0.tar.gz
fi
tar zxf .distfiles/gtest/release-1.7.0.tar.gz -C repos/
ln -sfn googletest-release-1.7.0 repos/gtest
rm -rf staging tarballs
mkdir -p staging tarballs
mkdir -p staging/pbbam/lib
mkdir -p staging/pbbam/bin
mkdir -p staging/pbbam/include
mkdir -p staging/blasr_libcpp/lib
mkdir -p staging/blasr_libcpp/include
mkdir -p staging/blasr/bin

set +x
type module >& /dev/null || . /mnt/software/Modules/current/init/bash
module load git/2.8.3
module load gcc/6.4.0
module load ccache/3.2.3
export CCACHE_DIR=/mnt/secondary/Share/tmp/bamboo.mobs.ccachedir
export CCACHE_COMPILERCHECK='%compiler% -dumpversion'
module load boost/1.60
if [[ $BOOST_ROOT =~ /include ]]; then
  set -x
  BOOST_ROOT=$(dirname $BOOST_ROOT)
  set +x
fi
module load ninja/1.7.1
module load cmake/3.7.2
module load hdf5-tools/1.8.19
module load zlib/1.2.8
module load htslib/1.3.1
set -x

cd repos/pbbam
export CCACHE_BASEDIR=$PWD
mkdir build
cd build
rm -rf * && CFLAGS=-fPIC CXXFLAGS=-fPIC CMAKE_BUILD_TYPE=ReleaseWithAssert cmake -GNinja ..
ninja
rsync -avx ../include bin lib ../../../staging/pbbam/

cd ../../blasr_libcpp
export CCACHE_BASEDIR=$PWD
rm -f defines.mk
CXXFLAGS="-std=c++14" \
python configure.py \
      PREFIX=dummy \
    HDF5_INC=$(pkg-config --cflags-only-I hdf5|awk '{print $1}'|sed -e 's/^-I//') \
    HDF5_LIB=$(pkg-config --libs-only-L hdf5|awk '{print $1}'|sed -e 's/^-L//') \
    ZLIB_LIB=$ZLIB_ROOT/lib \
   PBBAM_INC=$PWD/../pbbam/include \
   PBBAM_LIB=$PWD/../pbbam/build/lib \
   BOOST_INC=$BOOST_ROOT/include \
  HTSLIB_INC=$(pkg-config --cflags-only-I htslib|awk '{print $1}'|sed -e 's/^-I//') \
  HTSLIB_LIB=$(pkg-config --libs-only-L htslib|awk '{print $1}'|sed -e 's/^-L//')
make -j libpbdata LDLIBS=-lpbbam
make -j libpbihdf
make -j libblasr
tar c `find alignment hdf pbdata \( -name '*.hpp' -or -name '*.h' \)` \
  | tar x -C ../../staging/blasr_libcpp/include
cp -a pbdata/libpbdata.so*   ../../staging/blasr_libcpp/lib/
cp -a hdf/libpbihdf.so*      ../../staging/blasr_libcpp/lib/
cp -a alignment/libblasr.so* ../../staging/blasr_libcpp/lib/

cd ../blasr
export CCACHE_BASEDIR=$PWD
rm -f defines.mk
CXXFLAGS="-std=c++14" \
python configure.py --shared \
       PREFIX=dummy \
     HDF5_INC=$(pkg-config --cflags-only-I hdf5|awk '{print $1}'|sed -e 's/^-I//') \
     HDF5_LIB=$(pkg-config --libs-only-L hdf5|awk '{print $1}'|sed -e 's/^-L//') \
     ZLIB_LIB=$ZLIB_ROOT/lib \
    PBBAM_INC=$PWD/../pbbam/include \
    PBBAM_LIB=$PWD/../pbbam/build/lib \
    BOOST_INC=$BOOST_ROOT/include \
LIBPBDATA_INC=$PWD/../blasr_libcpp/pbdata \
LIBPBIHDF_INC=$PWD/../blasr_libcpp/hdf \
 LIBBLASR_INC=$PWD/../blasr_libcpp/alignment \
LIBPBDATA_LIB=$PWD/../blasr_libcpp/pbdata \
LIBPBIHDF_LIB=$PWD/../blasr_libcpp/hdf \
 LIBBLASR_LIB=$PWD/../blasr_libcpp/alignment \
   HTSLIB_INC=$(pkg-config --cflags-only-I htslib|awk '{print $1}'|sed -e 's/^-I//') \
   HTSLIB_LIB=$(pkg-config --libs-only-L htslib|awk '{print $1}'|sed -e 's/^-L//')
VERBOSE=1 make
VERBOSE=1 CXXFLAGS="-std=c++14 -O3 -g" make -C utils
cp -a blasr          ../../staging/blasr/bin/
cp -a utils/sawriter ../../staging/blasr/bin/

cd ../..
cd staging/blasr   && tar zcf ../../tarballs/blasr.tgz        bin
cd ../blasr_libcpp && tar zcf ../../tarballs/blasr_libcpp.tgz lib include
cd ../pbbam        && tar zcf ../../tarballs/pbbam.tgz        lib include bin
