Set up
  $ mkdir -p $OUTDIR

Test using *.ccs.h5 as input
# The results should be exactly the same as 
# blasr $DATDIR/ccsasinput_bas.fofn $DATDIR/ccsasinput.fasta -m 4 --out tmp.m4 --useccsdenovo
  $ rm -rf $OUTDIR/ccsasinput.m4
  $ $BLASR_EXE $DATDIR/ccsasinput.fofn $DATDIR/ccsasinput.fasta -m 4 --out $OUTDIR/ccsasinput.m4
  [INFO]* (glob)
  [INFO]* (glob)
  $ diff $OUTDIR/ccsasinput.m4 $STDDIR/ccsasinput_2014_06_10.m4
