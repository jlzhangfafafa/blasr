#!/bin/bash -e
rm -rf prebuilt
rm -rf deployment
mkdir -p prebuilts
HTSLIB=`/bin/ls -t tarballs/htslib-*-bin.tgz|head -1`
PBBAM=`/bin/ls -t tarballs/pbbam*-x86_64.tgz|head -1`
HAVE_HTSLIB=$PWD/prebuilts/`basename $HTSLIB .tgz`
mkdir -p $HAVE_HTSLIB
tar zxvf $HTSLIB -C $HAVE_HTSLIB
tar zxvf $PBBAM -C prebuilts
HAVE_PBBAM=`/bin/ls -d $PWD/prebuilts/pbbam-*|head -1`

type module >& /dev/null || . /mnt/software/Modules/current/init/bash
module load gcc/4.9.2
module load git/2.8.3
module load ccache/3.2.3

cat > pitchfork/settings.mk << EOF
PREFIX            = $PWD/deployment
CCACHE_DIR        = /net/flash/localdisk/scratch/bamboo.ccache
SCCACHE_DIR       = /mnt/secondary/Share/tmp/bamboo.sccache
DISTFILES         = $PWD/.distfiles
# from Herb
HAVE_OPENSSL      = /mnt/software/o/openssl/1.0.2a
HAVE_PYTHON       = /mnt/software/p/python/2.7.9/bin/python
HAVE_BOOST        = /mnt/software/b/boost/1.58.0
HAVE_ZLIB         = /mnt/software/z/zlib/1.2.8
HAVE_SAMTOOLS     = /mnt/software/s/samtools/1.3.1mobs
HAVE_NCURSES      = /mnt/software/n/ncurses/5.9
# from MJ
HAVE_HDF5         = /mnt/software/a/anaconda2/4.2.0
HAVE_OPENBLAS     = /mnt/software/o/openblas/0.2.14
HAVE_CMAKE        = /mnt/software/c/cmake/3.2.2/bin/cmake
#
HAVE_PBBAM        = $HAVE_PBBAM
#pbbam_REPO        = $PWD/repos/pbbam
blasr_REPO        = $PWD/repos/blasr
blasr_libcpp_REPO = $PWD/repos/blasr_libcpp
HAVE_HTSLIB       = $HAVE_HTSLIB
#htslib_REPO       = $PWD/repos/htslib
EOF

make -j -C pitchfork blasr cram
mkdir -p tarballs

bash << EOF2
source deployment/setup-env.sh 
pbindex --version|awk '{print \$1}' > VERSION.pbbam
blasr   --version|awk '{print \$2}' > VERSION.blasr
EOF2

cd deployment
rm -rf ../tarballs && mkdir -p ../tarballs
tar zcf ../tarballs/htslib-${bamboo_planRepository_2_revision}.tgz `grep -v ^# var/pkg/htslib`
tar zcf ../tarballs/blasr_libcpp-${bamboo_planRepository_3_revision}.tgz `grep -v ^# var/pkg/blasr_libcpp`
tar zcf ../tarballs/blasr-`cat ../VERSION.blasr`.tgz `grep -v ^# var/pkg/blasr`
tar zcf ../tarballs/pbbam-`cat ../VERSION.pbbam`.tgz `grep -v ^# var/pkg/pbbam`

echo ${bamboo_planRepository_1_branch}
