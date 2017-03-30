Set up
  $ . $TESTDIR/setup.sh

Test blasr with --noSplitSubreads
#  $ rm -rf $OUTDIR/lambda_bax_noSplitSubreads.m4
#  $ $EXEC $DATDIR/lambda_bax.fofn $DATDIR/lambda_ref.fasta --noSplitSubreads -m 4 --out lambda_bax_noSplitSubreads_tmp.m4 --nproc 15
#  [INFO]* (glob)
#  [INFO]* (glob)
#  $ sort lambda_bax_noSplitSubreads_tmp.m4 > $OUTDIR/lambda_bax_noSplitSubreads.m4
#  $ diff $OUTDIR/lambda_bax_noSplitSubreads.m4 $STDDIR/lambda_bax_noSplitSubreads.m4
# This test takes a long time, use a subset instad.

  $ rm -rf $OUTDIR/lambda_bax_noSplitSubreads_subset.m4
  $ $EXEC $DATDIR/lambda_bax.fofn $DATDIR/lambda_ref.fasta --noSplitSubreads -m 4 --out $OUTDIR/lambda_bax_noSplitSubreads_tmp_subset.m4 --nproc 15 --holeNumbers 1--1000 --sa $DATDIR/lambda_ref.sa
  [INFO]* (glob)
  [INFO]* (glob)
  $ sort $OUTDIR/lambda_bax_noSplitSubreads_tmp_subset.m4 > $OUTDIR/lambda_bax_noSplitSubreads_subset.m4
  $ diff $OUTDIR/lambda_bax_noSplitSubreads_subset.m4 $STDDIR/lambda_bax_noSplitSubreads_subset.m4

# Test key command of unrolled resequencing, check bam header and alignments in output
  $ outbam=$OUTDIR/unrolled-4mer.bam
  $ outsam=$OUTDIR/unrolled-4mer.sam
  $ query=$DATDIR/unrolled/m54006_151021_185942.subreadset.xml
  $ ref=$DATDIR/unrolled/All4mer_V2_11_V2_13_V2_15_V2_44_circular_72x_l50256.fasta
  $ stdsam=$STDDIR/unrolled-4mer.sam
  $ rm -rf $outbam $outsam
  $ $EXEC $query $ref --out $outbam --noSplitSubreads --fastMaxInterval --bam
  [INFO]* (glob)
  [INFO]* (glob)
  $ $SAMTOOLS view -h $outbam -o $outsam
  $ grep -v '^@PG' $outsam > $TMP1 && grep -v '^@PG' $stdsam > $TMP2 && diff $TMP1 $TMP2
  $ grep '@RG' $outsam
  @RG\tID:e6043908* (glob)
  $ grep 'RG:Z:e6043908' $outsam |wc -l
  4


  $ query=$DATDIR/unrolled/m54006_151021_185942.subreads.bam
  $ outbam=$OUTDIR/unrolled-4mer-bam-in.bam
  $ outsam=$OUTDIR/unrolled-4mer-bam-in.sam
  $ rm -rf $outbam $outsam
  $ $EXEC $query $ref --out $outbam --noSplitSubreads --fastMaxInterval --bam
  [INFO]* (glob)
  [INFO]* (glob)
  $ $SAMTOOLS view -h $outbam -o $outsam
  $ grep -v '^@PG' $outsam > $TMP1 && grep -v '^@PG' $stdsam > $TMP2 && diff $TMP1 $TMP2
  $ grep '@RG' $outsam
  @RG\tID:e6043908* (glob)
  $ grep 'RG:Z:e6043908' $outsam |wc -l
  4

# Test --polymerase and implicit zmw mode
  $ $EXEC $DATDIR/m54075_161031_164015.subreadset.xml $DATDIR/R_palustris_CGA009_pBR322_plasmidbell_4361bp_circular_6x_l52872.fasta --noSplitSubreads --out $OUTDIR/zmw.bam --bestn 1 --forwardOnly --hitPolicy leftmost --bam
  [INFO]* (glob)
  [INFO]* (glob)
  $ $SAMTOOLS view $OUTDIR/zmw.bam | cut -f 1 | sort -n
  m54075_161031_164015/13697608/0_14300
  m54075_161031_164015/18482117/0_31369
  m54075_161031_164015/23135225/0_18868
  m54075_161031_164015/27788237/0_37429
  m54075_161031_164015/27853378/0_30525
  m54075_161031_164015/64881154/0_30292

  $ $EXEC $DATDIR/m54075_161031_164015.subreadset.xml $DATDIR/R_palustris_CGA009_pBR322_plasmidbell_4361bp_circular_6x_l52872.fasta --noSplitSubreads --out $OUTDIR/polymerase.bam --bestn 1 --forwardOnly --hitPolicy leftmost --bam --polymerase
  [INFO]* (glob)
  [INFO]* (glob)
  $ $SAMTOOLS view $OUTDIR/polymerase.bam | cut -f 1 | sort -n
  m54075_161031_164015/13697608/7904_9620
  m54075_161031_164015/18482117/669_31369
  m54075_161031_164015/23135225/18205_18445
  m54075_161031_164015/27788237/23982_27080
  m54075_161031_164015/64881154/6575_30292