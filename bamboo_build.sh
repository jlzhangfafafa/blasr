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
mkdir -p staging/usr/local/bin
#/bin/ls -t pbbam-*.tgz        | head -1 | xargs -r -n 1 cat | tar zxv -C staging
/bin/ls -t blasr_libcpp-*.tgz | head -1 | xargs -r -n 1 cat | tar zxv -C staging
       STAGING=$(readlink -f staging)
#    PBBAM_INC=$STAGING/usr/local/include
#    PBBAM_LIB=$STAGING/usr/local/lib
 LIBPBDATA_INC=$STAGING/usr/local/include/pbdata
 LIBPBIHDF_INC=$STAGING/usr/local/include/hdf
  LIBBLASR_INC=$STAGING/usr/local/include/alignment
 LIBPBDATA_LIB=$STAGING/usr/local/lib
 LIBPBIHDF_LIB=$STAGING/usr/local/lib
  LIBBLASR_LIB=$STAGING/usr/local/lib

set +x
type module >& /dev/null || . /mnt/software/Modules/current/init/bash
module load git
module load gcc
module load ccache
if [[ $USER == bamboo ]]; then
  export CCACHE_DIR=/mnt/secondary/Share/tmp/bamboo.mobs.ccachedir
fi
export CCACHE_COMPILERCHECK='%compiler% -dumpversion'
module load boost
if [[ $BOOST_ROOT =~ /include ]]; then
  set -x
  BOOST_ROOT=$(dirname $BOOST_ROOT)
  set +x
fi
module load ninja
module load cmake
module load hdf5-tools
module load zlib
module load htslib
set -x


mkdir -p repos/pbbam/build
cd repos/pbbam/build
export CCACHE_BASEDIR=$PWD/..
rm -rf * && CFLAGS=-fPIC CXXFLAGS=-fPIC CMAKE_BUILD_TYPE=ReleaseWithAssert cmake -GNinja ..
ninja
rsync -avx ../include bin lib $STAGING/pbbam/
PBBAM_INC=$STAGING/pbbam/include
PBBAM_LIB=$STAGING/pbbam/lib
cd -

cd repos/blasr
export CCACHE_BASEDIR=$PWD
rm -f defines.mk
CXXFLAGS="-std=c++14" \
python configure.py --shared \
       PREFIX=dummy \
     HDF5_INC=$(pkg-config --cflags-only-I hdf5|awk '{print $1}'|sed -e 's/^-I//') \
     HDF5_LIB=$(pkg-config --libs-only-L hdf5|awk '{print $1}'|sed -e 's/^-L//') \
     ZLIB_LIB=$ZLIB_ROOT/lib \
    PBBAM_INC=$PBBAM_INC \
    PBBAM_LIB=$PBBAM_LIB \
    BOOST_INC=$BOOST_ROOT/include \
LIBPBDATA_INC=$LIBPBDATA_INC \
LIBPBIHDF_INC=$LIBPBIHDF_INC \
 LIBBLASR_INC=$LIBBLASR_INC  \
LIBPBDATA_LIB=$LIBPBDATA_LIB \
LIBPBIHDF_LIB=$LIBPBIHDF_LIB \
 LIBBLASR_LIB=$LIBBLASR_LIB  \
HTSLIB_CFLAGS=$(pkg-config --cflags htslib) \
  HTSLIB_LIBS=$(pkg-config --libs htslib)
sed -i -e 's/-lpbihdf/-llibcpp/;s/-lblasr//;s/-lpbdata//' defines.mk

VERBOSE=1 make
VERBOSE=1 CXXFLAGS="-std=c++14 -O3 -g" make -C utils
cp -a blasr          $STAGING/usr/local/bin/
cp -a utils/sawriter $STAGING/usr/local/bin/
if [[ $bamboo_planRepository_branchName == master ]]; then
  VERSION=$(git rev-parse --short HEAD)
  NEXUS_URL=http://ossnexus.pacificbiosciences.com/repository/maven-releases/pacbio/sat/blasr/$(git rev-parse --short HEAD)
elif [[ $bamboo_planRepository_branchName == develop ]]; then
  VERSION=SNAPSHOT$(git rev-parse --short HEAD)
  NEXUS_URL=http://ossnexus.pacificbiosciences.com/repository/maven-snapshots/pacbio/sat/blasr/$(git rev-parse --short HEAD)
else
  VERSION=$(git rev-parse --short HEAD)
  NEXUS_URL=
fi
cd -
 
(cd staging && tar zcf ../tarballs/blasr-${VERSION}.tgz usr/local/bin/{blasr,sawriter})
if [[ -z $NEXUS_URL ]]; then
  exit 0
fi
md5sum  blasr-${VERSION}.tgz | awk -e '{print $1}' >| ../blasr-${VERSION}.tgz.md5
sha1sum blasr-${VERSION}.tgz | awk -e '{print $1}' >| ../blasr-${VERSION}.tgz.sha1

curl -vn --upload-file ../blasr-${VERSION}.tgz \
  $NEXUS_URL/blasr-${VERSION}.tgz
curl -vn --upload-file ../blasr_libcpp-${VERSION}.tgz.md5 \
  $NEXUS_URL/blasr-${VERSION}.tgz.md5
curl -vn --upload-file ../blasr_libcpp-${VERSION}.tgz.sha1 \
  $NEXUS_URL/blasr-${VERSION}.tgz.sha1
