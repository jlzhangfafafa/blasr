Set up
  $ mkdir -p $OUTDIR

Test dataset.xml as input
  $ $BLASR_EXE $DATDIR/test_dataset/chunking.subreadset.xml $DATDIR/ecoli_reference.fasta -m 4 --out $OUTDIR/chunking.m4 --bestn 1 && echo $?
  [INFO]* (glob)
  [INFO]* (glob)
  0
Test filters in dataset.xml is respected.
  $ cat $OUTDIR/chunking.m4 | wc -l
  9

Test dataset.xml --bam output
  $ $BLASR_EXE $DATDIR/test_dataset/chunking.subreadset.xml $DATDIR/ecoli_reference.fasta --bam --out $OUTDIR/chunking.bam  && echo $?
  [INFO]* (glob)
  [INFO]* (glob)
  0

Test dataset.xml --concordant
  $ $BLASR_EXE $DATDIR/test_dataset/chunking.subreadset.xml $DATDIR/ecoli_reference.fasta --bam --out $OUTDIR/chunking.concordant.bam --concordant && echo $?
  [INFO]* (glob)
  [INFO]* (glob)
  0

Test dataset with no filters (to make sure that an empty filter does not discard all bam records.)
  $ $BLASR_EXE $DATDIR/test_dataset/nofilter.subreadset.xml $DATDIR/ecoli_reference.fasta --bam --out $OUTDIR/nofilter.bam --concordant --bestn 1 && echo $?
  [INFO]* (glob)
  [INFO]* (glob)
  0

  $ $SAMTOOLS_EXE view $OUTDIR/nofilter.bam|wc -l
  131


Test dataset with --concordant is on
  $ $BLASR_EXE $DATDIR/test_dataset/nofilter.subreadset.xml $DATDIR/bamConcordantRef.fasta --bam --concordant --refineConcordantAlignments --bestn 1 --out $OUTDIR/datasetConcordant.bam --holeNumbers 1898 && echo $?
  [INFO]* (glob)
  [INFO]* (glob)
  0

  $ $SAMTOOLS_EXE view $OUTDIR/datasetConcordant.bam | cut -f 4
  ??? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
  3?? (glob)
