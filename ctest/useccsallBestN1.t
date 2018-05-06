Set up
  $ mkdir -p $OUTDIR

Test --useccsall with bestn = 1
  $ $BLASR_EXE $DATDIR/ccstest.fofn $DATDIR/ccstest_ref.fasta --bestn 1 --useccsall --bam --out $OUTDIR/useccsall.bam --holeNumbers 76772
  [INFO]* (glob)
  [INFO]* (glob)
  $ $SAMTOOLS_EXE view -h $OUTDIR/useccsall.bam > $OUTDIR/useccsall.sam
  $ sed -n '9,$ p' $OUTDIR/useccsall.sam |cut -f 1-4 > $TMP1
  $ sed -n '9,$ p' $STDDIR/$UPDATEDATE/useccsall.sam | cut -f 1-4 > $TMP2
  $ diff $TMP1 $TMP2
  $ rm $TMP1 $TMP2
