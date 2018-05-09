Set up
  $ mkdir -p $OUTDIR

Similar to bamConcordant.t, just added option to place gap consistently.

Test using bam as input, use --concordant
  $ Q=$DATDIR/test_bam/tiny_bam.fofn
  $ T=$DATDIR/bamConcordantRef.fasta 
  $ OUTPREFIX=pgc-concordant
  $ O=$OUTDIR/$OUTPREFIX.bam

  $ $BLASR_EXE $Q $T --placeGapConsistently --bam --concordant --refineConcordantAlignments --bestn 1 --out $O
  [INFO]* (glob)
  [INFO]* (glob)

Check whether sam out and bam out have identical alignments, not checking qvs
  $ $SAMTOOLS_EXE view $O |cut -f 4
  1
  1
  733
  613
  600
  596
  600
  596
  596
  596
  600
  633
  612
  596
  596
  596
 
  $ LOG=$OUTDIR/pgc-bam-concordant.log
  $ O=$OUTDIR/pgc-bam-concordant-2.bam
  $ $BLASR_EXE /pbi/dept/secondary/siv/testdata/SA3-RS/lambda/2372215/0007_tiny/Analysis_Results/m150404_101626_42267_c100807920800000001823174110291514_s1_p0.1.subreads.bam $DATDIR/lambda_ref.fasta --placeGapConsistently -m 4 --concordant --bestn 1 --holeNumbers 17417 --out $O && echo $?
  [INFO]* (glob)
  [INFO]* (glob)
  0
