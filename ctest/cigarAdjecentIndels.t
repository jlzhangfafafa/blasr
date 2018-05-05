Set up
  $ mkdir -p $OUTDIR

Without --allowAdjacentIndels, adjacent indels should not exist in SAM/BAM CIGAR strings
  $ $BLASR_EXE $DATDIR/test_dataset/nofilter.subreadset.xml $DATDIR/ecoli_reference.fasta --bam --out $OUTDIR/noAdjacentIndels.bam --concordant --refineConcordantAlignments --bestn 1 && echo $?
  [INFO]* (glob)
  [INFO]* (glob)
  0

  $ $SAMTOOLS_EXE view $OUTDIR/noAdjacentIndels.bam |cut -f 6 > $TMP1

  $ grep 'ID' $TMP1 |wc -l
  0

  $ grep 'DI' $TMP1 |wc -l
  0

With --allowAdjacentIndels
  $ $BLASR_EXE $DATDIR/test_dataset/nofilter.subreadset.xml $DATDIR/ecoli_reference.fasta --bam --out $OUTDIR/allowAdjacentIndels.bam --concordant --bestn 1 --allowAdjacentIndels && echo $?
  [INFO]* (glob)
  [INFO]* (glob)
  0
