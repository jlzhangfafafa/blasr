Set up
  $ mkdir -p $OUTDIR
Test blasr on ecoli.
Test blasr with --bam

# The following job takes a very long time to finish, let us use a subset of reads instead
#See $STDOUT/ecoli_v1.4.sam for 1.4 output.
# $STDOUT/ecoli_2014_03_28.sam for bug before mapQV for affineAlign/align without QV is fixed.
  $ rm -rf $OUTDIR/ecoli_subset.bam
  $ rm -rf $OUTDIR/ecoli_subset.sam
  $ $BLASR_EXE $DATDIR/ecoli_subset.fasta $DATDIR/ecoli_reference.fasta --bam --out $OUTDIR/ecoli_subset.bam --nproc 15
  [INFO]* (glob)
  WARNING, BAM output without PacBio BAM or DATASET input is not supported by PacBio.
  [INFO]* (glob)

  $ $SAMTOOLS_EXE view -h $OUTDIR/ecoli_subset.bam > $OUTDIR/ecoli_subset.sam
  $ sed -n '5,$ p' $OUTDIR/ecoli_subset.sam | sort | cut -f 1-11 > $OUTDIR/ecoli_subset_out
  $ sed -n '5,$ p' $STDDIR/2016_10_20/ecoli_subset.sam | sort | cut -f 1-11 > $OUTDIR/ecoli_subset_std
  $ diff $OUTDIR/ecoli_subset_out $OUTDIR/ecoli_subset_std
# 2015_03_08 --> changelist 148101, 148080 updated read group id; 148100 updated TLEN
# 2015_04_09 --> changelist 148796, updated read group id
