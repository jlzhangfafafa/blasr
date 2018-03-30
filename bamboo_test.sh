#!/bin/bash
set -e
type module >& /dev/null || . /mnt/software/Modules/current/init/bash
module load gcc git cram hdf5-tools zlib htslib samtools
mkdir -p test-reports

TEST_REPORTS=$PWD/test-reports
PATH=$PWD/repos/blasr:$PATH
PATH=$PWD/repos/blasr/utils:$PATH
LD_LIBRARY_PATH=$PWD/staging/pbbam/lib:$PWD/staging/usr/local/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH PATH
#blasr /pbi/dept/secondary/siv/testdata/BlasrTestData/ctest/data/ecoli_subset.fasta /pbi/dept/secondary/siv/testdata/BlasrTestData/ctest/data/ecoli_reference.fasta --bam --out ecoli_subset.bam --nproc 2
#exit

cd repos/blasr
make -f cram.mk \
    VERBOSE=1 DEBUG=1 \
    XUNIT="--xunit-file=$TEST_REPORTS/blasr-cram_xunit.xml" \
    cramfast
chmod +w -R .
